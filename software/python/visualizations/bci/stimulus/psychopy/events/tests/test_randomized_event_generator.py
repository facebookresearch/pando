#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import pytest
from bci.stimulus.psychopy.config import TimingParams, TrialSpec
from bci.stimulus.psychopy.constants import (
    Instructions,
    TrialMode,
    VariableDurationFingerTappingCondition,
)
from bci.stimulus.psychopy.events import RandomizedEventGraphGenerator
from bci.stimulus.psychopy.messages import (
    BlockMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    ShowDecoderFeedbackMessage,
    TrialModeMessage,
)
from labgraph.events import WaitBeginMessage
from labgraph.graphs.topic import Topic
from labgraph.util.error import LabgraphError


BLOCKS = 15
DEFAULT_CONDITIONS = [
    condition
    for condition in list(VariableDurationFingerTappingCondition)
    if condition != VariableDurationFingerTappingCondition.UNDEFINED
]
DEFAULT_STIMULUS_CONDITIONS = VariableDurationFingerTappingCondition
DEFAULT_STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
    VariableDurationFingerTappingCondition.NULL: 2,
}
DEFAULT_TRIAL_SPEC_SET = [
    TrialSpec(condition, (6, 12), (0.5, 0.5), (6, 6.5))
    for condition in DEFAULT_CONDITIONS
]
TRIAL_SPEC_SETS_PER_BLOCK = 3
CALIBRATION_BLOCKS = 6
DEFAULT_TIMING_PARAMS = TimingParams(45.0, 45.0, 2.0)


def test_create_graph_generator() -> None:
    """
    Tests that a valid RandomizedEventGraphGenerator can be created and
    made to generate events in the correct order.
    """

    PRE_STIMULUS_DURATION = 2.0
    PERFORMANCE_DURATION = 1.0
    TIMING_PARAMS = TimingParams(1.0, 2.0, 2.0)

    generator = RandomizedEventGraphGenerator(
        blocks=1,
        stimulus_conditions=VariableDurationFingerTappingCondition,
        stimulus_groups={VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: 0},
        trial_spec_set=[
            TrialSpec(
                VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
                (PRE_STIMULUS_DURATION, PRE_STIMULUS_DURATION),
                (1, 1),
                (PERFORMANCE_DURATION, PERFORMANCE_DURATION),
            )
        ],
        trial_spec_sets_per_block=1,
        calibration_blocks=1,
        timing_params=TIMING_PARAMS,
        simulate_input=True,
        decoding=False,
        pad_block_duration=False,
        disallow_consecutive=[0, 1, 2],
    )

    generator.set_topics(
        Topic(ExperimentInfoMessage),
        Topic(InstructionsMessage),
        Topic(TrialModeMessage),
        Topic(BlockMessage),
        Topic(WaitBeginMessage),
        Topic(ShowDecoderFeedbackMessage),
    )

    event_heap = generator.generate_events()

    # experiment info message
    accumulated_time, _, start_event = event_heap.pop()
    assert accumulated_time == 0.0
    assert isinstance(start_event.message, ExperimentInfoMessage)
    assert start_event.message.total_blocks == 1
    assert start_event.message.trial_spec_sets_per_block == 1
    assert start_event.message.expected_experiment_duration == 6.0
    assert start_event.delay == 0.0

    # key mapping instructions
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == 0.0
    assert isinstance(next_event.message, InstructionsMessage)
    assert next_event.message.instructions == Instructions.KEY_MAPPING
    assert next_event.delay == 0.0

    # protocol instructions
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == 0.0
    assert isinstance(next_event.message, InstructionsMessage)
    assert next_event.message.instructions == Instructions.PROTOCOL
    assert next_event.delay == 0.0

    # Block start
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == 0.0
    assert isinstance(next_event.message, BlockMessage)
    assert next_event.message.block == 0
    assert next_event.delay == 0.0

    # Trial start

    # Pre-stimulus
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == TIMING_PARAMS.block_number_duration
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.PRE_STIMULUS
    assert next_event.message.block == 0
    assert next_event.message.trial == 0
    assert (
        next_event.message.condition
        == VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    )
    assert next_event.delay == 0.0

    # Performance
    accumulated_time, _, next_event = event_heap.pop()
    # block_number_duration + initial_rest_duration
    assert accumulated_time == 3.0
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.PERFORMANCE
    assert next_event.message.block == 0
    assert next_event.message.trial == 0
    assert (
        next_event.message.condition
        == VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    )
    assert next_event.delay == 0.0

    # Post-stimulus
    accumulated_time, _, next_event = event_heap.pop()
    # <performance accumulated> + performance_duration
    assert accumulated_time == 4.0
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.POST_STIMULUS
    assert next_event.message.block == 0
    assert next_event.message.trial == 0
    assert (
        next_event.message.condition
        == VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    )
    assert next_event.delay == 0.0

    # Block end
    accumulated_time, _, next_event = event_heap.pop()
    # <post-stimulus accumulated> + final_rest_duration
    assert accumulated_time == 6.0
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.BLOCK_DONE
    assert next_event.message.block == 0
    assert next_event.message.trial == -1
    assert (
        next_event.message.condition == VariableDurationFingerTappingCondition.UNDEFINED
    )
    assert next_event.delay == 0.0


def test_init_invalid_trial_spec_set():
    """
    Tests that an RandomizedEventGraphGenerator cannot be created and will
    throw if invalid parameters are given on initialization.
    """

    with pytest.raises(LabgraphError):
        RandomizedEventGraphGenerator(
            blocks=1,
            stimulus_conditions=VariableDurationFingerTappingCondition,
            stimulus_groups={VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: 0},
            # Invalid trial_spec_set - Can't arrange without adjacent duplicates
            trial_spec_set=[
                TrialSpec(
                    condition=VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
                    pre_stimulus_duration_range=(1, 1),
                    post_stimulus_duration_range=(1, 1),
                    performance_duration_range=(1, 1),
                ),
                TrialSpec(
                    condition=VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
                    pre_stimulus_duration_range=(1, 1),
                    post_stimulus_duration_range=(1, 1),
                    performance_duration_range=(1, 1),
                ),
            ],
            trial_spec_sets_per_block=1,
            calibration_blocks=1,
            timing_params=DEFAULT_TIMING_PARAMS,
            simulate_input=True,
            decoding=True,
            pad_block_duration=False,
            disallow_consecutive=[0],
        )


def test_generator_topics_must_be_set():
    """
    Tests that an RandomizedEventGraphGenerator cannot generate events
    until topics are set for it.
    """

    generator = RandomizedEventGraphGenerator(
        blocks=BLOCKS,
        stimulus_conditions=DEFAULT_STIMULUS_CONDITIONS,
        stimulus_groups=DEFAULT_STIMULUS_GROUPS,
        trial_spec_set=DEFAULT_TRIAL_SPEC_SET,
        trial_spec_sets_per_block=TRIAL_SPEC_SETS_PER_BLOCK,
        calibration_blocks=CALIBRATION_BLOCKS,
        timing_params=DEFAULT_TIMING_PARAMS,
        simulate_input=True,
        decoding=True,
        pad_block_duration=False,
    )

    with pytest.raises(LabgraphError):
        generator.generate_events()
