#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import random
from typing import List

from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import DefinedTrialOrderProtocolSpec
from bci.stimulus.psychopy.constants import FingerTappingCondition
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
    RED,
    SPACEBAR_KEY,
    WHITE,
    WINDOW_SIZE,
)
from bci.stimulus.psychopy.protocols.finger_tapping_feedback_params import (
    VALID_FINGERS_BY_CONDITION,
)


NUM_BLOCKS = 6
NUM_TRIALS = 5


def generate_experiment_trial_sequence(n_block: int) -> List[List[TrialSpec]]:
    first = [
        HAND_RIGHT_SPEC_SET,
        HAND_LEFT_SPEC_SET,
        NULL_SPEC_SET,
    ]
    last = [
        HAND_RIGHT_SPEC_SET,
        HAND_LEFT_SPEC_SET,
        NULL_SPEC_SET,
    ]
    random.shuffle(first)
    random.shuffle(last)

    # Shuffle until we don't get two in a row
    while first[-1] == last[0]:
        random.shuffle(last)

    return first + last


HAND_RIGHT_SPEC_SET = [
    TrialSpec(
        condition=FingerTappingCondition.HAND_RIGHT,
        pre_stimulus_duration_range=(20.0, 20.0),
        post_stimulus_duration_range=(20.0, 20.0),
        performance_duration_range=(20.0, 20.0),
    )
] * NUM_TRIALS

HAND_LEFT_SPEC_SET = [
    TrialSpec(
        condition=FingerTappingCondition.HAND_LEFT,
        pre_stimulus_duration_range=(20.0, 20.0),
        post_stimulus_duration_range=(20.0, 20.0),
        performance_duration_range=(20.0, 20.0),
    )
] * NUM_TRIALS

NULL_SPEC_SET = [
    TrialSpec(
        condition=FingerTappingCondition.NULL,
        pre_stimulus_duration_range=(20.0, 20.0),
        post_stimulus_duration_range=(20.0, 20.0),
        performance_duration_range=(20.0, 20.0),
    )
] * NUM_TRIALS

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
    incorrect_behavior_color=RED,
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
    photodiode_enabled=True,
)
STIMULUS_GROUPS = {
    FingerTappingCondition.HAND_LEFT: 0,
    FingerTappingCondition.HAND_RIGHT: 1,
    FingerTappingCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=20.0,
    final_rest_duration=20.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
TORRECELLI_FINGER_TAPPING_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence,
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)
