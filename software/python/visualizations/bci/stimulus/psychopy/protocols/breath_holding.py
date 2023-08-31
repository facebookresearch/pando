#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List

from bci.constants.laterality import ActivationSide
from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import DefinedTrialOrderProtocolSpec
from bci.stimulus.psychopy.constants import BreathHoldingCondition
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


NUM_BLOCKS = 4


def generate_experiment_trial_sequence(
    n_blocks: int = NUM_BLOCKS,
) -> List[List[TrialSpec]]:
    block_trial_sequence = []
    for _ in range(n_blocks):
        block_trial_sequence.append(TRIAL_SPEC_SET)
    return block_trial_sequence


TRIAL_SPEC_SET = [
    TrialSpec(
        condition=BreathHoldingCondition.DEEP_BREATHS,
        pre_stimulus_duration_range=(0.0, 0.0),
        post_stimulus_duration_range=(0.0, 0.0),
        performance_duration_range=(60.0, 60.0),
    ),
    TrialSpec(
        condition=BreathHoldingCondition.HOLD_BREATH,
        pre_stimulus_duration_range=(0.0, 0.0),
        post_stimulus_duration_range=(0.0, 0.0),
        performance_duration_range=(120.0, 120.0),
    ),
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
    incorrect_behavior_color=RED,
    image_size=IMAGE_SIZE,
    monitor_distance=MONITOR_DISTANCE,
    monitor_width=MONITOR_WIDTH,
    no_behavior_feedback=False,
    key_mapping_instructions=None,
    protocol_instructions=PROTOCOL_INSTRUCTIONS,
    stimulus_conditions=BreathHoldingCondition,
    stimulus_images={
        BreathHoldingCondition.DEEP_BREATHS: ImageProperties(
            load_image("deep_breaths")
        ),
        BreathHoldingCondition.HOLD_BREATH: ImageProperties(load_image("hold_breath")),
    },
    window_size=WINDOW_SIZE,
    photodiode_enabled=True,
)
STIMULUS_GROUPS = {
    BreathHoldingCondition.DEEP_BREATHS: 0,
    BreathHoldingCondition.HOLD_BREATH: 1,
}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=30.0,
    final_rest_duration=30.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
BREATH_HOLDING_PROTOCOL = DefinedTrialOrderProtocolSpec(
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    func_get_trial_sequence=generate_experiment_trial_sequence,
    correct_condition_events=None,
)
