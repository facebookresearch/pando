#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List

from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
    PermutableSequenceSpec,
    FixedSequenceSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import DefinedTrialOrderProtocolSpec
from bci.stimulus.psychopy.constants import HeadMotionFingerTapping
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
    MONITOR_DISTANCE,
    MONITOR_WIDTH,
    RED,
    SPACEBAR_KEY,
    WHITE,
    WINDOW_SIZE,
)

"""
Protocol description:
https://docs.google.com/document/d/17UmkDqIdcGWEZjCKnH4WhN5fn9CbOLPtA4m-BAHnkwU/edit

Initial and end resting state period for 3 minutes
6 conditions:
1. LEFT_HAND_PITCH,
2. LEFT_HAND_ROLL,
3. LEFT_HAND_YAW,
4. RIGHT_HAND_PITCH,
5. RIGHT_HAND_ROLL,
6. RIGHT_HAND_YAW

3x performance_duration per condition per block: [(6.0, 6.0)] --> each trial is 6s
8 blocks
First and last block =rest; BLOCK 2-4 = Fingertapping, BLOCK 5-7 = Fingertapping with head motion

A trial defined by `TrialSpec` has the structure:
    |<--fixation start-->|<--performance start-->|<--fixation end-->|

TRIAL_SPEC_SET defines trial structure for each condition
pre_stimulus_duration is interstimulus interval (ISI)
post_stimulus_duration_range is 0
"""
NUM_BLOCKS = 8
INTER_STIMULUS_TIME = 20
PERFORMANCE_DURATION = 6
REST_DURATION = 180

# create trial spec for finger tapping conditions only

FingerTappingCondition = [
    HeadMotionFingerTapping.LEFT_HAND,
    HeadMotionFingerTapping.RIGHT_HAND,
]
num_trials = 3

TRIAL_FINGERTAPPING_SPEC_SET = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=(0, 0),
        performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
        post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
    )
    for condition in FingerTappingCondition
] * num_trials

# create trial spec for finger tapping conditions with head motion

trial_left_hand_pitch = TrialSpec(
    condition=HeadMotionFingerTapping.LEFT_HAND_PITCH,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_left_hand_roll = TrialSpec(
    condition=HeadMotionFingerTapping.LEFT_HAND_ROLL,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

trial_left_hand_yaw = TrialSpec(
    condition=HeadMotionFingerTapping.LEFT_HAND_YAW,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_right_hand_pitch = TrialSpec(
    condition=HeadMotionFingerTapping.RIGHT_HAND_PITCH,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_right_hand_roll = TrialSpec(
    condition=HeadMotionFingerTapping.RIGHT_HAND_ROLL,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

trial_right_hand_yaw = TrialSpec(
    condition=HeadMotionFingerTapping.RIGHT_HAND_YAW,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
# create trial sequences for pitch, roll, and yaw head motion with fingertapping 3x

trial_sequence = [trial_left_hand_pitch, trial_right_hand_pitch] * 3
trial_sequence_pitch = FixedSequenceSpec(fixed_sequence=trial_sequence)

trial_sequence = [trial_left_hand_roll, trial_right_hand_roll] * 3
trial_sequence_roll = FixedSequenceSpec(fixed_sequence=trial_sequence)

trial_sequence = [trial_left_hand_yaw, trial_right_hand_yaw] * 3
trial_sequence_yaw = FixedSequenceSpec(fixed_sequence=trial_sequence)

trial_rest = TrialSpec(
    condition=HeadMotionFingerTapping.REST,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(REST_DURATION, REST_DURATION),  # 3 mins of rest state
    post_stimulus_duration_range=(0, 0),
)

# Generate trials for blocks


def generate_experiment_trial_sequence(
    n_blocks: int = NUM_BLOCKS,
) -> List[List[TrialSpec]]:

    rest_block = [trial_rest]
    finger_tapping_block = TRIAL_FINGERTAPPING_SPEC_SET
    motion_blocks = [
        sum(
            [
                PermutableSequenceSpec(
                    sequence_spec_list=[
                        trial_sequence_pitch,
                        trial_sequence_roll,
                        trial_sequence_yaw,
                    ]
                ).get_sequence(
                    num_sequences=3, allow_duplicates=False, allow_consecutive=False
                ),
            ],
            [],
        )
        for _ in range((n_blocks // 2) - 1)
    ]
    return sum(
        [
            [rest_block],
            [finger_tapping_block],
            [finger_tapping_block],
            [finger_tapping_block],
            motion_blocks,
            [rest_block],
        ],
        [],
    )


PROTOCOL_INSTRUCTIONS_AT_HOME_FOR_HEAD_MOTION = (
    "Reminders: \n\n"
    "- BLOCK 1 and 8: Rest\n\n"
    "- BLOCK 2-4: Fingertapping only and BLOCK 5-7 fingertapping with headmotion\n\n"
    "- Perform the head motion once in the beginning of finger tapping\n\n"
    "- Attend to tapping while performing the task\n\n"
    "- Default position - center\n\n"
    "- Remain still after the head motion and during tapping\n\n"
    "- Move your head in self paced rate \n\n"
    "- Pitch:  Move your head up and down once (e.g. saying yes non-verbally)\n\n"
    "- Roll: Move your head left and right once (e.g. saying so-so non-verbally)\n\n"
    "- Yaw: Move your head left and right once (e.g. saying no-no non verbally)\n\n"
    "- Try to stay still during the block after head motion.\n\n"
    "- You can have a break at the end of each block.\n\n"
    "Press the spacebar to begin."
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
    image_size=(600, 400),
    incorrect_behavior_color=RED,
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    key_mapping_instructions=None,
    protocol_instructions=PROTOCOL_INSTRUCTIONS_AT_HOME_FOR_HEAD_MOTION,
    stimulus_conditions=HeadMotionFingerTapping,
    stimulus_images={
        HeadMotionFingerTapping.LEFT_HAND_PITCH: ImageProperties(
            load_image("left_hand_pitch")
        ),
        HeadMotionFingerTapping.LEFT_HAND_ROLL: ImageProperties(
            load_image("left_hand_roll")
        ),
        HeadMotionFingerTapping.LEFT_HAND_YAW: ImageProperties(
            load_image("left_hand_yaw")
        ),
        HeadMotionFingerTapping.RIGHT_HAND_PITCH: ImageProperties(
            load_image("right_hand_pitch")
        ),
        HeadMotionFingerTapping.RIGHT_HAND_ROLL: ImageProperties(
            load_image("right_hand_roll")
        ),
        HeadMotionFingerTapping.RIGHT_HAND_YAW: ImageProperties(
            load_image("right_hand_yaw")
        ),
        HeadMotionFingerTapping.LEFT_HAND: ImageProperties(load_image("hand_left")),
        HeadMotionFingerTapping.RIGHT_HAND: ImageProperties(load_image("hand_right")),
        HeadMotionFingerTapping.REST: ImageProperties(load_image("null")),
    },
    window_size=WINDOW_SIZE,
)

STIMULUS_GROUPS = {
    HeadMotionFingerTapping.LEFT_HAND_PITCH: 0,
    HeadMotionFingerTapping.LEFT_HAND_ROLL: 1,
    HeadMotionFingerTapping.LEFT_HAND_YAW: 2,
    HeadMotionFingerTapping.RIGHT_HAND_PITCH: 3,
    HeadMotionFingerTapping.RIGHT_HAND_ROLL: 4,
    HeadMotionFingerTapping.RIGHT_HAND_YAW: 5,
    HeadMotionFingerTapping.LEFT_HAND: 6,
    HeadMotionFingerTapping.RIGHT_HAND: 7,
    HeadMotionFingerTapping.REST: 8,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=20.0,
    final_rest_duration=20.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,  # default time is 2s
)

RANDOMIZED_HEAD_MOTION_FINGERTAPPING_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence,
    correct_condition_events=None,
)
