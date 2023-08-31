#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging

from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import RandomTrialOrderProtocolSpec
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
Fixed duration single hand, single digit tapping protocol. I'm naming
this protocol "squirtle".

Conditions are:

- Right (or Left) Thumb
- Right (or Left) Pinky
- NULL

Performance period are all 6.0 seconds.

This result in 3 conditions.

Interstimulus time = 20 seconds

Each block has 4 trials of each condition, trial order is randomized, but
consecutive NULL trials are disallowed.

NO REST BLOCK option is built-in, since we currently do not use it for
anything. Because of this, we are using `VariableDurationFingerTapping` group.

The protocol can be configured to constrain the digit tapping on either left
or right hand from the command line argument from running the graph.
By default, digit tapping is for right hand only, the flag `--left-hand`
makes the experiment for left hand.

Block duration (trials only) in total is:

26 * 3 * 4 = 312 seconds (5min 12seconds)

  n_blocks    |   duration    |   trials per condition
    4             ~20min          16
    5             ~26min          20
    6             ~31min          24
"""

INITIAL_REST_DURATION = 20.0
FINAL_REST_DURATION = 20.0
TRIALS_PER_CONDITION = 4
PERFORMANCE_DURATION = 6.0
INTERSTIMULUS_DURATION = 20.0

# Use the FingerHapticStimulationCondition because
# same enum
LEFT_TRIAL_CONDITIONS = [
    FingerHapticStimulationCondition.LEFT_THUMB,
    FingerHapticStimulationCondition.LEFT_PINKY,
    FingerHapticStimulationCondition.NULL,
]
RIGHT_TRIAL_CONDITIONS = [
    FingerHapticStimulationCondition.RIGHT_THUMB,
    FingerHapticStimulationCondition.RIGHT_PINKY,
    FingerHapticStimulationCondition.NULL,
]
left_trial_spec_set = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=(0.0, 0.0),
        performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
        post_stimulus_duration_range=(INTERSTIMULUS_DURATION, INTERSTIMULUS_DURATION,),
    )
    for condition in LEFT_TRIAL_CONDITIONS
]
right_trial_spec_set = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=(0.0, 0.0),
        performance_duration_range=(PERFORMANCE_DURATION, PERFORMANCE_DURATION),
        post_stimulus_duration_range=(INTERSTIMULUS_DURATION, INTERSTIMULUS_DURATION,),
    )
    for condition in RIGHT_TRIAL_CONDITIONS
]


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

RANDOMIZED_LEFT_DIGIT_TAPPING_FIXED_DURATION_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=left_trial_spec_set,
    trial_spec_sets_per_block=TRIALS_PER_CONDITION,
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[2],
    correct_condition_events=VALID_DIGIT_BY_CONDITION,
)
RANDOMIZED_RIGHT_DIGIT_TAPPING_FIXED_DURATION_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=right_trial_spec_set,
    trial_spec_sets_per_block=TRIALS_PER_CONDITION,
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[2],
    correct_condition_events=VALID_DIGIT_BY_CONDITION,
)
