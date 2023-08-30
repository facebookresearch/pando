#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.constants.laterality import ActivationSide
from bci.stimulus.psychopy.config import (
    DisplayConfig,
    ImageProperties,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.config.protocol_spec import RandomTrialOrderProtocolSpec
from bci.stimulus.psychopy.constants import CuffOcclusionCondition
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


TRIAL_SPEC_SET = [
    TrialSpec(
        condition=CuffOcclusionCondition.CUFF,
        pre_stimulus_duration_range=(0.0, 0.0),
        post_stimulus_duration_range=(0.0, 0.0),
        performance_duration_range=(120.0, 120.0),
    )
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
    stimulus_conditions=CuffOcclusionCondition,
    stimulus_images={CuffOcclusionCondition.CUFF: ImageProperties(load_image("cuff"))},
    window_size=WINDOW_SIZE,
    photodiode_enabled=True,
)
STIMULUS_GROUPS = {CuffOcclusionCondition.CUFF: 0}
TIMING_PARAMS = TimingParams(
    initial_rest_duration=60.0,
    final_rest_duration=60.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
CUFF_OCCLUSION_PROTOCOL = RandomTrialOrderProtocolSpec(
    trial_spec_set=TRIAL_SPEC_SET,
    trial_spec_sets_per_block=1,
    display_config=DISPLAY_CONFIG,
    stimulus_groups=STIMULUS_GROUPS,
    timing_params=TIMING_PARAMS,
    disallow_consecutive=[2],
    correct_condition_events=None,
)
