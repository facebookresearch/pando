#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
from pyqtgraph.Qt import QtGui

from bci.systems.sawyer.messages import HRMMessage, PulseOxMessage, RespBeltMessage
from bci.libraries.labgraph_viz.plots import LinePlot, LinePlotConfig


TIMER_INTERVAL = 100


def hr_plot_config(title) -> LinePlotConfig:
    return LinePlotConfig(
        x_field="pando_timestamp",
        y_field="sample",
        window_size=100,
        style={
            "setLabels": {
                "title": title,
                "left": "Value",
                "bottom": "Lab Time (seconds)",
            }
        },
    )


class PyQtApplication(df.Node):
    # Plots
    HRM_PLOT: LinePlot
    PULSE_OX_PLOT: LinePlot
    RESP_PLOT: LinePlot

    @df.main
    def run_plot(self) -> None:
        self.hr_plots = [self.HRM_PLOT, self.PULSE_OX_PLOT, self.RESP_PLOT]

        self.app = QtGui.QApplication([])

        # Window
        self.hr_win = pg.GraphicsWindow()
        self.hr_win.resize(400, 640)
        self.hr_win.setWindowTitle("Heart")

        for plot in self.hr_plots:
            self.hr_win.addItem(plot.build())
            self.hr_win.nextRow()

        # Global timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(TIMER_INTERVAL)
        self.timer.start()
        self.timer.timeout.connect(self.update)

        self.app.exec_()

        self.timer.stop()

    def cleanup(self) -> None:
        self.app.quit()

    def update(self) -> None:
        for plot in self.hr_plots:
            plot.update()


class HeartRateVizGroup(df.Group):
    HRM = df.Topic(HRMMessage)
    PULSE_OX = df.Topic(PulseOxMessage)
    RESP_BELT = df.Topic(RespBeltMessage)

    # Application
    APPLICATION: PyQtApplication

    # Plots
    HRM_PLOT: LinePlot
    PULSE_OX_PLOT: LinePlot
    RESP_PLOT: LinePlot

    def setup(self) -> None:
        self.HRM_PLOT.configure(hr_plot_config("HRM"))
        self.PULSE_OX_PLOT.configure(hr_plot_config("Pulse Ox"))
        self.RESP_PLOT.configure(hr_plot_config("Resp Belt"))
        self.APPLICATION.HRM_PLOT = self.HRM_PLOT
        self.APPLICATION.PULSE_OX_PLOT = self.PULSE_OX_PLOT
        self.APPLICATION.RESP_PLOT = self.RESP_PLOT

    def connections(self) -> df.Connections:
        return (
            (self.HRM, self.HRM_PLOT.INPUT),
            (self.PULSE_OX, self.PULSE_OX_PLOT.INPUT),
            (self.RESP_BELT, self.RESP_PLOT.INPUT),
        )
