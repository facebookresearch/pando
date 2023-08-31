#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import dataclasses

from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import RandomTrialOrderProtocolSpec
from bci.stimulus.psychopy.constants import VariableDurationFingerTappingCondition
from bci.stimulus.psychopy.images import load_image
from bci.stimulus.psychopy.protocols.constants import (
    BLOCK_DONE_INSTRUCTIONS_AT_HOME,
    BLOCK_INFORMATION,
    DEFAULT_BLOCK_NUMBER_DURATION,
    EXPERIMENT_DONE_INSTRUCTIONS_AT_HOME,
    EXPERIMENT_INFO_AT_HOME,
    FULLSCREEN_SIZE,
    GRAY,
    GREEN,
    IMAGE_SIZE,
    MONITOR_DISTANCE,
    MONITOR_WIDTH,
    PROTOCOL_INSTRUCTIONS_AT_HOME,
    RED,
    SPACEBAR_KEY,
    WHITE,
    WINDOW_SIZE,
)
from bci.stimulus.psychopy.protocols.finger_tapping_feedback_params import (
    VALID_FINGERS_BY_CONDITION,
)


"""
Protocol also named "Pikachu", description is
https://docs.google.com/document/d/1TeYZILfhH1rfgLhT8EYS6EWfA5gVxNZkfF5gXGFOiII/edit

Three types of trials: Left-hand, Right-hand, and NULL.
Performance duration for all trials is 6 seconds
ISI is between 19 to 21 seconds.

Each block has 4 trials of each condition, trial order is
randomized. Disallow consecutive NULL trials (this is done by
specifying `stimulus_groups` and `disallow_consecutive`
arguments of RandomTrialOrderProtocolSpec, which will be
passed to RandomizedEventGraphGenerator).

Block duration is about 5min12 seconds
"""

PERFORMANCE_DURATION = 6
_CONDITION_SET = [
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
    VariableDurationFingerTappingCondition.NULL,
]
TRIAL_SPEC_SET = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=(0, 0),
        performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
        post_stimulus_duration_range=(19.0, 21.0),
    )
    for condition in _CONDITION_SET
]

POSITION_MAP = {"hand_left": (-75, 0), "hand_right": (75, 0), "null": (0, 0)}

DISPLAY_CONFIG = DisplayConfig(
    background_color=GRAY,
    block_done_instructions=BLOCK_DONE_INSTRUCTIONS_AT_HOME,
    block_information=BLOCK_INFORMATION,
    continue_key=SPACEBAR_KEY,
    experiment_done_instructions=EXPERIMENT_DONE_INSTRUCTIONS_AT_HOME,
    experiment_info=EXPERIMENT_INFO_AT_HOME,
    fixation_cross_color=GREEN,
    foreground_color=WHITE,
    fullscreen=True,
    fullscreen_size=FULLSCREEN_SIZE,
    image_size=IMAGE_SIZE,
    incorrect_behavior_color=RED,
    key_mapping_instructions=None,
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    protocol_instructions=PROTOCOL_INSTRUCTIONS_AT_HOME,
    stimulus_conditions=VariableDurationFingerTappingCondition,
    stimulus_images={
        VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
        ),
        VariableDurationFingerTappingCondition.HAND_LEFT_MID: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
        ),
        VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
        ),
        VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: ImageProperties(
            load_image("hand_right"), POSITION_MAP["hand_right"]
        ),
        VariableDurationFingerTappingCondition.HAND_RIGHT_MID: ImageProperties(
            load_image("hand_right"), POSITION_MAP["hand_right"]
        ),
        VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: ImageProperties(
            load_image("hand_right"), POSITION_MAP["hand_right"]
        ),
        VariableDurationFingerTappingCondition.NULL: ImageProperties(
            load_image("null"), POSITION_MAP["null"]
        ),
    },
    window_size=WINDOW_SIZE,
)

DISPLAY_CONFIG_PHOTODIODE = dataclasses.replace(DISPLAY_CONFIG, photodiode_enabled=True)

STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
    VariableDurationFingerTappingCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=20.0,
    final_rest_duration=20.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)

RANDOMIZED_FIXED_DURATION_FINGER_TAPPING_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=TRIAL_SPEC_SET,
    trial_spec_sets_per_block=4,
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[2],
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)

RANDOMIZED_FIXED_DURATION_FINGER_TAPPING_PHOTODIODE_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=TRIAL_SPEC_SET,
    trial_spec_sets_per_block=4,
    display_config=DISPLAY_CONFIG_PHOTODIODE,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[2],
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)
