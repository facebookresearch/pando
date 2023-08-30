#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.constants.laterality import ActivationSide
from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.constants import FingerHapticStimulationCondition
from bci.stimulus.psychopy.images import load_image
from bci.stimulus.psychopy.protocols.constants import (
    BLOCK_DONE_INSTRUCTIONS,
    BLOCK_INFORMATION,
    DEFAULT_BLOCK_NUMBER_DURATION,
    DEFAULT_POST_STIMULUS_DURATION_RANGE,
    EXPERIMENT_DONE_INSTRUCTIONS,
    EXPERIMENT_INFO,
    FULLSCREEN_SIZE,
    GRAY,
    GREEN,
    IMAGE_SIZE,
    MONITOR_DISTANCE,
    MONITOR_WIDTH,
    PROTOCOL_INSTRUCTIONS,
    SPACEBAR_KEY,
    WHITE,
    WINDOW_SIZE,
)


TRIAL_SPEC_SET = [
    TrialSpec(
        condition=condition,
        pre_stimulus_duration_range=(0.5, 1),
        post_stimulus_duration_range=DEFAULT_POST_STIMULUS_DURATION_RANGE,
        performance_duration_range=(2, 8),
    )
    for condition in list(FingerHapticStimulationCondition)
    if condition != FingerHapticStimulationCondition.UNDEFINED
]
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
    image_size=IMAGE_SIZE,
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    key_mapping_instructions=None,
    protocol_instructions=PROTOCOL_INSTRUCTIONS,
    stimulus_conditions=FingerHapticStimulationCondition,
    stimulus_images={
        # TODO: Use digit-specific images
        FingerHapticStimulationCondition.LEFT_THUMB: ImageProperties(
            load_image("hand_left")
        ),
        FingerHapticStimulationCondition.LEFT_INDEX: ImageProperties(
            load_image("hand_left")
        ),
        FingerHapticStimulationCondition.LEFT_MIDDLE: ImageProperties(
            load_image("hand_left")
        ),
        FingerHapticStimulationCondition.LEFT_PINKY: ImageProperties(
            load_image("hand_left")
        ),
        FingerHapticStimulationCondition.RIGHT_THUMB: ImageProperties(
            load_image("hand_right")
        ),
        FingerHapticStimulationCondition.RIGHT_INDEX: ImageProperties(
            load_image("hand_right")
        ),
        FingerHapticStimulationCondition.RIGHT_MIDDLE: ImageProperties(
            load_image("hand_right")
        ),
        FingerHapticStimulationCondition.RIGHT_PINKY: ImageProperties(
            load_image("hand_right")
        ),
        FingerHapticStimulationCondition.NULL: ImageProperties(load_image("null")),
    },
    window_size=WINDOW_SIZE,
)
STIMULUS_ACTIVATIONS = {
    FingerHapticStimulationCondition.LEFT_THUMB: ActivationSide.RIGHT,
    FingerHapticStimulationCondition.LEFT_INDEX: ActivationSide.RIGHT,
    FingerHapticStimulationCondition.LEFT_MIDDLE: ActivationSide.RIGHT,
    FingerHapticStimulationCondition.LEFT_PINKY: ActivationSide.RIGHT,
    FingerHapticStimulationCondition.RIGHT_THUMB: ActivationSide.LEFT,
    FingerHapticStimulationCondition.RIGHT_INDEX: ActivationSide.LEFT,
    FingerHapticStimulationCondition.RIGHT_MIDDLE: ActivationSide.LEFT,
    FingerHapticStimulationCondition.RIGHT_PINKY: ActivationSide.LEFT,
}
STIMULUS_GROUPS = {
    FingerHapticStimulationCondition.LEFT_THUMB: 0,
    FingerHapticStimulationCondition.LEFT_INDEX: 0,
    FingerHapticStimulationCondition.LEFT_MIDDLE: 0,
    FingerHapticStimulationCondition.LEFT_PINKY: 0,
    FingerHapticStimulationCondition.RIGHT_THUMB: 1,
    FingerHapticStimulationCondition.RIGHT_INDEX: 1,
    FingerHapticStimulationCondition.RIGHT_MIDDLE: 1,
    FingerHapticStimulationCondition.RIGHT_PINKY: 1,
    FingerHapticStimulationCondition.NULL: 2,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=1.0,
    final_rest_duration=1.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
