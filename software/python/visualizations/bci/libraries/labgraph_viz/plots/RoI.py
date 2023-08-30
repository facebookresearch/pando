#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Any, Dict, Optional, Tuple
from dataclasses import field

import contrast_calcs
import logging
import asyncio

import labgraph as df
import pyqtgraph as pg
import numpy as np
from pyqtgraph.Qt import QtCore, QtGui
from pgcolorbar.colorlegend import ColorLegendItem
from threading import Timer

from .common import TIMER_INTERVAL


# need to override the click and drag functions so that the user is prevented from dragging the plot
class CustomPlotItem(pg.PlotItem):
    def __init__(self, *args, **kwds):
        super().__init__(*args, **kwds)

    def mouseClickEvent(self, evt):
        pass

    def mouseDragEvent(self, evt):
        pass


class RoIState(df.State):
    """
    State for a RoIMap
    plot: The pyqt plot that we are updating
    data: The data for the roimap
    """

    plot: Optional[Any] = None
    avg_data: np.ndarray = None
    ra_win_len: int = 10
    timer: Optional[QtCore.QTimer] = None
    roi_1: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_2: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_3: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_4: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_1_active: bool = False
    roi_2_active: bool = False
    roi_3_active: bool = False
    roi_4_active: bool = False


class RoIConfig(df.Config):
    """
    Configuration for a RoIMap
    data: The field on incoming messages for data
    channel_map: The field on the incoming messages for channel_map
    style: Pass any plot functions and params to run in style()
    shape: The shape of the data to visualize
    color_map: The name of the color map to use
    external_timer: Specify whether you are providing an external timer to call update() manually
    """

    data: str = "image_data"
    width: str = "width"
    height: str = "height"
    channel_map: str = None
    style: Dict[str, Any] = field(default_factory=dict)
    hamamatsu: bool = True
    color_map: str = "jet"
    external_timer: bool = False


# class for sending selection regions of interest.
# need to send top left and bottom right corners to specify rectangle
# also include the number of the region it belongs to
class RectMsg(df.Message):
    roi_1: Tuple[slice, slice]
    roi_2: Tuple[slice, slice]
    roi_3: Tuple[slice, slice]
    roi_4: Tuple[slice, slice]
    roi_1_active: bool = False
    roi_2_active: bool = False
    roi_3_active: bool = False
    roi_4_active: bool = False


class RoIPlot(df.Node):
    INPUT = df.Topic(df.Message)
    OUTPUT = df.Topic(df.Message)

    state: RoIState
    handle_1: pg.ROI
    handle_2: pg.ROI
    handle_3: pg.ROI
    handle_4: pg.ROI
    msg_counter: int = 0
    width: int = 2304
    height: int = 2304
    imageItem: pg.ImageItem
    colorLegendItem: ColorLegendItem
    label_region_1_tl: pg.LabelItem
    label_region_2_tl: pg.LabelItem
    label_region_3_tl: pg.LabelItem
    label_region_4_tl: pg.LabelItem
    roi_1: pg.ROI
    roi_2: pg.ROI
    roi_3: pg.ROI
    roi_4: pg.ROI
    imv: CustomPlotItem
    graphicsLayoutWidget: pg.GraphicsLayoutWidget
    ra_spinbox: QtGui.QSpinBox
    rolling_avg = contrast_calcs.RollingAverage(5308416)

    def setup(self) -> None:
        logging.info("calling setup")
        if not self.config.hamamatsu:
            logging.info("Running RoI in Basler Mode")
            self.rolling_avg = contrast_calcs.RollingAverage(2304000)
            self.width = 1920
            self.height = 1200
            self.state.roi_1 = (slice(0, self.width, None), slice(0, self.height, None))
            self.state.roi_2 = (slice(0, self.width, None), slice(0, self.height, None))
            self.state.roi_3 = (slice(0, self.width, None), slice(0, self.height, None))
            self.state.roi_4 = (slice(0, self.width, None), slice(0, self.height, None))

    def update(self) -> None:
        if self.state.plot is not None and self.state.avg_data is not None:
            self.imageItem.setImage(self.state.avg_data, axisOrder="row-major")

    @df.subscriber(INPUT)
    def got_message(self, message: df.Message) -> None:
        """
        Receive data from input topic
        and display on plot according to RoIConfig
        """
        if self.state.plot is not None:
            # redeclare if receiving Bassler data
            self.state.data = message.image_data
            self.state.avg_data = self.rolling_avg.Update(
                self.state.data, self.state.ra_win_len
            )

    def style(self) -> None:
        if self.config.style:
            for k, v in self.config.style.items():
                if isinstance(v, str):
                    v = (v,)
                if isinstance(v, dict):
                    getattr(self.state.plot, k)(**v)
                else:
                    getattr(self.state.plot, k)(*v)

    @df.publisher(OUTPUT)
    async def zmq_publisher(self) -> df.AsyncPublisher:
        # Wait for socket connection
        while True:
            await asyncio.sleep(0.2)
            rm = RectMsg(
                roi_1=self.state.roi_1,
                roi_2=self.state.roi_2,
                roi_3=self.state.roi_3,
                roi_4=self.state.roi_4,
                roi_1_active=self.state.roi_1_active,
                roi_2_active=self.state.roi_2_active,
                roi_3_active=self.state.roi_3_active,
                roi_4_active=self.state.roi_4_active,
            )
            yield self.OUTPUT, rm

    def format_roi_1_str(self) -> None:
        reg_1_tl_str = (
            "Region 1 Coordinates: bot left: (%s, %s) top right: (%s, %s)"
            % (
                str(self.state.roi_1[0].start),
                str(self.state.roi_1[1].start),
                str(self.state.roi_1[0].stop),
                str(self.state.roi_1[1].stop),
            )
        )
        self.label_region_1_tl.setText(text=reg_1_tl_str)

    def format_roi_2_str(self) -> None:
        reg_2_tl_str = (
            "Region 2 Coordinates: bot left: (%s, %s) top right: (%s, %s)"
            % (
                str(self.state.roi_2[0].start),
                str(self.state.roi_2[1].start),
                str(self.state.roi_2[0].stop),
                str(self.state.roi_2[1].stop),
            )
        )
        self.label_region_2_tl.setText(text=reg_2_tl_str)

    def format_roi_3_str(self) -> None:
        reg_3_tl_str = (
            "Region 3 Coordinates: bot left: (%s, %s) top right: (%s, %s)"
            % (
                str(self.state.roi_3[0].start),
                str(self.state.roi_3[1].start),
                str(self.state.roi_3[0].stop),
                str(self.state.roi_3[1].stop),
            )
        )
        self.label_region_3_tl.setText(text=reg_3_tl_str)

    def format_roi_4_str(self) -> None:
        reg_4_tl_str = (
            "Region 4 Coordinates: bot left: (%s, %s) top right: (%s, %s)"
            % (
                str(self.state.roi_4[0].start),
                str(self.state.roi_4[1].start),
                str(self.state.roi_4[0].stop),
                str(self.state.roi_4[1].stop),
            )
        )
        self.label_region_4_tl.setText(text=reg_4_tl_str)

    def pub_roi_1(self, roi) -> None:
        rect = roi.getArraySlice(self.imageItem.image, self.imageItem)
        logging.info("calling pub_roi_1, rect: %s", rect[0])
        self.state.roi_1 = rect[0]
        self.state.roi_1_active = True
        self.format_roi_1_str()

    def pub_roi_2(self, roi) -> None:
        rect = roi.getArraySlice(self.imageItem.image, self.imageItem)
        logging.info("calling pub_roi_2, rect: %s", rect[0])
        self.state.roi_2 = rect[0]
        self.state.roi_2_active = True
        self.format_roi_2_str()

    def pub_roi_3(self, roi) -> None:
        rect = roi.getArraySlice(self.imageItem.image, self.imageItem)
        logging.info("calling pub_roi_3, rect: %s", rect[0])
        self.state.roi_3 = rect[0]
        self.state.roi_3_active = True
        self.format_roi_3_str()

    def pub_roi_4(self, roi) -> None:
        rect = roi.getArraySlice(self.imageItem.image, self.imageItem)
        logging.info("calling pub_roi_4, rect: %s", rect[0])
        self.state.roi_4 = rect[0]
        self.state.roi_4_active = True
        self.format_roi_4_str()

    def handle_setup(self, width: int, height: int) -> None:
        logging.info("setting up handles with witdh: %s", width)
        self.roi_1 = pg.ROI(
            [0, 0],
            [width, height],
            scaleSnap=True,
            maxBounds=QtCore.QRectF(0, 0, width, height),
        )
        self.roi_1.addScaleHandle([1, 1], [0.5, 0.5])
        self.roi_1.addScaleHandle([0, 0], [0.5, 0.5])
        self.roi_2 = pg.ROI(
            [0, 0],
            [width, height],
            scaleSnap=True,
            maxBounds=QtCore.QRectF(0, 0, width, height),
        )
        self.roi_2.addScaleHandle([1, 1], [0.5, 0.5])
        self.roi_2.addScaleHandle([0, 0], [0.5, 0.5])
        self.roi_2.setPen("b")
        self.roi_3 = pg.ROI(
            [0, 0],
            [width, height],
            scaleSnap=True,
            maxBounds=QtCore.QRectF(0, 0, width, height),
        )
        self.roi_3.addScaleHandle([1, 1], [0.5, 0.5])
        self.roi_3.addScaleHandle([0, 0], [0.5, 0.5])
        self.roi_3.setPen("r")
        self.roi_4 = pg.ROI(
            [0, 0],
            [width, height],
            scaleSnap=True,
            maxBounds=QtCore.QRectF(0, 0, width, height),
        )
        self.roi_4.addScaleHandle([1, 1], [0.5, 0.5])
        self.roi_4.addScaleHandle([0, 0], [0.5, 0.5])
        self.roi_4.setPen("g")
        # adding in reverse order so that handle_1 corresponds to the
        # outermost handle
        self.imv.addItem(self.roi_4)
        self.imv.addItem(self.roi_3)
        self.imv.addItem(self.roi_2)
        self.imv.addItem(self.roi_1)
        self.roi_1.sigRegionChangeFinished.connect(self.pub_roi_1)
        self.roi_2.sigRegionChangeFinished.connect(self.pub_roi_2)
        self.roi_3.sigRegionChangeFinished.connect(self.pub_roi_3)
        self.roi_4.sigRegionChangeFinished.connect(self.pub_roi_4)

    def build(self) -> Any:
        """
        Creates, stores, and returns a new ImageView
        """
        logging.info("calling build")

        self.imv = CustomPlotItem()
        self.imageItem = pg.ImageItem()
        self.imv.addItem(self.imageItem)
        self.colorLegendItem = ColorLegendItem(
            imageItem=self.imageItem, label="Pixel Intensity, Avg."
        )
        self.colorLegendItem.setLevels((0, 256))
        self.graphicsLayoutWidget = pg.GraphicsLayoutWidget()
        self.graphicsLayoutWidget.addItem(self.imv, 0, 0)
        self.graphicsLayoutWidget.addItem(self.colorLegendItem, 0, 1)
        self.imageItem.setImage(128 * np.ones(shape=(self.width, self.height)))
        self.handle_setup(self.width, self.height)
        self.state.plot = self.imv

        cm_cmap = pg.ColorMap(
            [0, 0.25, 0.75, 1],
            [
                [0, 0, 0, 255],
                [255, 0, 0, 255],
                [255, 255, 0, 255],
                [255, 255, 255, 255],
            ],
        )
        color_lut_1 = cm_cmap.getLookupTable()
        color_lut = color_lut_1.astype(np.uint8)
        self.imageItem.setLookupTable(color_lut)
        self.colorLegendItem.setLut(color_lut)
        self.style()

        self.label_region_1_tl = pg.LabelItem(
            "Region 1 Coordinates: bot left: (0, 0) top right: (%s, %s)"
            % (self.width, self.height)
        )
        self.graphicsLayoutWidget.addItem(self.label_region_1_tl, 1, 0)

        self.label_region_2_tl = pg.LabelItem(
            "Region 2 Coordinates: bot left: (0, 0) top right: (%s, %s)"
            % (self.width, self.height)
        )
        self.graphicsLayoutWidget.addItem(self.label_region_2_tl, 2, 0)

        self.label_region_3_tl = pg.LabelItem(
            "Region 3 Coordinates: bot left: (0, 0) top right: (%s, %s)"
            % (self.width, self.height)
        )
        self.graphicsLayoutWidget.addItem(self.label_region_3_tl, 3, 0)

        self.label_region_4_tl = pg.LabelItem(
            "Region 4 Coordinates: bot left: (0, 0) top right: (%s, %s)"
            % (self.width, self.height)
        )
        self.graphicsLayoutWidget.addItem(self.label_region_4_tl, 4, 0)

        proxy = QtGui.QGraphicsProxyWidget()
        self.ra_spinbox = QtGui.QSpinBox()
        self.ra_spinbox.setPrefix("Rolling Average Window Length (in Frames): \t")
        # default and max values must be matched in the state defaults
        # there is no other mechanism for enforcing this
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
        sbLayout = self.graphicsLayoutWidget.addLayout(5, 0)
        sbLayout.addItem(proxy, 1, 1)

        if not self.config.external_timer:
            self.state.timer = QtCore.QTimer()
            self.state.timer.setInterval(TIMER_INTERVAL)
            self.state.timer.start()
            self.state.timer.timeout.connect(self.update)

        return self.graphicsLayoutWidget

    def ra_value_changed(self) -> None:
        self.state.ra_win_len = self.ra_spinbox.value()

    def stop(self) -> None:
        self.state.timer.stop()
