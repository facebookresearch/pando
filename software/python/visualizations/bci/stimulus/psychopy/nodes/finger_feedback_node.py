#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import logging
import time
from typing import Dict, Optional, Set

import labgraph as df
from bci.stimulus.psychopy.constants import (
    BAD_BEHAVIOR_RESET_TIME,
    BEHAVIOR_BUFFER_TIME,
    MINIMUM_SLEEP_TIME,
    BehavioralFeedbackStatus,
    Condition,
    CorrectConditionEvents,
    FingerTapEvent,
    FingerHapticStimulationCondition,
    TrialMode,
    VariableDurationFingerTappingCondition,
)
from bci.stimulus.psychopy.messages import (
    BehavioralFeedbackMessage,
    BlockMessage,
    ExperimentInfoMessage,
    FingerTapMessage,
    InstructionsMessage,
    TrialModeMessage,
)


class FingerFeedbackNodeConfig(df.Config):
    # If None, then no feedback is given
    correct_condition_events: Optional[CorrectConditionEvents] = None


class FingerFeedbackNode(df.Node):
    # Subscribed topics
    FINGER_TAP_TOPIC = df.Topic(FingerTapMessage)
    TRIAL_MODE_TOPIC = df.Topic(TrialModeMessage)
    EXPERIMENT_INFO_TOPIC = df.Topic(ExperimentInfoMessage)
    BLOCK_TOPIC = df.Topic(BlockMessage)
    INSTRUCTIONS_TOPIC = df.Topic(InstructionsMessage)
    TERMINATION_TOPIC = df.Topic(df.TerminationMessage)

    # Published topics
    BEHAVIORAL_FEEDBACK_TOPIC = df.Topic(BehavioralFeedbackMessage)

    config: FingerFeedbackNodeConfig

    def setup(self) -> None:
        self._trial_mode = None
        self._time_allow_fingertap = 0.0
        self._condition = None
        self._bad_behavior_timestamp = None
        self._shutdown = None
        self._shutdown = asyncio.Event()

        # We create a queue for the feedback messges because we can't have multiple
        # publishers on a topic. Instead, we have a single publisher publishing from the
        # queue and multiple methods writing to the queue.

        # We need to initialize the asyncio.Queue inside the event loop, not here,
        # otherwise we'll get RuntimeError: got Future attached to a different loop
        self._feedback_queue: Optional[asyncio.Queue] = None

    def _tap_status(self, tap: FingerTapEvent) -> Optional[BehavioralFeedbackStatus]:
        # undefined keys are ignored only during non-trial time
        # otherwise they are incorrect
        if (
            self._trial_mode is None
            or self._trial_mode is TrialMode.UNDEFINED
            or self._trial_mode is TrialMode.BLOCK_DONE
        ):
            return None

        # tapping during the non-performance outside buffer time is incorrect
        if (
            self._trial_mode != TrialMode.PERFORMANCE
            and time.time() > self._time_allow_fingertap
        ):
            return BehavioralFeedbackStatus.INCORRECT

        # if we get here, tapping is during performance the effective performance period
        if self.is_correct_tap(tap):
            return BehavioralFeedbackStatus.CORRECT

        return BehavioralFeedbackStatus.INCORRECT

    def is_correct_tap(self, tap: FingerTapEvent) -> bool:
        return (
            self._condition in self.config.correct_condition_events
            and tap in self.config.correct_condition_events[self._condition]
        )

    @df.subscriber(EXPERIMENT_INFO_TOPIC)
    def experiment_info(self, message: ExperimentInfoMessage) -> None:
        self.non_trial_mode(message)

    @df.subscriber(BLOCK_TOPIC)
    def block_topic(self, message: BlockMessage) -> None:
        self.non_trial_mode(message)

    @df.subscriber(INSTRUCTIONS_TOPIC)
    def instruction_topic(self, message: InstructionsMessage) -> None:
        self.non_trial_mode(message)

    def non_trial_mode(self, message: df.TimestampedMessage) -> None:
        self._trial_mode = None
        self._condition = None

    @df.subscriber(TRIAL_MODE_TOPIC)
    def trial_mode(self, message: TrialModeMessage) -> None:
        self._trial_mode = message.trial_mode
        if self._trial_mode == TrialMode.POST_STIMULUS:
            # Don't penalize buffer_time after performance period end
            self._time_allow_fingertap = message.timestamp + BEHAVIOR_BUFFER_TIME
        self._condition = message.condition

    @df.subscriber(TERMINATION_TOPIC)
    def terminate(self, message: df.TerminationMessage) -> None:
        self._shutdown.set()
        raise df.NormalTermination()

    @df.subscriber(FINGER_TAP_TOPIC)
    async def finger_tap_feedback(self, message: FingerTapMessage) -> df.AsyncPublisher:
        await self._setup_feedback_queue()
        if self.config.correct_condition_events is None:
            # If config is empty, no config will be given
            tap_status = None
        else:
            tap_status = self._tap_status(message.event)

        if tap_status == BehavioralFeedbackStatus.INCORRECT:
            self._bad_behavior_timestamp = message.timestamp
        else:
            self._bad_behavior_timestamp = None

        if tap_status is not None:
            await self._feedback_queue.put(
                BehavioralFeedbackMessage(status=tap_status, timestamp=time.time())
            )

    # yield BehavioralFeedbackStatus.RESET
    # n seconds after BehavioralFeedbackStatus.INCORRECT
    @df.background
    async def reset_incorrect_feedback(self) -> None:
        await self._setup_feedback_queue()
        while not self._shutdown.is_set():
            if self._bad_behavior_timestamp is not None:
                if (
                    time.time() - self._bad_behavior_timestamp
                    >= BAD_BEHAVIOR_RESET_TIME
                ):
                    self._bad_behavior_timestamp = None
                    await self._feedback_queue.put(
                        BehavioralFeedbackMessage(
                            status=BehavioralFeedbackStatus.RESET, timestamp=time.time()
                        )
                    )
            await asyncio.sleep(MINIMUM_SLEEP_TIME)

    @df.publisher(BEHAVIORAL_FEEDBACK_TOPIC)
    async def feedback_publisher(self) -> df.AsyncPublisher:
        await self._setup_feedback_queue()
        while not self._shutdown.is_set():
            feedback_message = await self._feedback_queue.get()
            yield self.BEHAVIORAL_FEEDBACK_TOPIC, feedback_message

    async def _setup_feedback_queue(self) -> None:
        if self._feedback_queue is None:
            self._feedback_queue = asyncio.Queue()
