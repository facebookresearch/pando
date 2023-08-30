#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import sys
import labgraph as df
import numpy as np
import os
import logging

from bci.nodes.profiler_node import ProfileNode

if os.environ["CAM_MODE"] == "basler":
    from bci.systems.sawyer.messages import BaslerImageMessage as CameraImageMessage
else:
    from bci.systems.sawyer.messages import HamamatsuImageMessage as CameraImageMessage


class FrameInfoMsg(df.Message):
    pixel_intensity_mean: float
    frame_number: int


class FrameInfoConverter(ProfileNode):
    INPUT = df.Topic(CameraImageMessage)
    OUTPUT = df.Topic(FrameInfoMsg)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def extract_frame_info(self, message: CameraImageMessage) -> FrameInfoMsg:
        self.increment_message_processed()
        mean = np.mean(message.image_data)
        frame_number = message.sequence_number
        FIM = FrameInfoMsg(pixel_intensity_mean=mean, frame_number=frame_number)
        yield self.OUTPUT, FIM
