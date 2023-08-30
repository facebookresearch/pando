#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui

from bci.systems.sawyer.messages import CountRateMessage, HistogramMessage
from bci.systems.sawyer.nodes import CRHistogramConfig, CRHistogramNode
from bci.libraries.labgraph_viz.plots import (
    BarPlot,
    ColorMap,
    ColorMapConfig,
    HeatMap,
    HeatMapConfig,
    PlotConfig,
)


class PyQtApplication(df.Node):
    # Plots
    CR_HEATMAP: HeatMap
    CR_COLOR_MAP: ColorMap
    CR_HISTOGRAM_PLOT: BarPlot

    @df.main
    def run_plot(self) -> None:
        cr_win = pg.GraphicsWindow()
        cr_win.resize(400, 640)
        view_box = cr_win.addViewBox()
        view_box.invertY()
        view_box.addItem(self.CR_HEATMAP.build())
        color_bar = self.CR_COLOR_MAP.build()
        color_bar.setFixedWidth(50)
        cr_win.addItem(color_bar)
        cr_win.nextRow()
        cr_win.addItem(self.CR_HISTOGRAM_PLOT.build())

        self.timer = QtCore.QTimer()
        self.timer.setInterval(100)
        self.timer.start()
        self.timer.timeout.connect(self.update)

        QtGui.QApplication.instance().exec_()

        self.timer.stop()
        self.CR_COLOR_MAP.stop()
        self.CR_HEATMAP.stop()
        self.CR_HISTOGRAM_PLOT.stop()

    def update(self) -> None:
        self.CR_COLOR_MAP.update()
        self.CR_HEATMAP.update()
        self.CR_HISTOGRAM_PLOT.update()

    def cleanup(self) -> None:
        QtGui.QApplication.instance().quit()


class CountRateWindow(df.Group):
    # Messages
    COUNT_RATES = df.Topic(CountRateMessage)

    # Nodes
    CR_HISTOGRAM: CRHistogramNode

    # Application
    APPLICATION: PyQtApplication

    # Plots
    CR_HEATMAP: HeatMap
    CR_COLOR_MAP: ColorMap
    CR_HISTOGRAM_PLOT: BarPlot

    def setup(self) -> None:
        # Nodes
        self.CR_HISTOGRAM.configure(CRHistogramConfig(bins=32))

        # Plots
        self.CR_HEATMAP.configure(
            HeatMapConfig(
                data="count_rates",
                channel_map="channel_map",
                shape=((32, 32)),
                external_timer=True,
            )
        )
        self.CR_COLOR_MAP.configure(
            ColorMapConfig(
                data="count_rates",
                channel_map="channel_map",
                shape=((32, 32)),
                external_timer=True,
            )
        )
        self.CR_HISTOGRAM_PLOT.configure(
            PlotConfig(
                x_field="x",
                y_field="y",
                style={
                    "setLabels": {
                        "title": "Count Rate Histogram",
                        "left": "Number of Channels",
                        "bottom": "Counts Per Second",
                    }
                },
                external_timer=True,
            )
        )
        self.APPLICATION.CR_HEATMAP = self.CR_HEATMAP
        self.APPLICATION.CR_COLOR_MAP = self.CR_COLOR_MAP
        self.APPLICATION.CR_HISTOGRAM_PLOT = self.CR_HISTOGRAM_PLOT

    def connections(self) -> df.Connections:
        return (
            (self.COUNT_RATES, self.CR_HISTOGRAM.INPUT),
            (self.COUNT_RATES, self.CR_HEATMAP.INPUT),
            (self.COUNT_RATES, self.CR_COLOR_MAP.INPUT),
            (self.CR_HISTOGRAM.CR_HISTOGRAM, self.CR_HISTOGRAM_PLOT.INPUT),
        )
