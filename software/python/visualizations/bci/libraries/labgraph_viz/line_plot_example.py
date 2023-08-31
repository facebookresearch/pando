#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import labgraph as df
import numpy as np
import pyqtgraph as pg
import time
from random import random
from pyqtgraph.Qt import QtGui
from typing import Tuple

from .plots import LinePlot, LinePlotConfig, Mode


SAMPLE_RATE = 10.0
NUM_FEATURES = 100


class AppendMessage(df.Message):
    timestamp: float
    data: float


class UpdateMessage(df.Message):
    domain: df.NumpyType((NUM_FEATURES,), np.int)
    range: df.NumpyType((NUM_FEATURES,), np.float64)


class GeneratorConfig(df.Config):
    sample_rate: float
    num_features: int


class Generator(df.Node):
    """
    Generate messages to visualize
    """

    OUTPUT = df.Topic(AppendMessage)
    OUTPUT_2 = df.Topic(UpdateMessage)
    config: GeneratorConfig

    @df.publisher(OUTPUT)
    async def generate_noise(self) -> df.AsyncPublisher:
        while True:
            yield self.OUTPUT, AppendMessage(timestamp=time.time(), data=random() * 100)
            await asyncio.sleep(1 / self.config.sample_rate)

    @df.publisher(OUTPUT_2)
    async def generate_update_noise(self) -> df.AsyncPublisher:
        while True:
            yield self.OUTPUT_2, UpdateMessage(
                domain=np.arange(self.config.num_features),
                range=np.random.rand(self.config.num_features),
            )
            await asyncio.sleep(0.5)


class Window(df.Node):
    """
    This is an example of a cusom Window Node.
    It creates a new window, sets some properties of the Window,
    adds some plots and starts the QT application.
    """

    PLOT: LinePlot
    PLOT_2: LinePlot

    @df.main
    def run_plot(self) -> None:
        win = pg.GraphicsWindow()
        win.addItem(self.PLOT.build())
        win.nextRow()
        win.addItem(self.PLOT_2.build())
        QtGui.QApplication.instance().exec_()

    def cleanup(self) -> None:
        QtGui.QApplication.instance().quit()


class SimpleVizGroup(df.Group):
    """
    This is a simple example of how we can display the BarPlot
    in a custom Window Node.

    Note that we map the message fields to x_field and y_field
    in self.PLOT.configure to tell the BarPlot which data is associated
    with which axis.
    """

    PLOT: LinePlot
    PLOT_2: LinePlot
    WINDOW: Window

    def __init__(self) -> None:
        super().__init__()
        self.PLOT.configure(
            LinePlotConfig(
                x_field="timestamp", y_field="data", mode=Mode.APPEND, window_size=20
            )
        )
        self.PLOT_2.configure(
            LinePlotConfig(x_field="domain", y_field="range", mode=Mode.UPDATE)
        )
        self.WINDOW.PLOT = self.PLOT
        self.WINDOW.PLOT_2 = self.PLOT_2

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (self.PLOT, self.PLOT_2, self.WINDOW)


class Demo(df.Graph):
    """
    A simple graph showing how we can add our group
    """

    GENERATOR: Generator
    VIZ: SimpleVizGroup

    def setup(self) -> None:
        self.GENERATOR.configure(
            GeneratorConfig(sample_rate=SAMPLE_RATE, num_features=NUM_FEATURES)
        )

    def connections(self) -> df.Connections:
        return (
            (self.GENERATOR.OUTPUT, self.VIZ.PLOT.INPUT),
            (self.GENERATOR.OUTPUT_2, self.VIZ.PLOT_2.INPUT),
        )

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (self.GENERATOR, self.VIZ)


if __name__ == "__main__":
    graph = Demo()
    runner = df.LocalRunner(module=graph)
    runner.run()
