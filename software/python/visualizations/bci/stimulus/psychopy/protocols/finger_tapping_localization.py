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
Protocol also named "Jigglypuff", see detailed description at:
https://docs.google.com/document/d/16vaZ8QcpY_mA9bYiaHWZueW-h58b7fDcvvA52AuwVn4/edit#heading=h.a2n0xvk1wuzu

2 conditions: Left-hand and Right-hand.
Performance duration for all trials is 6 seconds
ISI is between 19 to 21 seconds.
4 locations.
No NULL condition in the interest of time.
20 seconds resting state bookending each block.

Each block as 12 trials (6 per condition).
Trial order is randomized.
8 blocks total (2 per location)
Experiment duration is on average 44 min 36 sec
Block duration is on average 5 min 12 sec

"""

PERFORMANCE_DURATION = 6
_CONDITION_SET = [
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
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

POSITION_MAP = {"hand_left": (-75, 0), "hand_right": (75, 0)}

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
        VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
        ),
        VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: ImageProperties(
            load_image("hand_right"), POSITION_MAP["hand_right"]
        ),
    },
    window_size=WINDOW_SIZE,
)

PHANTOM_ACTIVATIONS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: ActivationSide.LEFT,
}

DISPLAY_CONFIG_PHOTODIODE = dataclasses.replace(DISPLAY_CONFIG, photodiode_enabled=True)

STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
}

TIMING_PARAMS = TimingParams(
    initial_rest_duration=20.0,
    final_rest_duration=20.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)

RANDOMIZED_FINGER_TAPPING_LOCALIZATION = RandomTrialOrderProtocolSpec(
    trial_spec_set=TRIAL_SPEC_SET,
    trial_spec_sets_per_block=6,
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
    disallow_consecutive=[],
)
