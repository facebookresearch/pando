#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import os
import labgraph as df
import pyqtgraph as pg

from typing import Any, Dict, Optional, Tuple
from .line_plot import LinePlot, LinePlotConfig, PlotState, CurveStyle

from . import Mode


class LinePlotMessage(df.Message):
    contrast: float
    timestamp: float


class LPMsgSplitter(df.Node):
    INPUT = df.Topic(df.Message)
    OUTPUT_1 = df.Topic(LinePlotMessage)
    OUTPUT_2 = df.Topic(LinePlotMessage)
    OUTPUT_3 = df.Topic(LinePlotMessage)
    OUTPUT_4 = df.Topic(LinePlotMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT_1)
    @df.publisher(OUTPUT_2)
    @df.publisher(OUTPUT_3)
    @df.publisher(OUTPUT_4)
    async def message_splitter(self, message: df.Message) -> LinePlotMessage:
        ts = message.timestamp
        k1 = message.contrast_1
        k2 = message.contrast_2
        k3 = message.contrast_3
        k4 = message.contrast_4

        yield self.OUTPUT_1, LinePlotMessage(contrast=k1, timestamp=ts)
        yield self.OUTPUT_2, LinePlotMessage(contrast=k2, timestamp=ts)
        yield self.OUTPUT_3, LinePlotMessage(contrast=k3, timestamp=ts)
        yield self.OUTPUT_4, LinePlotMessage(contrast=k4, timestamp=ts)


class LinePlotPanelState(df.State):
    """
    State for label panel
    """

    plot: pg.GraphicsLayout = None


class LinePlotPanelConfig(df.Config):
    contrast_1: str
    contrast_2: str
    contrast_3: str
    contrast_4: str
    timestamp: str


class LinePlotPanel(df.Group):
    INPUT = df.Topic(df.Message)

    LPSPLITTER: LPMsgSplitter
    PLOT_1: LinePlot
    PLOT_2: LinePlot
    PLOT_3: LinePlot
    PLOT_4: LinePlot

    state: LinePlotPanelState

    def update(self):
        self.PLOT_1.update()
        self.PLOT_2.update()
        self.PLOT_3.update()
        self.PLOT_4.update()

    def setup(self) -> None:
        if os.environ["CAM_MODE"] == "basler":
            window_size = 750
        else:
            window_size = 350

        lp_config_1 = LinePlotConfig(
            x_field="timestamp",
            y_field="contrast",
            mode=Mode.APPEND,
            window_size=window_size,
            curve_style=CurveStyle(pen="w"),
            external_timer=True,
        )
        lp_config_2 = LinePlotConfig(
            x_field="timestamp",
            y_field="contrast",
            mode=Mode.APPEND,
            window_size=window_size,
            curve_style=CurveStyle(pen="b"),
            external_timer=True,
        )
        lp_config_3 = LinePlotConfig(
            x_field="timestamp",
            y_field="contrast",
            mode=Mode.APPEND,
            window_size=window_size,
            curve_style=CurveStyle(pen="r"),
            external_timer=True,
        )
        lp_config_4 = LinePlotConfig(
            x_field="timestamp",
            y_field="contrast",
            mode=Mode.APPEND,
            window_size=window_size,
            curve_style=CurveStyle(pen="g"),
            external_timer=True,
        )
        self.PLOT_1.configure(lp_config_1)
        self.PLOT_2.configure(lp_config_2)
        self.PLOT_3.configure(lp_config_3)
        self.PLOT_4.configure(lp_config_4)

    def connections(self) -> df.Connections:
        return (
            (self.INPUT, self.LPSPLITTER.INPUT),
            (self.LPSPLITTER.OUTPUT_1, self.PLOT_1.INPUT),
            (self.LPSPLITTER.OUTPUT_2, self.PLOT_2.INPUT),
            (self.LPSPLITTER.OUTPUT_3, self.PLOT_3.INPUT),
            (self.LPSPLITTER.OUTPUT_4, self.PLOT_4.INPUT),
        )

    def build(self) -> Any:
        # might want to create associated style() function later
        self.state.plot = pg.GraphicsLayout()

        plot_1 = self.PLOT_1.build()
        plot_1.setLabel("left", "Contrast (K)")
        self.state.plot.addItem(plot_1, row=1, col=1)
        plot_2 = self.PLOT_2.build()
        self.state.plot.addItem(plot_2, row=1, col=2)
        plot_3 = self.PLOT_3.build()
        plot_3.setLabel("left", "Contrast (K)")
        plot_3.setLabel("bottom", "Time (s)")
        self.state.plot.addItem(plot_3, row=2, col=1)
        plot_4 = self.PLOT_4.build()
        plot_4.setLabel("bottom", "Time (s)")
        self.state.plot.addItem(plot_4, row=2, col=2)

        return self.state.plot
