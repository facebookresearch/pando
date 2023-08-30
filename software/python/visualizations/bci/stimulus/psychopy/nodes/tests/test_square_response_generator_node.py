#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time
from contextlib import contextmanager
from typing import Any, List

import numpy as np
from labgraph.events import TerminationMessage
from labgraph.v1.graphs import (
    AsyncPublisher,
    Connections,
    Graph,
    Node,
    Topic,
    publisher,
    subscriber,
)
from labgraph.messages.message import Message
from labgraph.runners import LocalRunner
from labgraph.simulations import FunctionGenerator

from ...constants import TrialMode, VariableDurationFingerTappingCondition
from ...messages import TrialModeMessage
from ...simulations import SquareResponseGeneratorState
from ..square_response_generator_node import (
    SquareResponseGeneratorNode,
    SquareResponseSampleMessage,
)


pytest_plugins = ["pytest_mock"]
WAIT_FOR_SAMPLE_DELAY = 0.1


class MyGenerator(FunctionGenerator):
    def next_sample(self, state: SquareResponseGeneratorState) -> np.ndarray:
        return np.array([1.0, 2.0, 3.0])


class MyTestNode(Node):
    TRIAL_MODE_TOPIC = Topic(TrialModeMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)
    SAMPLE_TOPIC = Topic(SquareResponseSampleMessage)

    def __init__(self) -> None:
        super(MyTestNode, self).__init__()
        self.samples: List[Message] = []
        self.state_available = asyncio.Event()
        self.shutdown = asyncio.Event()

    @publisher(TRIAL_MODE_TOPIC)
    async def trial_source(self) -> AsyncPublisher:
        yield (
            self.TRIAL_MODE_TOPIC,
            TrialModeMessage(
                trial_mode=TrialMode.PERFORMANCE,
                block=0,
                trial=0,
                condition=VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT,
                timestamp=time.time(),
            ),
        )
        await asyncio.sleep(0.2)
        self.state_available.set()

    @publisher(TERMINATION_TOPIC)
    async def terminate(self) -> AsyncPublisher:
        while not self.shutdown.is_set():
            await asyncio.sleep(0.01)
        yield self.TERMINATION_TOPIC, TerminationMessage(timestamp=time.time())

    @subscriber(SAMPLE_TOPIC)
    def sample_sink(self, message: SquareResponseSampleMessage) -> None:
        if self.state_available.is_set():
            self.samples.append(message.data)
            self.shutdown.set()
            self.state_available.clear()


class MyGraph(Graph):
    TEST_NODE: MyTestNode
    SAMPLE_SOURCE: SquareResponseGeneratorNode

    def setup(self) -> None:
        generator = MyGenerator()
        self.SAMPLE_SOURCE.set_generator(generator)

    def get_received(self) -> List[Message]:
        return self.TEST_NODE.samples

    def connections(self) -> Connections:
        return (
            (self.TEST_NODE.TRIAL_MODE_TOPIC, self.SAMPLE_SOURCE.TRIAL_MODE_TOPIC),
            (self.TEST_NODE.TERMINATION_TOPIC, self.SAMPLE_SOURCE.TERMINATION_TOPIC),
            (self.SAMPLE_SOURCE.SAMPLE_TOPIC, self.TEST_NODE.SAMPLE_TOPIC),
        )


@contextmanager
def _mock_clock(mocker: Any) -> None:
    now = time.time()
    mock_clock = mocker.Mock()
    mock_clock.get_time.return_value = now
    mock_ExperimentClock = mocker.patch(
        "labgraph.simulations.function_generator_node.ExperimentClock"
    )
    mock_ExperimentClock.return_value = mock_clock
    yield
    mock_ExperimentClock.assert_called()
    mock_clock.get_time.assert_called()


def test_sample_node_generation(mocker: Any) -> None:
    """
    Tests that node configures itself correctly and returns samples from the generator.
    """
    expected = [np.array([1.0, 2.0, 3.0])]
    with _mock_clock(mocker):
        graph = MyGraph()
        runner = LocalRunner(module=graph)
        runner.run()  # Shouldn't raise exception
        received = graph.get_received()
    assert np.array_equal(received, expected)
