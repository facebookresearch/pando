#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import logging
import time
from typing import Any

import labgraph as df
import pytest
from bci.stimulus.psychopy.constants import (
    BAD_BEHAVIOR_RESET_TIME,
    BEHAVIOR_BUFFER_TIME,
    BehavioralFeedbackStatus,
    FingerTapEvent,
    TrialMode,
    VariableDurationFingerTappingCondition,
    FingerHapticStimulationCondition,
    Instructions,
)
from bci.stimulus.psychopy.messages import (
    BehavioralFeedbackMessage,
    BlockMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    FingerTapMessage,
    TrialModeMessage,
)
from bci.stimulus.psychopy.nodes import (
    FingerFeedbackNode,
    FingerFeedbackNodeConfig,
)
from bci.stimulus.psychopy.protocols.finger_tapping_feedback_params import (
    VALID_DIGIT_BY_CONDITION,
    VALID_FINGERS_BY_CONDITION,
)
from labgraph import NormalTermination
from labgraph.events import TerminationMessage


pytest_plugins = ["pytest_mock"]


def test_incorrect_feedback() -> None:
    """
    Test node gives feedback for incorrect actions appropriately
    """
    test_harness = df.NodeTestHarness(FingerFeedbackNode)
    config = FingerFeedbackNodeConfig(correct_condition_events=VALID_DIGIT_BY_CONDITION)
    with test_harness.get_node(config=config) as node:
        # Pressing during NULL
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.PERFORMANCE,
                trial=0,
                condition=FingerHapticStimulationCondition.NULL,
                timestamp=time.time(),
                block=0,
            )
        )
        df.run_async(
            node.finger_tap_feedback,
            args=(
                FingerTapMessage(event=FingerTapEvent.LEFT_RING, timestamp=time.time()),
            ),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.INCORRECT

        # Pressing wrong finger on the same hand
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.PERFORMANCE,
                trial=1,
                condition=FingerHapticStimulationCondition.LEFT_INDEX,
                timestamp=time.time(),
                block=0,
            )
        )
        df.run_async(
            node.finger_tap_feedback,
            args=(
                FingerTapMessage(event=FingerTapEvent.LEFT_RING, timestamp=time.time()),
            ),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.INCORRECT

        # Pressing undefined finger during right performance
        df.run_async(
            node.finger_tap_feedback,
            args=(
                FingerTapMessage(event=FingerTapEvent.UNDEFINED, timestamp=time.time()),
            ),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.INCORRECT

        # Pressing during post-stimulus
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.POST_STIMULUS,
                trial=1,
                condition=FingerHapticStimulationCondition.LEFT_INDEX,
                timestamp=time.time(),
                block=0,
            )
        )
        time.sleep(BEHAVIOR_BUFFER_TIME)
        df.run_async(
            node.finger_tap_feedback,
            args=(
                FingerTapMessage(
                    event=FingerTapEvent.LEFT_INDEX, timestamp=time.time()
                ),
            ),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.INCORRECT


def test_correct_feedback() -> None:
    """
    Test node gives feedback for correct actions appropriately
    """
    test_harness = df.NodeTestHarness(FingerFeedbackNode)
    config = FingerFeedbackNodeConfig(
        correct_condition_events=VALID_FINGERS_BY_CONDITION
    )
    with test_harness.get_node(config=config) as node:
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.PERFORMANCE,
                trial=0,
                condition=VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
                timestamp=time.time(),
                block=0,
            )
        )
        for finger in [
            FingerTapEvent.LEFT_INDEX,
            FingerTapEvent.LEFT_MIDDLE,
            FingerTapEvent.LEFT_RING,
            FingerTapEvent.LEFT_PINKY,
            FingerTapEvent.LEFT_THUMB,
        ]:
            df.run_async(
                node.finger_tap_feedback,
                args=(FingerTapMessage(event=finger, timestamp=time.time()),),
            )
            feedback_msg = df.run_async(node._feedback_queue.get)
            assert feedback_msg.status == BehavioralFeedbackStatus.CORRECT


def test_ignore_tap_event() -> None:
    """
    Test node ignore any tapping events not during trials
    """
    test_harness = df.NodeTestHarness(FingerFeedbackNode)
    config = FingerFeedbackNodeConfig(
        correct_condition_events=VALID_FINGERS_BY_CONDITION
    )
    with test_harness.get_node(config=config) as node:
        # Undefined trial mode -- instruction screens, etc
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.UNDEFINED,
                trial=-1,
                condition=VariableDurationFingerTappingCondition.UNDEFINED,
                timestamp=time.time(),
                block=-1,
            )
        )
        for tap_event in FingerTapEvent:
            logging.info(f"Giving tap_event: {tap_event}")
            df.run_async(
                node.finger_tap_feedback,
                args=(FingerTapMessage(event=tap_event, timestamp=time.time()),),
            )
            assert node._feedback_queue.empty()
        # BLOCK_DONE
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.BLOCK_DONE,
                trial=-1,
                condition=VariableDurationFingerTappingCondition.UNDEFINED,
                timestamp=time.time(),
                block=0,
            )
        )
        for tap_event in FingerTapEvent:
            df.run_async(
                node.finger_tap_feedback,
                args=(FingerTapMessage(event=tap_event, timestamp=time.time()),),
            )
            assert node._feedback_queue.empty()


def test_tap_buffer(mocker: Any) -> None:
    """
    Test that taps that happen shortly after post-stimulus
    starts are not penalized
    """
    test_harness = df.NodeTestHarness(FingerFeedbackNode)
    config = FingerFeedbackNodeConfig(
        correct_condition_events=VALID_FINGERS_BY_CONDITION
    )
    now = time.time()

    with test_harness.get_node(config=config) as node:
        node.trial_mode(
            TrialModeMessage(
                trial_mode=TrialMode.POST_STIMULUS,
                trial=0,
                condition=VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
                timestamp=time.time(),
                block=0,
            )
        )
        mock_time = mocker.patch(
            "arvr.bci.stimulus.psychopy.nodes.finger_feedback_node.time"
        )
        mock_time.time.return_value = now + BEHAVIOR_BUFFER_TIME / 2
        # Correct tap half buffer time after mode change considered correct
        df.run_async(
            node.finger_tap_feedback,
            args=(FingerTapMessage(event=FingerTapEvent.LEFT_INDEX, timestamp=now),),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.CORRECT

        # Incorrect tap half buffer time after mode change considered wrong
        df.run_async(
            node.finger_tap_feedback,
            args=(FingerTapMessage(event=FingerTapEvent.RIGHT_INDEX, timestamp=now),),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.INCORRECT

        # Correct tap beyond buffer time is incorrect
        mock_time = mocker.patch(
            "arvr.bci.stimulus.psychopy.nodes.finger_feedback_node.time"
        )
        mock_time.time.return_value = now + BEHAVIOR_BUFFER_TIME + 0.01
        df.run_async(
            node.finger_tap_feedback,
            args=(FingerTapMessage(event=FingerTapEvent.LEFT_INDEX, timestamp=now),),
        )
        feedback_msg = df.run_async(node._feedback_queue.get)
        assert feedback_msg.status == BehavioralFeedbackStatus.INCORRECT
