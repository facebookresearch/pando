#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Any, Optional

import numpy as np
import labgraph as df
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
from bci.nodes.profiler_node import ProfileNode

from .common import TIMER_INTERVAL


class ContrastReceiverState(df.State):
    k_std_1: float = 0.0
    k_std_2: float = 0.0
    k_std_3: float = 0.0
    k_std_4: float = 0.0


class ContrastReceiverConfig(df.Config):
    k_std_1: str = "contrast_std_1"
    k_std_2: str = "contrast_std_2"
    k_std_3: str = "contrast_std_3"
    k_std_4: str = "contrast_std_4"


class ContrastReceiver(ProfileNode):
    INPUT = df.Topic(df.Message)

    state: ContrastReceiverState
    config: ContrastReceiverConfig

    counter: int = 0
    rolling_means = [0.0, 0.0, 0.0, 0.0]
    rolling_sum_squares = [0.0, 0.0, 0.0, 0.0]

    # implementation of Welford's online algorithm.  Time for computation constant and not prone to numeric instability
    def update(self, k_vals):
        self.counter += 1
        if self.counter == 1:
            self.rolling_means = k_vals
            (
                self.state.k_std_1,
                self.state.k_std_2,
                self.state.k_std_3,
                self.state.k_std_4,
            ) = [0.0, 0.0, 0.0, 0.0]
        else:
            new_means = [0.0, 0.0, 0.0, 0.0]
            new_sum_squares = [0.0, 0.0, 0.0, 0.0]
            stds = [0.0, 0.0, 0.0, 0.0]
            for i in range(4):
                # this is small but shouldn't be too small in terms of numeric stability
                delta = k_vals[i] / (self.counter - 1)
                # paren grouping promotes stability
                new_means[i] = (self.rolling_means[i] + delta) * (
                    (self.counter - 1) / self.counter
                )
                new_sum_squares[i] = self.rolling_sum_squares[i] + (
                    (k_vals[i] - self.rolling_means[i]) * (k_vals[i] - new_means[i])
                )
                stds[i] = np.sqrt(new_sum_squares[i] / self.counter)
            # update rolling_means and rolling_sum_squares now that stds have been extracted
            self.rolling_means = new_means
            self.rolling_sum_squares = new_sum_squares
            (
                self.state.k_std_1,
                self.state.k_std_2,
                self.state.k_std_3,
                self.state.k_std_4,
            ) = stds

    @df.subscriber(INPUT)
    async def update_state(self, message: df.Message) -> None:
        self.increment_message_processed()
        self.update(
            [
                message.contrast_1,
                message.contrast_2,
                message.contrast_3,
                message.contrast_4,
            ]
        )


class FrameInfoReceiverState(df.State):
    pixel_intensity_mean: float = 0.0
    frame_number: int = 0


class FrameInfoReceiverConfig(df.Config):
    pixel_intensity_mean: str = "pixel_intensity_mean"
    frame_number: str = "frame_number"


class FrameInfoReceiver(df.Node):
    INPUT = df.Topic(df.Message)

    state: FrameInfoReceiverState
    config: FrameInfoReceiverConfig

    @df.subscriber(INPUT)
    async def update_state(self, message: df.Message) -> None:
        self.state.pixel_intensity_mean = getattr(
            message, self.config.pixel_intensity_mean
        )
        self.state.frame_number = getattr(message, self.config.frame_number)


class LabelPanelState(df.State):
    """
    State for label panel
    plot: layout widget
    timer: mediates updates
    """

    plot: pg.GraphicsLayout = None
    timer: Optional[QtCore.QTimer] = None


class LabelPanelConfig(df.Config):
    k_std: str
    histo_mean: str
    frame_number: str
    external_timer: bool = False


class LabelPanel(df.Group):
    CR: ContrastReceiver
    FIR: FrameInfoReceiver

    CR_INPUT = df.Topic(df.Message)
    FIR_INPUT = df.Topic(df.Message)

    state: LabelPanelState

    def connections(self) -> df.Connections:
        return (
            (self.CR_INPUT, self.CR.INPUT),
            (self.FIR_INPUT, self.FIR.INPUT),
        )

    def update(self) -> None:
        if self.state.plot is not None and self.state.plot is not None:
            frame_number_str = "Frame Number: %s" % self.FIR.state.frame_number
            histo_mean_str = (
                "Mean Pixel Intensity: %s"
                % str(self.FIR.state.pixel_intensity_mean)[0:10]
            )
            k_std_str_1 = "K STD RoI 1 (White): %s" % str(self.CR.state.k_std_1)[0:10]
            k_std_str_2 = "K STD RoI 2 (Blue): %s" % str(self.CR.state.k_std_2)[0:10]
            k_std_str_3 = "K STD RoI 3 (Red): %s" % str(self.CR.state.k_std_3)[0:10]
            k_std_str_4 = "K STD RoI 4 (Green): %s" % str(self.CR.state.k_std_4)[0:10]
            self.label_1.setText(text=frame_number_str)
            self.label_2.setText(text=histo_mean_str)
            self.label_3.setText(text=k_std_str_1)
            self.label_4.setText(text=k_std_str_2)
            self.label_5.setText(text=k_std_str_3)
            self.label_6.setText(text=k_std_str_4)

    def build(self) -> Any:
        # might want to create associated style() function later
        self.state.plot = pg.GraphicsLayout()

        self.label_1 = pg.LabelItem()
        self.state.plot.addItem(self.label_1, row=1, col=1)
        self.label_2 = pg.LabelItem()
        self.state.plot.addItem(self.label_2, row=1, col=2)
        self.label_3 = pg.LabelItem()
        self.state.plot.addItem(self.label_3, row=2, col=1)
        self.label_4 = pg.LabelItem()
        self.state.plot.addItem(self.label_4, row=2, col=2)
        self.label_5 = pg.LabelItem()
        self.state.plot.addItem(self.label_5, row=3, col=1)
        self.label_6 = pg.LabelItem()
        self.state.plot.addItem(self.label_6, row=3, col=2)

        if not self.config.external_timer:
            self.state.timer = QtCore.QTimer()
            self.state.timer.setInterval(TIMER_INTERVAL)
            self.state.timer.start()
            self.state.timer.timeout.connect(self.update)

        return self.state.plot

    def stop(self) -> None:
        self.state.timer.stop()
