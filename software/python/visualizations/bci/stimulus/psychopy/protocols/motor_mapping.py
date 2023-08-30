#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.constants.laterality import ActivationSide
from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.constants import MotorMappingCondition
from bci.stimulus.psychopy.images import load_image
from bci.stimulus.psychopy.protocols.constants import (
    BLOCK_DONE_INSTRUCTIONS,
    BLOCK_INFORMATION,
    DEFAULT_BLOCK_NUMBER_DURATION,
    EXPERIMENT_DONE_INSTRUCTIONS,
    EXPERIMENT_INFO,
    FULLSCREEN_SIZE,
    GRAY,
    GREEN,
    IMAGE_SIZE,
    MONITOR_DISTANCE,
    MONITOR_WIDTH,
    PROTOCOL_INSTRUCTIONS,
    SPACEBAR_KEY,
    WHITE,
    WINDOW_SIZE,
)


# 2 of each condition + 3 of Null
TRIAL_SPEC_SET = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=(0, 0),
        post_stimulus_duration_range=(0, 0),
        performance_duration_range=(15, 15),  # No jitter
    )
    for condition in list(MotorMappingCondition)
    if condition != MotorMappingCondition.UNDEFINED
] * 2
TRIAL_SPEC_SET.append(
    TrialSpec(
        condition=MotorMappingCondition.NULL,
        pre_stimulus_duration_range=(0, 0),
        post_stimulus_duration_range=(0, 0),
        performance_duration_range=(15, 15),  # No jitter
    )
)
DISPLAY_CONFIG = DisplayConfig(
    background_color=GRAY,
    block_done_instructions=BLOCK_DONE_INSTRUCTIONS,
    block_information=BLOCK_INFORMATION,
    continue_key=SPACEBAR_KEY,
    experiment_done_instructions=EXPERIMENT_DONE_INSTRUCTIONS,
    experiment_info=EXPERIMENT_INFO,
    fixation_cross_color=GREEN,
    foreground_color=WHITE,
    fullscreen=True,
    fullscreen_size=FULLSCREEN_SIZE,
    image_size=IMAGE_SIZE,
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    key_mapping_instructions=None,
    protocol_instructions=PROTOCOL_INSTRUCTIONS,
    stimulus_conditions=MotorMappingCondition,
    stimulus_images={
        MotorMappingCondition.HAND_LEFT: ImageProperties(load_image("hand_left")),
        MotorMappingCondition.HAND_RIGHT: ImageProperties(load_image("hand_right")),
        MotorMappingCondition.FOOT_LEFT: ImageProperties(load_image("foot_left")),
        MotorMappingCondition.FOOT_RIGHT: ImageProperties(load_image("foot_right")),
        MotorMappingCondition.TONGUE: ImageProperties(load_image("tongue")),
        MotorMappingCondition.NULL: ImageProperties(load_image("null")),
    },
    window_size=WINDOW_SIZE,
)
STIMULUS_ACTIVATIONS = {
    MotorMappingCondition.HAND_LEFT: ActivationSide.RIGHT,
    MotorMappingCondition.HAND_RIGHT: ActivationSide.LEFT,
}
STIMULUS_GROUPS = {
    MotorMappingCondition.HAND_LEFT: 0,
    MotorMappingCondition.HAND_RIGHT: 1,
    MotorMappingCondition.FOOT_LEFT: 2,
    MotorMappingCondition.FOOT_RIGHT: 3,
    MotorMappingCondition.TONGUE: 4,
    MotorMappingCondition.NULL: 5,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=0.0,
    final_rest_duration=0.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
