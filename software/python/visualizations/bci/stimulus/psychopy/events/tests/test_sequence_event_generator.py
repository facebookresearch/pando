#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List, Tuple

import pytest
from bci.stimulus.psychopy.config import (
    FixedSequenceSpec,
    PermutableSequenceSpec,
    TimingParams,
    TrialSpec,
)
from bci.stimulus.psychopy.constants import (
    Instructions,
    TrialMode,
    VariableDurationFingerTappingCondition,
)
from bci.stimulus.psychopy.events import SequenceEventGraphGenerator
from bci.stimulus.psychopy.messages import (
    BlockMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    ShowDecoderFeedbackMessage,
    TrialModeMessage,
)
from labgraph.events import WaitBeginMessage
from labgraph.events.event_generator import EventPublishingHeap
from labgraph.graphs.topic import Topic
from labgraph.util.error import LabgraphError


DEFAULT_BLOCK_NUMBER_DURATION = 2.0
NULL_TIME = 7.0
INTER_STIMULUS_TIME = 22.0
INITIAL_REST_TIME = INTER_STIMULUS_TIME - NULL_TIME
PRE_STIMULUS_TIME = 0.0
SHORT_TAP_TIME = 2.0
LONG_TAP_TIME = 5.0

# Define trial specs: short trial=24, long trial=27, null=7
# 2x HAND_LEFT trial
trial_hand_left_short = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
    pre_stimulus_duration_range=(PRE_STIMULUS_TIME, PRE_STIMULUS_TIME),
    performance_duration_range=(SHORT_TAP_TIME, SHORT_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_hand_left_long = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    pre_stimulus_duration_range=(PRE_STIMULUS_TIME, PRE_STIMULUS_TIME),
    performance_duration_range=(LONG_TAP_TIME, LONG_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

# 2x HAND_RIGHT trials
trial_hand_right_short = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT,
    pre_stimulus_duration_range=(PRE_STIMULUS_TIME, PRE_STIMULUS_TIME),
    performance_duration_range=(SHORT_TAP_TIME, SHORT_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)
trial_hand_right_long = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
    pre_stimulus_duration_range=(PRE_STIMULUS_TIME, PRE_STIMULUS_TIME),
    performance_duration_range=(LONG_TAP_TIME, LONG_TAP_TIME),
    post_stimulus_duration_range=(INTER_STIMULUS_TIME, INTER_STIMULUS_TIME),
)

# 1x NULL trial
trial_null = TrialSpec(
    condition=VariableDurationFingerTappingCondition.NULL,
    pre_stimulus_duration_range=(PRE_STIMULUS_TIME, PRE_STIMULUS_TIME),
    performance_duration_range=(NULL_TIME, NULL_TIME),
    post_stimulus_duration_range=(0.0, 0.0),
)

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

# SequenceSpec1: four sequences drawn from [21212, 12121]
block_sequence_1 = sum(
    [
        start_null_sequence.get_sequence(),
        PermutableSequenceSpec(
            sequence_spec_list=[short_tap_sequence1, short_tap_sequence2]
        ).get_sequence(num_sequences=2, allow_duplicates=True, allow_consecutive=False),
    ],
    [],
)

# SequenceSpec2: four sequences drawn from [34343, 434343]
block_sequence_2 = sum(
    [
        start_null_sequence.get_sequence(),
        PermutableSequenceSpec(
            sequence_spec_list=[long_tap_sequence1, long_tap_sequence2]
        ).get_sequence(num_sequences=2, allow_duplicates=True, allow_consecutive=False),
    ],
    [],
)

DEFAULT_STIMULUS_CONDITIONS = VariableDurationFingerTappingCondition
DEFAULT_STIMULUS_GROUPS = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: 0,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: 0,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: 1,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: 1,
    VariableDurationFingerTappingCondition.NULL: 2,
}
DEFAULT_TIMING_PARAMS = TimingParams(
    initial_rest_duration=INITIAL_REST_TIME,
    final_rest_duration=0.0,
    block_number_duration=DEFAULT_BLOCK_NUMBER_DURATION,
)
DEFAULT_TRIAL_SPEC_LIST = [block_sequence_1, block_sequence_2]
DEFAULT_BLOCKS = 2

# Total time is:
# (24 * 10 + 7 * 2 + 22) +
# (27 * 10 + 7 * 2 + 22)
# = 582 seconds


def test_create_graph_generator() -> None:
    """
    Tests that a valid SequenceEventGraphGenerator can be created and
    made to generate events in the correct order.
    """
    generator = SequenceEventGraphGenerator(
        stimulus_conditions=DEFAULT_STIMULUS_CONDITIONS,
        stimulus_groups=DEFAULT_STIMULUS_GROUPS,
        trial_spec_list=DEFAULT_TRIAL_SPEC_LIST,
        calibration_blocks=6,  # doesn't matter
        timing_params=DEFAULT_TIMING_PARAMS,
        simulate_input=True,
        decoding=False,
        pad_block_duration=False,
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
    assert start_event.message.total_blocks == 2
    assert start_event.message.trial_spec_sets_per_block == -1
    assert start_event.message.expected_experiment_duration == 582.0
    assert start_event.delay == 0.0

    # Block 0 start
    cur_block = 0

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

    # trial start
    cur_time = validate_block(event_heap, "short", 0.0, cur_block)

    # Block 1 start
    cur_block += 1

    # trial start
    cur_time = validate_block(event_heap, "long", cur_time, cur_block)


def validate_block(
    event_heap: EventPublishingHeap, tap_type: str, cur_time: float, cur_block: int
) -> float:
    """
    Assume trial sequence of:
        0121210212120
    with first trial having initial_rest_duration

    Return post-stimulus event time of last event of block
    """
    if tap_type == "short":
        tap_conditions = [
            VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
            VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT,
        ]
        tap_duration = SHORT_TAP_TIME
    else:
        tap_conditions = [
            VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
            VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
        ]
        tap_duration = LONG_TAP_TIME

    # Block start
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == cur_time
    assert isinstance(next_event.message, BlockMessage)
    assert next_event.message.block == cur_block
    assert next_event.delay == 0.0

    # Trial start
    cur_trial = 0
    # 0th trial is NULL, with initial_rest_duration
    (cur_time, cur_condition) = validate_trial_event(
        event_heap=event_heap,
        expected_condition=[VariableDurationFingerTappingCondition.NULL],
        expected_perf_duration=NULL_TIME,
        expected_block=cur_block,
        expected_trial=cur_trial,
        cur_time=cur_time + DEFAULT_TIMING_PARAMS.block_number_duration,
        initial_rest_duration=DEFAULT_TIMING_PARAMS.initial_rest_duration,
    )
    cur_trial += 1
    # 1st trial is left_short or right_short
    (cur_time, cur_condition) = validate_trial_event(
        event_heap=event_heap,
        expected_condition=tap_conditions,
        expected_perf_duration=tap_duration,
        expected_block=cur_block,
        expected_trial=cur_trial,
        cur_time=cur_time,
    )
    cur_trial += 1
    # 2nd - 5th trial is opposite hand short
    while cur_trial < 6:
        expected_condition = (
            tap_conditions[0]
            if (cur_condition == tap_conditions[1])
            else tap_conditions[1]
        )
        (cur_time, cur_condition) = validate_trial_event(
            event_heap=event_heap,
            expected_condition=[expected_condition],
            expected_perf_duration=tap_duration,
            expected_block=cur_block,
            expected_trial=cur_trial,
            cur_time=cur_time + INTER_STIMULUS_TIME,
        )
        cur_trial += 1
    # 6th trial is NULL again
    (cur_time, _) = validate_trial_event(
        event_heap=event_heap,
        expected_condition=[VariableDurationFingerTappingCondition.NULL],
        expected_perf_duration=NULL_TIME,
        expected_block=cur_block,
        expected_trial=cur_trial,
        cur_time=cur_time + INTER_STIMULUS_TIME,
    )
    cur_trial += 1

    # 7th trial is alternate hand, but without ISI
    expected_condition = (
        tap_conditions[0] if (cur_condition == tap_conditions[1]) else tap_conditions[1]
    )
    (cur_time, cur_condition) = validate_trial_event(
        event_heap=event_heap,
        expected_condition=[expected_condition],
        expected_perf_duration=tap_duration,
        expected_block=cur_block,
        expected_trial=cur_trial,
        cur_time=cur_time,
    )
    cur_trial += 1
    # 8th - 11th trial are alternating
    while cur_trial < 12:
        expected_condition = (
            tap_conditions[0]
            if (cur_condition == tap_conditions[1])
            else tap_conditions[1]
        )
        (cur_time, cur_condition) = validate_trial_event(
            event_heap=event_heap,
            expected_condition=[expected_condition],
            expected_perf_duration=tap_duration,
            expected_block=cur_block,
            expected_trial=cur_trial,
            cur_time=cur_time + INTER_STIMULUS_TIME,
        )
        cur_trial += 1
    # 12th trial is NULL
    (cur_time, _) = validate_trial_event(
        event_heap=event_heap,
        expected_condition=[VariableDurationFingerTappingCondition.NULL],
        expected_perf_duration=NULL_TIME,
        expected_block=cur_block,
        expected_trial=cur_trial,
        cur_time=cur_time + INTER_STIMULUS_TIME,
    )
    cur_trial += 1

    # Block done message
    accumulated_time, _, next_event = event_heap.pop()
    # cur_time advanced by post-stimulus of NULL trial (0),
    # and final_rest_duration (also 0)
    assert accumulated_time == cur_time
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.BLOCK_DONE
    assert next_event.message.block == cur_block
    assert next_event.message.trial == -1
    assert (
        next_event.message.condition == VariableDurationFingerTappingCondition.UNDEFINED
    )
    assert next_event.delay == 0.0

    return cur_time


def validate_trial_event(
    event_heap: EventPublishingHeap,
    expected_condition: List[VariableDurationFingerTappingCondition],
    expected_perf_duration: float,
    expected_block: int,
    expected_trial: int,
    cur_time: float,
    initial_rest_duration: float = 0.0,
) -> Tuple[float, VariableDurationFingerTappingCondition]:
    """
    Pop one trial's worth of events from `event_heap`, check all
    the events are as expected, and return the tuple:

    [time_of_post_stimulus, trial_condition]
    """

    # Check prestimulus
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == cur_time
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.PRE_STIMULUS
    assert next_event.message.block == expected_block
    assert next_event.message.trial == expected_trial
    message_condition = next_event.message.condition
    assert any(message_condition == condition for condition in expected_condition)
    assert next_event.delay == 0.0
    cur_time = accumulated_time

    # performance
    accumulated_time, _, next_event = event_heap.pop()
    if initial_rest_duration > 0:
        assert accumulated_time == cur_time + initial_rest_duration
    else:
        assert accumulated_time == cur_time + PRE_STIMULUS_TIME
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.PERFORMANCE
    assert next_event.message.block == expected_block
    assert next_event.message.trial == expected_trial
    assert next_event.message.condition == message_condition
    assert next_event.delay == 0.0
    cur_time = accumulated_time

    # post-stimulus
    accumulated_time, _, next_event = event_heap.pop()
    assert accumulated_time == cur_time + expected_perf_duration
    assert isinstance(next_event.message, TrialModeMessage)
    assert next_event.message.trial_mode == TrialMode.POST_STIMULUS
    assert next_event.message.block == expected_block
    assert next_event.message.trial == expected_trial
    assert next_event.message.condition == message_condition
    assert next_event.delay == 0.0
    cur_time = accumulated_time

    return (cur_time, message_condition)


def test_generator_topics_must_be_set():
    """
    Tests that an SequenceEventGraphGenerator cannot generate events
    until topics are set for it.
    """
    generator = SequenceEventGraphGenerator(
        stimulus_conditions=DEFAULT_STIMULUS_CONDITIONS,
        stimulus_groups=DEFAULT_STIMULUS_GROUPS,
        trial_spec_list=DEFAULT_TRIAL_SPEC_LIST,
        calibration_blocks=6,  # doesn't matter
        timing_params=DEFAULT_TIMING_PARAMS,
        simulate_input=True,
        decoding=False,
        pad_block_duration=False,
    )
    with pytest.raises(LabgraphError):
        generator.generate_events()
