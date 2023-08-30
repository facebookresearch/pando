#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.constants.laterality import ActivationSide
from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.constants import FingerTappingCondition
from bci.stimulus.psychopy.images import load_image
from bci.stimulus.psychopy.protocols.constants import (
    BLOCK_DONE_INSTRUCTIONS,
    BLOCK_INFORMATION,
    DEFAULT_BLOCK_NUMBER_DURATION,
    DEFAULT_PERFORMANCE_DURATION_RANGE,
    DEFAULT_POST_STIMULUS_DURATION_RANGE,
    DEFAULT_PRE_STIMULUS_DURATION_RANGE,
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


TRIAL_SPEC_SET = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=DEFAULT_PRE_STIMULUS_DURATION_RANGE,
        post_stimulus_duration_range=DEFAULT_POST_STIMULUS_DURATION_RANGE,
        performance_duration_range=DEFAULT_PERFORMANCE_DURATION_RANGE,
    )
    for condition in list(FingerTappingCondition)
    if condition != FingerTappingCondition.UNDEFINED
]

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
    stimulus_conditions=FingerTappingCondition,
    stimulus_images={
        FingerTappingCondition.HAND_LEFT: ImageProperties(load_image("hand_left")),
        FingerTappingCondition.HAND_RIGHT: ImageProperties(load_image("hand_right")),
        FingerTappingCondition.NULL: ImageProperties(load_image("null")),
    },
    window_size=WINDOW_SIZE,
)
STIMULUS_ACTIVATIONS = {
    FingerTappingCondition.HAND_LEFT: ActivationSide.RIGHT,
    FingerTappingCondition.HAND_RIGHT: ActivationSide.LEFT,
}
STIMULUS_GROUPS = {
    FingerTappingCondition.HAND_LEFT: 0,
    FingerTappingCondition.HAND_RIGHT: 1,
    FingerTappingCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=9.0,
    final_rest_duration=9.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
