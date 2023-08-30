#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
import time
from asyncio import Event, sleep

from bci.stimulus.psychopy.messages import (
    BlockMessage,
    ShowDecoderFeedbackMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    TrialModeMessage,
)
from labgraph.events import (
    BaseEventGenerator,
    BaseEventGeneratorNode,
    EventPublishingHeap,
    TerminationMessage,
    WaitBeginMessage,
    WaitEndMessage,
)
from labgraph.graphs import AsyncPublisher, Topic, subscriber
from labgraph.util.error import LabgraphError


CHECK_FOR_WAIT_COMPLETION_DELAY = 0.1
ACCEPTABLE_PUBLISH_TIME_DIFF = 0.01


class DerivedEventGeneratorNode(BaseEventGeneratorNode):
    """
    Node which acts as a state-machine.
    """

    EXPERIMENT_INFO_TOPIC = Topic(ExperimentInfoMessage)
    BLOCK_TOPIC = Topic(BlockMessage)
    INSTRUCTIONS_TOPIC = Topic(InstructionsMessage)
    TRIAL_MODE_TOPIC = Topic(TrialModeMessage)
    SHOW_DECODER_FEEDBACK_TOPIC = Topic(ShowDecoderFeedbackMessage)

    WAIT_BEGIN_TOPIC = Topic(WaitBeginMessage)
    WAIT_END_TOPIC = Topic(WaitEndMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)

    def set_generator(self, generator: BaseEventGenerator) -> None:
        super(DerivedEventGeneratorNode, self).setup_generator(generator)
        self.wait_initiated: Event = Event()
        self._generator.set_topics(
            self.EXPERIMENT_INFO_TOPIC,
            self.INSTRUCTIONS_TOPIC,
            self.TRIAL_MODE_TOPIC,
            self.BLOCK_TOPIC,
            self.WAIT_BEGIN_TOPIC,
            self.SHOW_DECODER_FEEDBACK_TOPIC,
        )

    @subscriber(WAIT_END_TOPIC)
    def wait_end(self, message: WaitEndMessage) -> None:
        self.wait_initiated.clear()

    async def publish_events(self) -> AsyncPublisher:
        event_heap: EventPublishingHeap = self.generate_events()

        accumulated_time: float = 0.0  # Track amount of delay time elapsed
        timeout_remaining: float = 0.0
        while event_heap:
            await self._wait_for_wait_end(timeout_remaining)

            # Get next event for publishing
            acc_publish_time, _, next_event = event_heap.pop()

            # Sleep until appropriate message send time
            time_diff = acc_publish_time - accumulated_time
            if time_diff > ACCEPTABLE_PUBLISH_TIME_DIFF:
                await sleep(time_diff)
            elif time_diff < -ACCEPTABLE_PUBLISH_TIME_DIFF:
                logging.warning(
                    f"Published event {next_event} at {abs(time_diff)} seconds "
                    "later than expected."
                )

            accumulated_time = acc_publish_time

            if isinstance(next_event.message, WaitBeginMessage):
                self.wait_initiated.set()
                timeout_remaining = next_event.message.timeout

            yield next_event.topic, next_event.message

        await self._wait_for_wait_end(timeout_remaining)
        # TODO (T62122941): Find better way to signal end of graph run
        yield self.TERMINATION_TOPIC, TerminationMessage(timestamp=time.time())

    async def _wait_for_wait_end(self, timeout_remaining: float) -> None:
        while self.wait_initiated.is_set():
            await sleep(CHECK_FOR_WAIT_COMPLETION_DELAY)
            timeout_remaining -= CHECK_FOR_WAIT_COMPLETION_DELAY
            if timeout_remaining <= 0:
                raise LabgraphError("Timed out on waiting for next event.")
