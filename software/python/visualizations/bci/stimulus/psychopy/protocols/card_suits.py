#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.constants import SuitCondition
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
    for condition in list(SuitCondition)
    if condition != SuitCondition.UNDEFINED
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
    stimulus_conditions=SuitCondition,
    stimulus_images={
        SuitCondition.HEART: ImageProperties(load_image("heart")),
        SuitCondition.SPADE: ImageProperties(load_image("spade")),
        SuitCondition.NULL: ImageProperties(load_image("null")),
    },
    window_size=WINDOW_SIZE,
)
STIMULUS_GROUPS = {
    SuitCondition.HEART: 0,
    SuitCondition.SPADE: 1,
    SuitCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=45.0,
    final_rest_duration=45.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
