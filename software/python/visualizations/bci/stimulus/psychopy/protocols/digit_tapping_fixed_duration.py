#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
from typing import List

from bci.stimulus.psychopy.config import (
    DisplayConfig,
    FixedSequenceSpec,
    ImageProperties,
    PermutableSequenceSpec,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import DefinedTrialOrderProtocolSpec
from bci.stimulus.psychopy.constants import FingerHapticStimulationCondition
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
    RED,
    SPACEBAR_KEY,
    WHITE,
    WINDOW_SIZE,
)
from bci.stimulus.psychopy.protocols.finger_tapping_feedback_params import (
    VALID_DIGIT_BY_CONDITION,
)

"""
Fixed duration single digit tapping protocol, I'm naming
this protocol "charizard".

Conditions are product of:

{LEFT, RIGHT} x {thumb, index, pinky}: Each for 6.0 seconds.
NULL: 6.0 seconds.

This result in a total of 7 conditions.

Interstimulus time: 20 seconds.

Each block has 2 trials of each condition, trial order is randomized,
but consecutive NULL trials are disallowed.

In addition, there is a 3 minute rest block before the active block
portion, if needed. Two protocol specs will be available to chose
from.

Block duration (trials only) in total is:
26 * 7 * 2 = 364 seconds (6min 4seconds)

n_blocks (w/o rest block) |  duration    | trials per condition
            4                 ~25               8
            5                 ~31min            10
            6                 ~37min            12
            7                 ~43min            14
            8                 ~49min            16
"""
INITIAL_REST_DURATION = 20.0
FINAL_REST_DURATION = 20.0
REST_BLOCK_DURATION = 180.0
REST_BLOCK_PERFORMANCE = REST_BLOCK_DURATION - INITIAL_REST_DURATION
TRIALS_PER_CONDITION = 2
PERFORMANCE_DURATION = 6.0
INTERSTIMULUS_DURATION = 20.0


# Use the FingerHapticStimulationCondition because
# same enum
TRIAL_CONDITIONS = [
    FingerHapticStimulationCondition.LEFT_THUMB,
    FingerHapticStimulationCondition.LEFT_INDEX,
    FingerHapticStimulationCondition.LEFT_PINKY,
    FingerHapticStimulationCondition.RIGHT_THUMB,
    FingerHapticStimulationCondition.RIGHT_INDEX,
    FingerHapticStimulationCondition.RIGHT_PINKY,
    FingerHapticStimulationCondition.NULL,
]
tapping_trial_sequences = [
    FixedSequenceSpec(
        fixed_sequence=[
            TrialSpec(
                condition=condition,
                pre_stimulus_duration_range=(0.0, 0.0),
                performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
                post_stimulus_duration_range=(
                    INTERSTIMULUS_DURATION,
                    INTERSTIMULUS_DURATION,
                ),
            )
        ]
    )
    for condition in TRIAL_CONDITIONS
]
# Allow consecutive trials for all except for null condition
ALLOW_CONSECUTIVE = [True] * len(tapping_trial_sequences)
ALLOW_CONSECUTIVE[-1] = False

trial_rest = TrialSpec(
    condition=FingerHapticStimulationCondition.NULL,
    pre_stimulus_duration_range=(0.0, 0.0),
    performance_duration_range=(REST_BLOCK_PERFORMANCE, REST_BLOCK_PERFORMANCE),
    post_stimulus_duration_range=(0.0, 0.0),
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


PROTOCOL_INSTRUCTIONS_AT_HOME = (
    "Reminders: \n\n"
    "- Try your best to stay still during the blok.\n\n"
    "- When instructed, tap ONLY THE SPECIFIED FINGER at a steady pace.\n\n"
    "- Attend to each finger tap as you perform it.\n\n"
    "- You will have a break at the end of each block.\n\n"
    "Press the spacebar to being."
)


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
    stimulus_conditions=FingerHapticStimulationCondition,
    stimulus_images={
        FingerHapticStimulationCondition.LEFT_THUMB: ImageProperties(
            load_image("thumb_left"), POSITION_MAP["hand_left"]
        ),
        FingerHapticStimulationCondition.LEFT_INDEX: ImageProperties(
            load_image("index_left"), POSITION_MAP["hand_left"]
        ),
        FingerHapticStimulationCondition.LEFT_PINKY: ImageProperties(
            load_image("pinky_left"), POSITION_MAP["hand_left"]
        ),
        FingerHapticStimulationCondition.RIGHT_THUMB: ImageProperties(
            load_image("thumb_right"), POSITION_MAP["hand_right"]
        ),
        FingerHapticStimulationCondition.RIGHT_INDEX: ImageProperties(
            load_image("index_right"), POSITION_MAP["hand_right"]
        ),
        FingerHapticStimulationCondition.RIGHT_PINKY: ImageProperties(
            load_image("pinky_right"), POSITION_MAP["hand_right"]
        ),
        FingerHapticStimulationCondition.NULL: ImageProperties(
            load_image("null"), POSITION_MAP["null"]
        ),
    },
    window_size=WINDOW_SIZE,
)
STIMULUS_GROUPS = {
    FingerHapticStimulationCondition.LEFT_THUMB: 0,
    FingerHapticStimulationCondition.LEFT_INDEX: 0,
    FingerHapticStimulationCondition.LEFT_PINKY: 0,
    FingerHapticStimulationCondition.RIGHT_THUMB: 1,
    FingerHapticStimulationCondition.RIGHT_INDEX: 1,
    FingerHapticStimulationCondition.RIGHT_PINKY: 1,
    FingerHapticStimulationCondition.NULL: 2,
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


RANDOMIZED_REST_DIGIT_TAPPING_FIXED_DURATION_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence_with_rest,
    correct_condition_events=VALID_DIGIT_BY_CONDITION,
)


# This protocol does not have a dedicated rest block
def generate_experiment_trial_sequence_no_rest(
    n_motion_blocks: int,
) -> List[List[TrialSpec]]:
    return generate_experiment_trial_sequence(n_motion_blocks, rest_block=False)


RANDOMIZED_DIGIT_TAPPING_FIXED_DURATION_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence_no_rest,
    correct_condition_events=VALID_DIGIT_BY_CONDITION,
)
