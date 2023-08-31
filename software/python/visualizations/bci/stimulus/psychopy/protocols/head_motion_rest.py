#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
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
from bci.stimulus.psychopy.constants import HeadMotionRest
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

"""
Protocol description:
https://docs.google.com/document/d/1hZRPnRyDmpkjUlqTDSLrZKI-hWZTjm2w9a8W27vDHZA/edit

Initial and end resting state period for 3 minutes
6 conditions: PITCH_UP, PITCH_DOWN, ROLL_LEFT, ROLL_RIGHT, YAW_LEFT, YAW_RIGHT
3x performance_duration per condition per block: [(6.0, 6.0)] --> each trial is 6s
8 blocks
First and last block =rest; BLOCK 2-4 = slow head motion, BLOCK 5-7 = fast head motion

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

# create trial spec for conditions
trial_pitch_up = TrialSpec(
    condition=HeadMotionRest.PITCH_UP,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_pitch_down = TrialSpec(
    condition=HeadMotionRest.PITCH_DOWN,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

trial_roll_left = TrialSpec(
    condition=HeadMotionRest.ROLL_LEFT,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_roll_right = TrialSpec(
    condition=HeadMotionRest.ROLL_RIGHT,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_yaw_left = TrialSpec(
    condition=HeadMotionRest.YAW_LEFT,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

trial_yaw_right = TrialSpec(
    condition=HeadMotionRest.YAW_RIGHT,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
# create trial sequences for pitch, roll, and yaw head motion 3x

trial_sequence = [trial_pitch_up, trial_pitch_down] * 3
trial_sequence_pitch = FixedSequenceSpec(fixed_sequence=trial_sequence)
trial_sequence = [trial_roll_left, trial_roll_right] * 3
trial_sequence_roll = FixedSequenceSpec(fixed_sequence=trial_sequence)
trial_sequence = [trial_yaw_left, trial_yaw_right] * 3
trial_sequence_yaw = FixedSequenceSpec(fixed_sequence=trial_sequence)

trial_rest = TrialSpec(
    condition=HeadMotionRest.REST,
    pre_stimulus_duration_range=(0, 0),
    performance_duration_range=(REST_DURATION, REST_DURATION),  # 3 mins of rest state
    post_stimulus_duration_range=(0, 0),
)

# Generate trials for blocks


def generate_experiment_trial_sequence(
    n_blocks: int = NUM_BLOCKS,
) -> List[List[TrialSpec]]:

    if n_blocks < 3:
        logging.info(
            "Protocol requires at least three blocks (2 rest and one motion), "
            "Setting number of blocks to 3"
        )
    rest_block = [trial_rest]
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
        for _ in range(n_blocks - 2)
    ]
    return sum([[rest_block], motion_blocks, [rest_block]], [])


PROTOCOL_INSTRUCTIONS_AT_HOME_FOR_HEAD_MOTION = (
    "Reminders: \n\n"
    "- BLOCK 1 and 8: Rest\n\n"
    "- BLOCK 2-4: slow head motion and BLOCK 5-7 fast\n\n"
    "- Move your head once after the trial starts.\n\n"
    "- Move to the direction of arrow and return to center \n\n"
    "- Attend to the picture while head motion.\n\n"
    "- Try to stay still during the block after head motion.\n\n"
    "- You will have a break at the end of each block.\n\n"
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
    image_size=IMAGE_SIZE,
    incorrect_behavior_color=RED,
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    key_mapping_instructions=None,
    protocol_instructions=PROTOCOL_INSTRUCTIONS_AT_HOME_FOR_HEAD_MOTION,
    stimulus_conditions=HeadMotionRest,
    stimulus_images={
        HeadMotionRest.PITCH_UP: ImageProperties(load_image("pitch_up")),
        HeadMotionRest.PITCH_DOWN: ImageProperties(load_image("pitch_down")),
        HeadMotionRest.ROLL_LEFT: ImageProperties(load_image("roll_left")),
        HeadMotionRest.ROLL_RIGHT: ImageProperties(load_image("roll_right")),
        HeadMotionRest.YAW_LEFT: ImageProperties(load_image("yaw_left")),
        HeadMotionRest.YAW_RIGHT: ImageProperties(load_image("yaw_right")),
        HeadMotionRest.REST: ImageProperties(load_image("null")),
    },
    window_size=WINDOW_SIZE,
)

STIMULUS_GROUPS = {
    HeadMotionRest.PITCH_UP: 0,
    HeadMotionRest.PITCH_DOWN: 1,
    HeadMotionRest.ROLL_LEFT: 2,
    HeadMotionRest.ROLL_RIGHT: 3,
    HeadMotionRest.YAW_LEFT: 4,
    HeadMotionRest.YAW_RIGHT: 5,
    HeadMotionRest.REST: 6,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=20.0,
    final_rest_duration=20.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,  # default time is 2s
)

RANDOMIZED_HEAD_MOTION_REST_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence,
    correct_condition_events=None,
)
