#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
from typing import List

from bci.constants.laterality import ActivationSide
from bci.stimulus.psychopy.config import (
    DisplayConfig,
    FixedSequenceSpec,
    ImageProperties,
    PermutableSequenceSpec,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import DefinedTrialOrderProtocolSpec
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
Variable duration finger tapping protoocol with
two number of durations (aka "charmeleon")

Condition:
LEFT/RIGHT tapping: 3.0 seconds, 6.0 seconds
NULL: 6.0 seconds.

Interstimulus time: 20 seconds.

Each block has 4 trials of each condition, trial order is
randomized, but consecutive NULL trials are disallowed.

In addition, there is a 3 minute rest block before the
active block portion. Two protocol specs will be available
to chose from.

Block duration (trials only) in total is:
(23 + 23 + 26 + 26 + 26) * 4 = 496 seconds (8min 16seconds)

n_blocks (w/o rest block) | duration | trials per condition
        4                    ~33min      16
        5                    ~42min      20
        6                    ~50min      24
"""
INITIAL_REST_DURATION = 20.0
FINAL_REST_DURATION = 20.0
REST_BLOCK_DURATION = 180.0
REST_BLOCK_PERFORMANCE = REST_BLOCK_DURATION - INITIAL_REST_DURATION
TRIALS_PER_CONDITION = 4


_PERFORMANCE_DURATION_RANGES = {
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: (3.0, 3.0),
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: (6.0, 6.0),
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: (3.0, 3.0),
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: (6.0, 6.0),
}
tapping_trial_sequences = []
for condition, performance_duration_range in _PERFORMANCE_DURATION_RANGES.items():
    tapping_trial_sequences.append(
        FixedSequenceSpec(
            fixed_sequence=[
                TrialSpec(
                    condition=condition,
                    pre_stimulus_duration_range=(0, 0),
                    performance_duration_range=performance_duration_range,
                    post_stimulus_duration_range=(20.0, 20.0),
                )
            ]
        )
    )
tapping_trial_sequences.append(
    FixedSequenceSpec(
        fixed_sequence=[
            TrialSpec(
                condition=VariableDurationFingerTappingCondition.NULL,
                pre_stimulus_duration_range=(0, 0),
                performance_duration_range=(6.0, 6.0),
                post_stimulus_duration_range=(20.0, 20.0),
            )
        ]
    )
)
# Allow consecutive trials for all except for null trial
ALLOW_CONSECUTIVE = [True] * len(tapping_trial_sequences)
ALLOW_CONSECUTIVE[-1] = False

# Time for rest block
trial_rest = TrialSpec(
    condition=VariableDurationFingerTappingCondition.NULL,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(REST_BLOCK_PERFORMANCE, REST_BLOCK_PERFORMANCE),
    post_stimulus_duration_range=(0, 0),
)


# Generate trials for blocks
# Use PermutableSequenceSpec and DefinedTrialOrderProtocolSpec
# instead of RandomizedTrialOrderProtocolSpec
# because we want the first block to be specifically rest
def generate_experiment_trial_sequence(
    n_motion_blocks: int, rest_block: bool
) -> List[List[TrialSpec]]:
    """
    Inputs:
    - n_motion_blocks: Determines how many performance blocks
    - rest_block: If True, add a 3 minutes rest block at beginning of experiment
    """
    performance_blocks = [
        sum(
            [
                PermutableSequenceSpec(
                    sequence_spec_list=tapping_trial_sequences
                ).get_sequence_with_repetition(
                    n_reps_per_sequence=TRIALS_PER_CONDITION,
                    allow_consecutive=ALLOW_CONSECUTIVE,
                ),
            ],
            [],
        )
        for _ in range(n_motion_blocks)
    ]
    if rest_block:
        rest = [trial_rest]
        result = sum([[rest], performance_blocks], [])
    else:
        result = performance_blocks
    _print_trial_sequence(result)
    return result


def _print_trial_sequence(blocks: List[List[TrialSpec]]):
    for (i, block) in enumerate(blocks):
        logging.info(f"Block {i}")
        for (j, trial) in enumerate(block):
            logging.info(f"  trial {j}: {trial.condition.name}")


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
        VariableDurationFingerTappingCondition.HAND_LEFT_MID: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
        ),
        VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
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

PHANTOM_ACTIVATIONS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: ActivationSide.LEFT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: ActivationSide.LEFT,
}
STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
    VariableDurationFingerTappingCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=INITIAL_REST_DURATION,
    final_rest_duration=FINAL_REST_DURATION,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)


# This protocol has a dedicated rest block
def generate_experiment_trial_sequence_with_rest(
    n_motion_blocks: int,
) -> List[List[TrialSpec]]:
    return generate_experiment_trial_sequence(n_motion_blocks, rest_block=True)


RANDOMIZED_REST_TWO_DURATION_FINGER_TAPPING_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence_with_rest,
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)


# This protocol does not have a dedicated rest block
def generate_experiment_trial_sequence_no_rest(
    n_motion_blocks: int,
) -> List[List[TrialSpec]]:
    return generate_experiment_trial_sequence(n_motion_blocks, rest_block=False)


RANDOMIZED_TWO_DURATION_FINGER_TAPPING_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence_no_rest,
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)
