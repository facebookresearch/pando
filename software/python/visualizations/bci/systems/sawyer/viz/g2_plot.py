#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import numpy as np
import PyQt5.sip as sip
import pyqtgraph as pg
from dataclasses import asdict, field
from typing import Any, Dict, Optional

from bci.libraries.labgraph_viz.plots import (
    CurveStyle,
    Mode,
    PlotState,
    TIMER_INTERVAL,
)
from bci.systems.sawyer.messages import Command
from pyqtgraph.Qt import QtCore


class G2PlotConfig(df.Config):
    """
    Configuration for a PlotNode
    g2_mean_x_field: The field on incoming messages for data associated with the g2 mean x axis
    g2_mean_y_field: The field on incoming messages for data associated with the g2 mean y axis
    g2_fit_x_field: The field on incoming messages for data associated with the g2 fit x axis
    g2_fit_y_field: The field on incoming messages for data associated with the g2 fit y axis
    window_size: The size of a scrolling window for the data
        in samples (available in append mode)
    window_time: The period of a scrolling window for the data
        in the timebase of the x axis (available in append mode)
    mode: Append/Update
    style: Pass any plot functions and params to run in style()
    filter_index: Index to filter y_field on; Defaults to no filtering
    curves: Individual style for each curve (if drawing multiple; empty otherwise)
            If provided, the keys are the names expected on the incoming message.
    external_timer: Specify whether you are providing an external timer to call update() manually
    """

    tau_field: str
    g2_mean_field: str
    g2_fit_field: str
    window_size: Optional[int] = None
    window_time: Optional[float] = None
    mode: str = Mode.APPEND
    style: Dict[str, Any] = field(default_factory=dict)
    filter_index: int = -1
    curves: Dict[str, CurveStyle] = field(default_factory=dict)
    external_timer: bool = False


class G2Plot(df.Node):
    """
    PyQTGraph line plot
    Subscribe to a topic and visualize the data in a line plot
    """

    INPUT = df.Topic(df.Message)

    state: PlotState
    config: G2PlotConfig

    timer: Optional[QtCore.QTimer] = None

    @df.subscriber(INPUT)
    def got_g2(self, message: df.Message) -> None:
        """
        Receive data from input topic
        and display on plot according to G2PlotConfig
        """
        if self.state.plot is not None:
            if hasattr(message, "command"):
                if message.command == Command.CLEAR:
                    for name in self.state.data:
                        self.state.data[name] = (np.array([]), np.array([]))

            if self.config.curves and self.config.g2_mean_field in self.config.curves:
                name = self.config.g2_mean_field
                values = getattr(message, name)
                # Run filter function
                x_val = values[self.config.tau_field]
                y_val = values[self.config.g2_mean_field]
                self._update_curve(name, x_val, y_val)
            else:
                name = self.config.g2_mean_field
                x_val = getattr(message, self.config.tau_field)
                y_val = getattr(message, self.config.g2_mean_field)
                self._update_curve(name, x_val, y_val)

            if self.config.curves and self.config.g2_fit_field in self.config.curves:
                name = self.config.g2_fit_field
                values = getattr(message, name)
                # Run filter function
                x_val = values[self.config.tau_field]
                y_val = values[self.config.g2_fit_field]
                self._update_curve(name, x_val, y_val)
            else:
                name = self.config.g2_fit_field
                x_val = getattr(message, self.config.tau_field)
                y_val = getattr(message, self.config.g2_fit_field)
                self._update_curve(name, x_val, y_val)

    def update(self):
        for name, curve in self.state.curves.items():
            data = self.state.data[name]
            try:
                curve.setData(data[0], data[1])
            except RuntimeError:
                if sip.isdeleted(curve):
                    return
                else:
                    raise

    def _update_curve(self, name: str, x_val: Any, y_val: Any) -> None:
        if self.config.filter_index != -1:
            y_val = y_val[self.config.filter_index]
        data = self.state.data[name]
        if self.config.mode == Mode.APPEND:
            data = (np.append(data[0], x_val), np.append(data[1], y_val))
            if self.config.window_size:
                data = (
                    data[0][-(self.config.window_size) :],
                    data[1][-(self.config.window_size) :],
                )
            if self.config.window_time:
                try:
                    first_valid_time = data[0][-1] - self.config.window_time
                    data_x = data[0][data[0] > first_valid_time]
                    data_y = data[1][-len(data_x) :]
                    data = (data_x, data_y)
                except Exception:
                    raise ValueError("Invalid x_field value for window time")

        elif self.config.mode == Mode.UPDATE:
            data = (x_val, y_val)
        else:
            raise ValueError("Invalid Mode {self.config.mode}")
        self.state.data[name] = data

    def _add_curve(self, plot: Any, name: str, style: CurveStyle) -> None:
        style_dict = asdict(style)
        if style_dict["name"] is None:
            style_dict["name"] = name
        self.state.curves[name] = plot.plot(**style_dict)

    def _style(self, plot: Any) -> None:
        if self.config.style:
            for k, v in self.config.style.items():
                if isinstance(v, str):
                    v = (v,)
                if isinstance(v, dict):
                    getattr(plot, k)(**v)
                else:
                    getattr(plot, k)(*v)

    def build(self) -> Any:
        if self.config.window_size and self.config.window_time:
            raise ValueError("window_size and window_time cannot both be enabled")

        self.state.data = {}
        self.state.curves = {}
        plot = pg.PlotItem()
        self._style(plot)
        if self.config.curves:
            for name, style in self.config.curves.items():
                self._add_curve(plot, name, style)
                self.state.data[name] = (np.array([]), np.array([]))
        else:
            self._add_curve(plot, self.config.g2_mean_field, CurveStyle())
            self._add_curve(plot, self.config.g2_fit_field, CurveStyle(pen="g"))
            self.state.data[self.config.g2_mean_field] = (np.array([]), np.array([]))
            self.state.data[self.config.g2_fit_field] = (np.array([]), np.array([]))
        self.state.plot = plot

        if not self.config.external_timer:
            self.timer = QtCore.QTimer()
            self.timer.setInterval(TIMER_INTERVAL)
            self.timer.start()
            self.timer.timeout.connect(self.update)

        return self.state.plot

    def stop(self) -> None:
        if not self.config.external_timer:
            self.timer.stop()
