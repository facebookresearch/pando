#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Any, Dict, Optional, Tuple
from dataclasses import field

import logging
import labgraph as df
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
from pgcolorbar.colorlegend import ColorLegendItem

from .common import TIMER_INTERVAL


class BasHamHeatMapState(df.State):
    """
    State for a HeatMap
    plot: The pyqt plot that we are updating
    data: The data for the heatmap
    """

    plot: Optional[Any] = None
    data: np.ndarray = None
    timer: Optional[QtCore.QTimer] = None


class BasHamHeatMapConfig(df.Config):
    """
    Configuration for a HeatMap
    data: The field on incoming messages for data
    channel_map: The field on the incoming messages for channel_map
    style: Pass any plot functions and params to run in style()
    hamamatsu: bool determining camera mode
    color_map: The name of the color map to use
    external_timer: Specify whether you are providing an external timer to call update() manually
    """

    data: str = None
    width: str = None
    height: str = None
    channel_map: str = None
    style: Dict[str, Any] = field(default_factory=dict)
    color_map: str = "jet"
    hamamatsu: bool = True
    external_timer: bool = False


class BasHamHeatMap(df.Node):
    """
    PyQTGraph heat map plot
    Subscribe to a topic and visualize the data in a heat map
    """

    INPUT = df.Topic(df.Message)

    state: BasHamHeatMapState
    width: int = 2304
    height: int = 2304
    cb_set: bool = False
    colorLegendItem: ColorLegendItem
    imageItem: pg.ImageItem
    graphicsLayout: pg.GraphicsLayout

    def setup(self) -> None:
        if not self.config.hamamatsu:
            logging.info("Running heatmap in Basler Mode")
            self.width = 1920
            self.height = 1200

    def update(self) -> None:
        if self.state.plot is not None and self.state.data is not None:
            self.imageItem.setImage(self.state.data, axisOrder="row-major")

    @df.subscriber(INPUT)
    def got_message(self, message: df.Message) -> None:
        """
        Receive data from input topic
        and display on plot according to HeatMapConfig
        """
        if self.state.plot is not None:
            self.state.data = getattr(message, self.config.data)

    def style(self) -> None:
        """
        Call any style functions for the HeatMap
        """
        if self.config.style:
            for k, v in self.config.style.items():
                if isinstance(v, str):
                    v = (v,)
                if isinstance(v, dict):
                    getattr(self.state.plot, k)(**v)
                else:
                    getattr(self.state.plot, k)(*v)

    def build(self) -> Any:
        """
        Creates, stores, and returns a new ImageItem
        """
        import pyqtgraph as pg

        self.state.plot = pg.PlotItem()
        self.imageItem = pg.ImageItem()
        self.state.plot.addItem(self.imageItem)
        self.colorLegendItem = ColorLegendItem(
            imageItem=self.imageItem, label="Pixel Intensity"
        )
        self.colorLegendItem.setLevels((0, 256))

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

        self.graphicsLayout = pg.GraphicsLayout()
        self.graphicsLayout.addItem(self.state.plot, 0, 0)
        self.graphicsLayout.addItem(self.colorLegendItem, 0, 1)
        self.imageItem.setImage(np.zeros(shape=(self.width, self.height)))
        self.style()

        if not self.config.external_timer:
            self.state.timer = QtCore.QTimer()
            self.state.timer.setInterval(TIMER_INTERVAL)
            self.state.timer.start()
            self.state.timer.timeout.connect(self.update)

        return self.graphicsLayout

    def stop(self) -> None:
        self.state.timer.stop()
