#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import pyqtgraph as pg
from bci.systems.sawyer.messages import IMUMessage
from bci.libraries.labgraph_viz.plots import LinePlot, LinePlotConfig
from pyqtgraph.Qt import QtCore, QtGui


TIMER_INTERVAL = 1000


class PyQtApplication(df.Node):
    # Plots
    GYRO_X_PLOT: LinePlot
    GYRO_Y_PLOT: LinePlot
    GYRO_Z_PLOT: LinePlot
    ACCEL_X_PLOT: LinePlot
    ACCEL_Y_PLOT: LinePlot
    ACCEL_Z_PLOT: LinePlot

    @df.main
    def run_plot(self) -> None:
        self.plots = [
            self.GYRO_X_PLOT,
            self.GYRO_Y_PLOT,
            self.GYRO_Z_PLOT,
            self.ACCEL_X_PLOT,
            self.ACCEL_Y_PLOT,
            self.ACCEL_Z_PLOT,
        ]

        win = pg.GraphicsWindow()
        win.resize(400, 640)
        win.setWindowTitle("IMU")

        for plot in self.plots:
            win.addItem(plot.build())
            win.nextRow()

        # Global timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(TIMER_INTERVAL)
        self.timer.start()
        self.timer.timeout.connect(self.update)

        QtGui.QApplication.instance().exec_()

        self.timer.stop()

    def cleanup(self) -> None:
        QtGui.QApplication.instance().quit()

    def update(self) -> None:
        for plot in self.plots:
            plot.update()


def imu_plot_config(y_field) -> LinePlotConfig:
    return LinePlotConfig(
        x_field="timestamp",
        y_field=y_field,
        window_size=2000,
        style={
            "setLabels": {
                "title": y_field,
                "left": "Count Rate",
                "bottom": "Lab Time (seconds)",
            }
        },
    )


class IMUVizGroup(df.Group):
    # Application
    APPLICATION: PyQtApplication

    # Messages
    IMU = df.Topic(IMUMessage)

    # Plots
    GYRO_X_PLOT: LinePlot
    GYRO_Y_PLOT: LinePlot
    GYRO_Z_PLOT: LinePlot
    ACCEL_X_PLOT: LinePlot
    ACCEL_Y_PLOT: LinePlot
    ACCEL_Z_PLOT: LinePlot

    def setup(self) -> None:
        self.GYRO_X_PLOT.configure(imu_plot_config("gyro_x"))
        self.GYRO_Y_PLOT.configure(imu_plot_config("gyro_y"))
        self.GYRO_Z_PLOT.configure(imu_plot_config("gyro_z"))
        self.ACCEL_X_PLOT.configure(imu_plot_config("accel_x"))
        self.ACCEL_Y_PLOT.configure(imu_plot_config("accel_y"))
        self.ACCEL_Z_PLOT.configure(imu_plot_config("accel_z"))
        self.APPLICATION.GYRO_X_PLOT = self.GYRO_X_PLOT
        self.APPLICATION.GYRO_Y_PLOT = self.GYRO_Y_PLOT
        self.APPLICATION.GYRO_Z_PLOT = self.GYRO_Z_PLOT
        self.APPLICATION.ACCEL_X_PLOT = self.ACCEL_X_PLOT
        self.APPLICATION.ACCEL_Y_PLOT = self.ACCEL_Y_PLOT
        self.APPLICATION.ACCEL_Z_PLOT = self.ACCEL_Z_PLOT

    def connections(self) -> df.Connections:
        return (
            (self.IMU, self.GYRO_X_PLOT.INPUT),
            (self.IMU, self.GYRO_Y_PLOT.INPUT),
            (self.IMU, self.GYRO_Z_PLOT.INPUT),
            (self.IMU, self.ACCEL_X_PLOT.INPUT),
            (self.IMU, self.ACCEL_Y_PLOT.INPUT),
            (self.IMU, self.ACCEL_Z_PLOT.INPUT),
        )
