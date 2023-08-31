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
Protocol description (otherwise known as Charmander):
(https://fb.quip.com/oRx7AXMWAEk7, section under
'Quantify what "good baseline hemodynamic response" / true HRF looks like'
)
- Use a _Slow Event-Related_ protocol
(https://fb.workplace.com/groups/bci.xfn/permalink/947840689006262/)
- Each block will consist of multiple trials, ISI is 22 seconds.
Null trial is 7s, and the next stimulus starts at the end of the 7s period.

See illustration for block structure:

    [0_long, 1, 2, 1, 2, 1, 0, 2, 1, 2, 1, 2, 0, 3, 4, 3, 4, 3, 0, 4, 3, 4, 3, 4, 0]
    |
    extra time for settling

    - 0 is null condition of 10 seconds
    - 1 and 2 are left and right hand tapping for 5s.
    - 3 and 4 are left and right hand tapping for 2s.
    - The '121' trial-block structure has periodicity of 28 seconds
    - The '343' trial-block structure has periodicity of 24 seconds
    - Each block has 5 trials of each condition, and lasts 555 seconds (~9.25 minutes).
- At a minimum we want 15 trials of each tapping condition (4 total) and null condition,
  for a total of 75 trials. This would require a minimum of 3 blocks, about 30 minutes.
- The ordering of the 121 and 343 trial-blocks can vary.
- We can add a block if subject is up for it, numbers quoted here are minimal required
  assuming minimal motion and good behavior performance

"""

INTER_STIMULUS_TIME = 22.0
SHORT_TAP_TIME = 2.0
LONG_TAP_TIME = 5.0
NULL_TIME = 7.0
INITIAL_REST_TIME = INTER_STIMULUS_TIME - NULL_TIME

"""
A trial defined by `TrialSpec` has the structure:
    |<--fixation start-->|<--performance start-->|<--fixation end-->|
"""
# 2x HAND_LEFT trial
trial_hand_left_short = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(SHORT_TAP_TIME, SHORT_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_hand_left_long = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(LONG_TAP_TIME, LONG_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

# 2x HAND_RIGHT trials
trial_hand_right_short = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(SHORT_TAP_TIME, SHORT_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_hand_right_long = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(LONG_TAP_TIME, LONG_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

# 1x NULL trial
trial_null = TrialSpec(
    condition=VariableDurationFingerTappingCondition.NULL,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(NULL_TIME, NULL_TIME),
    post_stimulus_duration_range=(
        3.0,
        3.0,
    ),  # this makes finger tap event parsing easier
)


# Define sequences

# start null_long
start_null_sequence = FixedSequenceSpec(fixed_sequence=[trial_null])
# 12121
trial_sequence = [trial_hand_left_short, trial_hand_right_short] * 3
trial_sequence[-1] = trial_null
short_tap_sequence1 = FixedSequenceSpec(fixed_sequence=trial_sequence)
# 21212
trial_sequence = [trial_hand_right_short, trial_hand_left_short] * 3
trial_sequence[-1] = trial_null
short_tap_sequence2 = FixedSequenceSpec(fixed_sequence=trial_sequence)
# 34343
trial_sequence = [trial_hand_left_long, trial_hand_right_long] * 3
trial_sequence[-1] = trial_null
long_tap_sequence1 = FixedSequenceSpec(fixed_sequence=trial_sequence)
# 43434
trial_sequence = [trial_hand_right_long, trial_hand_left_long] * 3
trial_sequence[-1] = trial_null
long_tap_sequence2 = FixedSequenceSpec(fixed_sequence=trial_sequence)


# Generate trials for blocks
def generate_experiment_trial_sequence(n_blocks: int) -> List[List[TrialSpec]]:
    block_trial_sequence = [
        sum(
            [
                start_null_sequence.get_sequence(),
                PermutableSequenceSpec(
                    sequence_spec_list=[
                        short_tap_sequence1,
                        short_tap_sequence2,
                        long_tap_sequence1,
                        long_tap_sequence2,
                    ]
                ).get_sequence(
                    num_sequences=4, allow_duplicates=False, allow_consecutive=False
                ),
            ],
            [],
        )
        for _ in range(n_blocks)
    ]
    return block_trial_sequence


def generate_experiment_trial_sequence_short_block(
    n_blocks: int,
) -> List[List[TrialSpec]]:
    """
    This one breaks the standard block into two -- so ~5min per block
    """
    permutable_seq1 = PermutableSequenceSpec(
        sequence_spec_list=[short_tap_sequence1, long_tap_sequence1]
    )
    permutable_seq2 = PermutableSequenceSpec(
        sequence_spec_list=[short_tap_sequence2, long_tap_sequence2]
    )
    block_trial_sequence = []
    for _ in range(n_blocks):
        block_trial_sequence.append(
            start_null_sequence.get_sequence()
            + permutable_seq1.get_sequence(
                num_sequences=2, allow_duplicates=False, allow_consecutive=False
            )
        )
        block_trial_sequence.append(
            start_null_sequence.get_sequence()
            + permutable_seq2.get_sequence(
                num_sequences=2, allow_duplicates=False, allow_consecutive=False
            )
        )
    logging.info(f"block_trial_sequence length is: {len(block_trial_sequence)}")
    return block_trial_sequence


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
        VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ImageProperties(
            load_image("hand_left"), POSITION_MAP["hand_left"]
        ),
        VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: ImageProperties(
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
STIMULUS_ACTIVATIONS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: ActivationSide.RIGHT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: ActivationSide.LEFT,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: ActivationSide.LEFT,
}
STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
    VariableDurationFingerTappingCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=INITIAL_REST_TIME,
    final_rest_duration=0.1,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)

LONG_ISI_PERIODIC_FINGER_TAPPING_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence,
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)

LONG_ISI_PERIODIC_FINGER_TAPPING_SHORT_BLOCK_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence_short_block,
    correct_condition_events=VALID_FINGERS_BY_CONDITION,
)
