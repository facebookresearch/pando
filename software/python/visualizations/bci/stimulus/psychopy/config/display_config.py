#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass
from typing import Dict, Optional, Tuple, Type

from bci.stimulus.psychopy.constants import Condition
from PIL import Image


@dataclass
class ImageProperties:
    image: Image
    position: Optional[Tuple[int, int]] = (0, 0)


# TODO: Use labgraph node configuration thingy
@dataclass
class DisplayConfig:
    # TODO: Add training/feedback members
    background_color: Tuple[float, float, float]
    block_done_instructions: str
    block_information: str
    continue_key: str
    experiment_done_instructions: str
    experiment_info: str
    fixation_cross_color: Tuple[float, float, float]
    foreground_color: Tuple[float, float, float]
    fullscreen: bool
    fullscreen_size: Tuple[int, int]
    image_size: Tuple[int, int]
    incorrect_behavior_color: Tuple[float, float, float]
    monitor_distance: float
    monitor_width: float
    no_behavior_feedback: bool
    key_mapping_instructions: Optional[str]
    protocol_instructions: str
    stimulus_conditions: Type[Condition]
    stimulus_images: Dict[Condition, ImageProperties]
    window_size: Tuple[int, int]
    exit_key: str = "escape"
    photodiode_enabled: bool = False
