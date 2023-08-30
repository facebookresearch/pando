#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import sys
import os
import logging


if __name__ == "__main__":
    if "basler" in sys.argv:
        os.environ["CAM_MODE"] = "basler"
    else:
        os.environ["CAM_MODE"] = "hamamatsu"

if os.environ["CAM_MODE"] == "basler":
    from bci.systems.sawyer.messages import BaslerImageMessage as CameraImageMessage
else:
    from bci.systems.sawyer.messages import HamamatsuImageMessage as CameraImageMessage

import time
import asyncio

import labgraph as df
import pyqtgraph as pg
from argparse import ArgumentParser
from pyqtgraph.Qt import QtGui, QtWidgets, QtCore
from typing import Tuple, Dict

from .plots import (
    LinePlot,
    LinePlotConfig,
    Mode,
    BasHamHeatMap,
    BasHamHeatMapConfig,
    BarPlot,
    PlotConfig,
    RoIPlot,
    RoIConfig,
    RectMsg,
    LabelPanel,
    LabelPanelConfig,
    LabelPanelState,
    LinePlotPanel,
    ContrastMsg,
    ContrastGroup,
    RALenMsg,
)
from .viz_nodes import (
    FrameInfoMsg,
    FrameInfoConverter,
    Filter,
    FilterConfig,
    PollerSerializer,
    PollerSerializerConfig,
)

from bci.nodes.profiler_node import ProfileNode


class WindowConfig(df.Config):
    hamamatsu_mode: bool = True


class Window(ProfileNode):
    OUTPUT = df.Topic(RALenMsg)

    PLOT: LinePlot
    PLOT_2: BasHamHeatMap
    PLOT_3: BarPlot
    PLOT_4: LabelPanel
    PLOT_5: RoIPlot

    timer: QtCore.QTimer

    ra_spinbox: QtGui.QSpinBox
    ra_win_len: int = 10

    _update_timer: float = 0.0
    _update_counter: int = 0

    def update(self):
        # Measure the update rate

        start_time = time.time()

        self.PLOT.update()
        self.PLOT_2.update()
        self.PLOT_3.update()
        self.PLOT_4.update()
        self.PLOT_5.update()

        QtGui.QApplication.instance().processEvents()

        end_time = time.time()

        self._update_timer += end_time - start_time
        self._update_counter += 1
        # logging.error("DJF")

    @df.main
    def run_plot(self) -> None:
        win = pg.GraphicsLayoutWidget()
        logging.info("camera_mode, Window config: %s", self.config.hamamatsu_mode)
        if self.config.hamamatsu_mode:
            win.setWindowTitle("Hamamatsu Camera Visualizations")
        else:
            win.setWindowTitle("Basler Camera Visualizations")

        plot_1 = self.PLOT_2.build()
        # plot_2.setLabel('left', 'Height (1920px)')
        # plot_2.setLabel('bottom', 'Width (1200px)')
        # plot_2.setTitle("Image Intensity Heatmap")
        win.addItem(plot_1, 0, 0)

        plot_2 = self.PLOT.build()
        win.addItem(plot_2, 1, 0)

        plot_3 = self.PLOT_3.build()
        plot_3.setLabel("left", "Frequency")
        plot_3.setLabel("bottom", "Intensity")
        plot_3.setTitle("Intensity Histogram")
        win.addItem(plot_3, 0, 1)

        plot_4 = self.PLOT_4.build()
        win.addItem(plot_4, 1, 1)

        proxy = QtGui.QGraphicsProxyWidget()
        self.ra_spinbox = QtGui.QSpinBox()
        self.ra_spinbox.setPrefix("Rolling Average Window Length (in Frames): \t")
        # defaults must be matched in contrast_group state and setup
        self.ra_spinbox.setValue(10)
        self.ra_spinbox.setMinimum(1)
        self.ra_spinbox.setMaximum(50)
        self.ra_spinbox.setStyleSheet(
            "QSpinBox"
            "{"
            "color : white;"
            "background-color : black;"
            "font-size : 10pt;"
            "}"
        )
        self.ra_spinbox.valueChanged.connect(self.ra_value_changed)
        proxy.setWidget(self.ra_spinbox)

        p3 = win.addLayout(2, 0)
        p3.addItem(proxy, 1, 1)

        imv = self.PLOT_5.build()
        imv.show()

        # Set up a timer to update the visualization at 30FPS
        self.timer = QtCore.QTimer()
        self.timer.setInterval(500)
        self.timer.start()
        self.timer.timeout.connect(self.update)

        win.resize(1200, 1000)
        win.show()
        QtGui.QApplication.instance().exec_()
        raise df.NormalTermination

    def ra_value_changed(self) -> None:
        self.ra_win_len = self.ra_spinbox.value()
        self.ra_value_emit()

    # hacky but it works
    @df.publisher(OUTPUT)
    async def ra_value_emit(self) -> RALenMsg:
        ra_win_len_old = 10
        while True:
            await asyncio.sleep(0.01)
            if self.ra_win_len != ra_win_len_old:
                logging.info("emitting message")
                ra_win_len_old = self.ra_win_len
                yield self.OUTPUT, RALenMsg(ra_win_len=self.ra_win_len)

    def cleanup(self) -> None:
        QtGui.QApplication.instance().quit()
        super(Window, self).cleanup()
        logging.error(
            "Number of Updates: %d Time Spent: %.3f FPS: %.2f",
            self._update_counter,
            self._update_timer,
            self._update_counter / self._update_timer,
        )


class SimpleVizGroupConfig(df.Config):
    hamamatsu_mode: bool = True


class SimpleVizGroup(df.Group):
    PLOT: LinePlotPanel
    PLOT_2: BasHamHeatMap
    PLOT_3: BarPlot
    PLOT_4: LabelPanel
    PLOT_5: RoIPlot
    WINDOW: Window

    CONTRAST_INPUT = df.Topic(ContrastMsg)
    CAM_INPUT = df.Topic(CameraImageMessage)
    LABEL_INPUT = df.Topic(ContrastMsg)
    LABEL_INPUT_2 = df.Topic(FrameInfoMsg)
    ROI_OUTPUT = df.Topic(RectMsg)
    WIN_OUTPUT = df.Topic(RALenMsg)

    def connections(self) -> df.Connections:
        return (
            (self.CONTRAST_INPUT, self.PLOT.INPUT),
            (self.CAM_INPUT, self.PLOT_2.INPUT),
            (self.CAM_INPUT, self.PLOT_3.INPUT),
            (self.LABEL_INPUT, self.PLOT_4.CR_INPUT),
            (self.LABEL_INPUT_2, self.PLOT_4.FIR_INPUT),
            (self.CAM_INPUT, self.PLOT_5.INPUT),
            (self.PLOT_5.OUTPUT, self.ROI_OUTPUT),
            (self.WINDOW.OUTPUT, self.WIN_OUTPUT),
        )

    def setup(self) -> None:
        logging.info("camera_mode,  SimpleVizGroup: %s", self.config.hamamatsu_mode)
        self.WINDOW.configure(WindowConfig(hamamatsu_mode=self.config.hamamatsu_mode))
        self.PLOT_2.configure(
            BasHamHeatMapConfig(
                data="image_data",
                width="width",
                height="height",
                channel_map="channel_map",
                hamamatsu=self.config.hamamatsu_mode,
                external_timer=True,
            )
        )
        self.PLOT_3.configure(
            PlotConfig(x_field="x_data", y_field="y_data", external_timer=True,)
        )
        self.PLOT_4.configure(
            LabelPanelConfig(
                k_std="contrast_std",
                histo_mean="pixel_intensity_mean",
                frame_number="frame_number",
                external_timer=True,
            )
        )
        self.PLOT_5.configure(
            RoIConfig(
                data="data",
                width="width",
                height="height",
                channel_map="channel_map",
                hamamatsu=self.config.hamamatsu_mode,
                external_timer=True,
            )
        )
        self.WINDOW.PLOT = self.PLOT
        self.WINDOW.PLOT_2 = self.PLOT_2
        self.WINDOW.PLOT_3 = self.PLOT_3
        self.WINDOW.PLOT_4 = self.PLOT_4
        self.WINDOW.PLOT_5 = self.PLOT_5

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (
            self.PLOT,
            self.PLOT_2,
            self.PLOT_3,
            self.PLOT_4,
            self.PLOT_5,
            self.WINDOW,
        )


# TODO rename this
class BaslerConfig(df.Config):
    read_addr: str = "tcp://localhost:5060"
    hamamatsu_mode: bool = True


class BaslerViz(df.Graph):
    POLLER: PollerSerializer
    FILTER: Filter
    FICONVERTER: FrameInfoConverter
    VIZ: SimpleVizGroup
    CG: ContrastGroup

    def setup(self) -> None:
        logging.info("camera_mode in BaslerViz: %s", self._config.hamamatsu_mode)
        self.POLLER.configure(
            PollerSerializerConfig(
                read_addr=self._config.read_addr, zmq_topic="pando.camera_image"
            )
        )
        self.VIZ.configure(
            SimpleVizGroupConfig(hamamatsu_mode=self._config.hamamatsu_mode)
        )
        if self._config.hamamatsu_mode:
            self.FILTER.configure(FilterConfig(filter_ratio=35))
        else:
            self.FILTER.configure(FilterConfig(filter_ratio=75))

    def connections(self) -> df.Connections:
        return (
            (self.POLLER.topic, self.FILTER.INPUT),
            (self.FILTER.OUTPUT, self.VIZ.CAM_INPUT),
            (self.FILTER.OUTPUT, self.VIZ.CAM_INPUT),
            (self.FILTER.OUTPUT, self.FICONVERTER.INPUT),
            (self.FILTER.OUTPUT, self.VIZ.CAM_INPUT),
            (self.CG.OUTPUT, self.VIZ.CONTRAST_INPUT),
            (self.CG.OUTPUT, self.VIZ.LABEL_INPUT),
            (self.FICONVERTER.OUTPUT, self.VIZ.LABEL_INPUT_2),
            (self.POLLER.topic, self.CG.CON_INPUT),
            (self.VIZ.ROI_OUTPUT, self.CG.RSR_INPUT),
            (self.VIZ.WIN_OUTPUT, self.CG.RAR_INPUT),
        )

    def logging(self) -> Dict[str, df.Topic]:
        return {"ContrastMsg": self.CG.OUTPUT}

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (
            self.POLLER,
            self.FICONVERTER,
            self.VIZ,
            self.FILTER,
            self.CG,
        )


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument(
        "--read-addr",
        help="The ZMQ address to read data from",
        default="tcp://127.0.0.1:5060",
    )
    parser.add_argument(
        "--camera-mode",
        help="Camera to run viz for (basler or hamamatsu)",
        default="hamamatsu",
    )
    args = parser.parse_args()
    hamamatsu_mode: bool
    if args.camera_mode == "hamamatsu":
        hamamatsu_mode = True
    elif args.camera_mode == "basler":
        hamamatsu_mode = False
    else:
        raise ValueError('camera_mode argument neither "basler" nor "hamamatsu"')

    log_filepath = os.path.join(os.getcwd(), "hdf5_logs")
    logging.info("Logging to: %s", log_filepath)
    if not os.path.exists(log_filepath):
        os.makedirs(log_filepath)
        logging.info("Making directory %s", log_filepath)
    options = df.RunnerOptions(
        logger_config=df.LoggerConfig(
            output_directory=log_filepath,
            recording_name=time.strftime("contrast-%Y%m%d-%H%M%S"),
        ),
    )

    graph = BaslerViz()
    graph.configure(
        BaslerConfig(read_addr=args.read_addr, hamamatsu_mode=hamamatsu_mode)
    )
    runner = df.ParallelRunner(graph, options=options)
    runner.run()
