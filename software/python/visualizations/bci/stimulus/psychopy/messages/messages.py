#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.


from bci.stimulus.psychopy.constants import (
    BehavioralFeedbackStatus,
    Condition,
    CuffOcclusionCondition,
    DecoderFeedbackStatus,
    FingerTapEvent,
    Instructions,
    TrialMode,
    ViewChangeType,
)
from labgraph.messages import TimestampedMessage


class ExperimentInfoMessage(TimestampedMessage):
    total_blocks: int
    trial_spec_sets_per_block: int
    expected_experiment_duration: float
    condition_set: str


class InstructionsMessage(TimestampedMessage):
    instructions: Instructions


class BlockMessage(TimestampedMessage):
    block: int


class TrialModeMessage(BlockMessage):
    trial_mode: TrialMode
    trial: int
    # TODO (T62479656): Fix derived enum field-types
    condition: int


class KeyboardInputMessage(TimestampedMessage):
    key: str


class RecordingInfoMessage(TimestampedMessage):
    session_id: str
    recording: int


class FingerTapMessage(TimestampedMessage):
    event: FingerTapEvent


class BehavioralFeedbackMessage(TimestampedMessage):
    status: BehavioralFeedbackStatus


class ShowDecoderFeedbackMessage(TimestampedMessage):
    status: DecoderFeedbackStatus


class ViewChangeMessage(TrialModeMessage):
    rendered_at: float
    change_type: ViewChangeType
