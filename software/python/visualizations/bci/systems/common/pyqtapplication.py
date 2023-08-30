#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import pyqtgraph as pg
from typing import List
from pyqtgraph.Qt import QtCore, QtGui


class PyQtApplicationConfig(df.Config):
    """
    Config for PyQtApplication
    """

    title: str
    width: int
    height: int
    external_timer: bool
    external_timer_interval: int = 33


class PyQtApplication(df.Node):
    """
    Creates a pyqt application with a window + plots
    """

    config: PyQtApplicationConfig

    plots: List[df.Node]

    @df.main
    def run_plot(self) -> None:
        self.app = QtGui.QApplication([])

        # Window
        self.win = pg.GraphicsWindow()
        self.win.resize(self.config.width, self.config.height)
        self.win.setWindowTitle(self.config.title)

        # Plots
        for plot in self.plots:
            self.win.addItem(plot.build())
            self.win.nextRow()

        if self.config.external_timer:
            self.timer = QtCore.QTimer()
            self.timer.setInterval(self.config.external_timer_interval)
            self.timer.start()
            self.timer.timeout.connect(self.update)

        self.app.exec_()

        if self.config.external_timer:
            self.timer.stop()

    def cleanup(self) -> None:
        self.app.quit()

    def update(self) -> None:
        for plot in self.plots:
            plot.update()
