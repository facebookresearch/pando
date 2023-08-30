#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
import time
import sys
import os
import contrast_calcs

import labgraph as df
import numpy as np
from typing import Any, Tuple, Dict, Optional
from pyqtgraph.Qt import QtCore
from threading import Timer

from . import RectMsg, ContrastMsg
from bci.nodes.profiler_node import ProfileNode

if "CAM_MODE" in os.environ and os.environ["CAM_MODE"] == "basler":
    logging.info("[ContrastGroup] importing BaslerImageMessage as CameraImageMessage")
    from bci.systems.sawyer.messages import BaslerImageMessage as CameraImageMessage
else:
    from bci.systems.sawyer.messages import HamamatsuImageMessage as CameraImageMessage


from .common import TIMER_INTERVAL


class ContrastGroupState(df.State):
    roi_1: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_2: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_3: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    roi_4: Tuple[slice, slice] = (slice(0, 2304, None), slice(0, 2304, None))
    # keep track of which RoI's are active, initialize to all inactive
    r_active_1: bool = False
    r_active_2: bool = False
    r_active_3: bool = False
    r_active_4: bool = False
    ra_win_len: int = 10
    # TODO rewrite both this and c++ code to only support 8 bit depth images
    data: np.ndarray = np.zeros(shape=(2304, 2304), dtype="uint8")
    avg_data: np.ndarray = np.zeros(shape=(2304, 2304), dtype="uint8")
    normalized_data: np.ndarray = np.zeros(shape=(2304, 2304), dtype="float32")
    timer: Optional[QtCore.QTimer] = None


class RALenMsg(df.Message):
    ra_win_len: int


class RAConfig(df.Config):
    ra_win_len: str = "ra_win_len"


class RAReceiver(ProfileNode):
    INPUT = df.Topic(RALenMsg)

    state: ContrastGroupState
    config: RAConfig

    @df.subscriber(INPUT)
    async def update_state(self, message: RALenMsg) -> None:
        self.increment_message_processed()
        self.state.ra_win_len = getattr(message, self.config.ra_win_len)


class RoIStateReceiver(ProfileNode):
    INPUT = df.Topic(RectMsg)

    state: ContrastGroupState

    @df.subscriber(INPUT)
    async def update_roi_state(self, message: RectMsg) -> None:
        self.increment_message_processed()
        self.state.roi_1 = message.roi_1
        self.state.roi_2 = message.roi_2
        self.state.roi_3 = message.roi_3
        self.state.roi_4 = message.roi_4
        # if RoI slices non-default value, activate corresponding region
        self.state.r_active_1 = message.roi_1_active
        self.state.r_active_2 = message.roi_2_active
        self.state.r_active_3 = message.roi_3_active
        self.state.r_active_4 = message.roi_4_active


class Contrast(ProfileNode):
    INPUT = df.Topic(CameraImageMessage)
    OUTPUT = df.Topic(ContrastMsg)

    msg_counter: int = 0
    start_time: float = 0.0
    contrast_1: float = 0.0
    contrast_2: float = 0.0
    contrast_3: float = 0.0
    contrast_4: float = 0.0
    # TODO might need to make this dynamic for different image sizes
    rolling_avg = contrast_calcs.RollingAverage(5308416)
    contrast_calculator = contrast_calcs.ContrastCalculator()
    RA_hamamatsu_mode: bool = True
    state: ContrastGroupState

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def compute_contrast(self, message: CameraImageMessage) -> ContrastMsg:
        self.increment_message_processed()
        if (message.width != 2304 or message.height != 2304) and self.RA_hamamatsu_mode:
            logging.info("[ContrastGroup] redeclaring contrast calcs")
            self.rolling_avg = contrast_calcs.RollingAverage(2304000)
            self.state.normalized_data = np.zeros(shape=(1920, 1200), dtype="float32")
            self.RA_hamamatsu_mode = False
        self.state.data = message.image_data

        self.state.avg_data = self.rolling_avg.Update(
            self.state.data, self.state.ra_win_len
        )

        self.msg_counter += 1

        # normalize the data by the last ra_window_len number of frames
        # This line produces a runtime error because of an invalid value in true_divide
        # not sure what to do about that given that I'm already handling all the pertinent
        # exceptions implicitly by calling np.nan_to_num
        # void return, results automatically placed in self.normalized_data
        contrast_calcs.normalize(
            self.state.avg_data, self.state.data, self.state.normalized_data
        )
        # logging.info("data: %s", self.state.data)
        # logging.info("averaged data: %s", self.state.avg_data)
        # logging.info("normalized data: %s", self.state.normalized_data)
        contrasts = self.contrast_calculator.GetAllContrasts(
            self.state.normalized_data[self.state.roi_1[1], self.state.roi_1[0]],
            self.state.normalized_data[self.state.roi_2[1], self.state.roi_2[0]],
            self.state.normalized_data[self.state.roi_3[1], self.state.roi_3[0]],
            self.state.normalized_data[self.state.roi_4[1], self.state.roi_4[0]],
            self.state.r_active_1,
            self.state.r_active_2,
            self.state.r_active_3,
            self.state.r_active_4,
        )
        # logging.info("Contrasts: %s", contrasts)
        self.contrast_1 = contrasts[0]
        self.contrast_2 = contrasts[1]
        self.contrast_3 = contrasts[2]
        self.contrast_4 = contrasts[3]
        # logging.info("Contrast 1: %s", self.contrast_1)

        ts = message.timestamp
        if self.start_time == 0:
            self.start_time = ts
        ts = ts - self.start_time
        self.msg_counter += 1
        yield self.OUTPUT, ContrastMsg(
            contrast_1=self.contrast_1,
            contrast_2=self.contrast_2,
            contrast_3=self.contrast_3,
            contrast_4=self.contrast_4,
            timestamp=ts,
        )


class ContrastGroupConfig(df.Config):
    external_timer: bool = False


class ContrastGroup(df.Group):
    RAR: RAReceiver
    RSR: RoIStateReceiver
    CONTRAST: Contrast

    RAR_INPUT = df.Topic(RALenMsg)
    RSR_INPUT = df.Topic(RectMsg)
    CON_INPUT = df.Topic(CameraImageMessage)
    OUTPUT = df.Topic(ContrastMsg)

    start_time: float = 0.0
    msg_counter: int = 0
    state: ContrastGroupState
    config: ContrastGroupConfig
    end_timer: bool = False

    def connections(self) -> df.Connections:
        return (
            (self.RAR_INPUT, self.RAR.INPUT),
            (self.CON_INPUT, self.CONTRAST.INPUT),
            (self.RSR_INPUT, self.RSR.INPUT),
            (self.CONTRAST.OUTPUT, self.OUTPUT),
        )

    def update(self) -> None:
        self.CONTRAST.state.r_active_1 = self.RSR.state.r_active_1
        self.CONTRAST.state.r_active_2 = self.RSR.state.r_active_2
        self.CONTRAST.state.r_active_3 = self.RSR.state.r_active_3
        self.CONTRAST.state.r_active_4 = self.RSR.state.r_active_4
        self.CONTRAST.state.roi_1 = self.RSR.state.roi_1
        self.CONTRAST.state.roi_2 = self.RSR.state.roi_2
        self.CONTRAST.state.roi_3 = self.RSR.state.roi_3
        self.CONTRAST.state.roi_4 = self.RSR.state.roi_4
        self.CONTRAST.state.ra_win_len = self.RAR.state.ra_win_len
        if not self.end_timer:
            Timer(0.25, self.update).start()

    def setup(self) -> None:
        # might want to create associated style() function later
        self.update()

    def cleanup(self) -> None:
        self.end_timer = True
