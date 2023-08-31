#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import dataclasses

from bci.constants.laterality import ActivationSide
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
    DEFAULT_POST_STIMULUS_DURATION_RANGE,
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


# 3x HAND_LEFT performance_duration: [(1.5, 1.5), (3.0, 3.0), (6.0, 6.0)]
# 3x HAND_RIGHT performance_duration: [(1.5, 1.5), (3.0, 3.0), (6.0, 6.0)]
# 1x NULL performance_duration: (6.0, 6.0)
_PERFORMANCE_DURATION_RANGES = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: (1.5, 1.5),
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: (3.0, 3.0),
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: (6.0, 6.0),
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: (1.5, 1.5),
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: (3.0, 3.0),
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: (6.0, 6.0),
}
TRIAL_SPEC_SET = []
for condition, performance_duration_range in _PERFORMANCE_DURATION_RANGES.items():
    TRIAL_SPEC_SET.append(
        TrialSpec(
            condition=condition,
            pre_stimulus_duration_range=(12, 13),
            post_stimulus_duration_range=DEFAULT_POST_STIMULUS_DURATION_RANGE,
            performance_duration_range=performance_duration_range,
        )
    )
TRIAL_SPEC_SET.append(
    TrialSpec(
        condition=VariableDurationFingerTappingCondition.NULL,
        pre_stimulus_duration_range=(12, 13),
        post_stimulus_duration_range=DEFAULT_POST_STIMULUS_DURATION_RANGE,
        performance_duration_range=(6.0, 6.0),
    )
)


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
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    key_mapping_instructions=None,
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

STIMULUS_ACTIVATIONS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: ActivationSide.LEFT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: ActivationSide.LEFT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: ActivationSide.LEFT,
}
STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
    VariableDurationFingerTappingCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=9.0,
    final_rest_duration=9.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)

RANDOMIZED_VARIABLE_DURATION_FINGER_TAPPING_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=TRIAL_SPEC_SET,
    trial_spec_sets_per_block=3,
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[0, 1, 2],
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)

RANDOMIZED_VARIABLE_DURATION_FINGER_TAPPING_PHOTODIODE_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=TRIAL_SPEC_SET,
    trial_spec_sets_per_block=3,
    display_config=DISPLAY_CONFIG_PHOTODIODE,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[0, 1, 2],
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)
