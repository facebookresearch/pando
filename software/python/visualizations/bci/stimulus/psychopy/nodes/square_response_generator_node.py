#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio

from labgraph import NormalTermination
from labgraph.events import TerminationMessage
from labgraph.graphs import (
    AsyncPublisher,
    publisher,
    subscriber,
    Topic,
)
from labgraph.messages import NumpyType, TimestampedMessage
from labgraph.simulations import FunctionGeneratorNode

from ..messages import TrialModeMessage
from ..simulations import SquareResponseGeneratorState


TEST_NUMPY_SHAPE = (3,)
PUBLISH_DELAY = 1.0


class SquareResponseSampleMessage(TimestampedMessage):
    data: NumpyType(shape=TEST_NUMPY_SHAPE)


class SquareResponseGeneratorNode(FunctionGeneratorNode):
    # Subscribed topics
    TRIAL_MODE_TOPIC = Topic(TrialModeMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)

    # Published topics
    SAMPLE_TOPIC = Topic(SquareResponseSampleMessage)

    # Experiment state
    state: SquareResponseGeneratorState

    def __init__(self) -> None:
        super().__init__()
        self._should_continue = asyncio.Event()

    @subscriber(TRIAL_MODE_TOPIC)
    def trial_mode_sink(self, message: TrialModeMessage) -> None:
        self.state.trial_mode = message.trial_mode
        self.state.condition = message.condition

    @subscriber(TERMINATION_TOPIC)
    def termination_sink(self, message: TerminationMessage) -> None:
        self._should_continue.set()
        raise NormalTermination()

    @publisher(SAMPLE_TOPIC)
    async def publish_samples(self) -> AsyncPublisher:
        while not self._should_continue.is_set():
            next_sample = self._generator.next_sample(self.state)
            yield self.SAMPLE_TOPIC, SquareResponseSampleMessage(
                timestamp=self._clock.get_time(), data=next_sample
            )
            await asyncio.sleep(PUBLISH_DELAY)
