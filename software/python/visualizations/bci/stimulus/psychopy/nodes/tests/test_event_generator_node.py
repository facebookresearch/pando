#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time
from typing import Any

import pytest
from bci.stimulus.psychopy.nodes import DerivedEventGeneratorNode
from labgraph.events import (
    BaseEventGenerator,
    EventPublishingHeap,
    WaitBeginMessage,
    WaitEndMessage,
    TerminationMessage,
)
from labgraph.graphs import Topic
from labgraph.util.error import LabgraphError

pytest_plugins = ["pytest_mock"]


@pytest.fixture
def event_loop():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    yield loop
    loop.close()


async def _async_generator_to_list(func):
    result = []
    async for retval in func():
        result.append(retval)
    return result


class MyEventGenerator(BaseEventGenerator):
    def __init__(self):
        self.heap = EventPublishingHeap()
        self.topics_set = False

    def generate_events(self) -> EventPublishingHeap:
        return self.heap

    def set_topics(
        self,
        experiment_info_topic: Topic,
        instructions_topic: Topic,
        trial_mode_topic: Topic,
        block_topic: Topic,
        wait_for_input_topic: Topic,
    ) -> None:
        self.topics_set = True


def test_node_set_generator() -> None:
    generator = MyEventGenerator()
    assert not generator.topics_set
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    assert node._generator == generator
    assert not node.wait_initiated.is_set()
    assert generator.topics_set


def test_subscriber_wait_end() -> None:
    generator = MyEventGenerator()
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    node.wait_initiated.set()
    message = WaitEndMessage(wait_length=0.1, timestamp=time.time())
    node.wait_end(message)
    assert not node.wait_initiated.is_set()


def test_helper_wait_for_wait_end(event_loop: Any, mocker: Any) -> None:
    generator = MyEventGenerator()
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    mock_wait_initiated = mocker.Mock()
    mock_wait_initiated.is_set.side_effect = [True, False]
    node.wait_initiated = mock_wait_initiated
    event_loop.run_until_complete(node._wait_for_wait_end(0.2))
    assert mock_wait_initiated.is_set.mock_calls == [mocker.call(), mocker.call()]


def test_helper_wait_for_wait_end_error(event_loop: Any, mocker: Any) -> None:
    generator = MyEventGenerator()
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    mock_wait_initiated = mocker.Mock()
    mock_wait_initiated.is_set.side_effect = [True, False]
    node.wait_initiated = mock_wait_initiated
    with pytest.raises(LabgraphError):
        event_loop.run_until_complete(node._wait_for_wait_end(0.0))
    assert mock_wait_initiated.is_set.mock_calls == [mocker.call()]


def test_publisher_publish_events(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch(
        "arvr.bci.stimulus.psychopy.nodes.event_generator_node.time"
    )
    mock_time.time.return_value = now
    mock_event = mocker.Mock()
    generator = MyEventGenerator()
    generator.heap.push((0.0, 1, mock_event))
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    messages = event_loop.run_until_complete(
        _async_generator_to_list(node.publish_events)
    )
    assert messages == [
        (mock_event.topic, mock_event.message),
        (node.TERMINATION_TOPIC, TerminationMessage(timestamp=now)),
    ]


def test_publisher_publish_events_warning(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch(
        "arvr.bci.stimulus.psychopy.nodes.event_generator_node.time"
    )
    mock_time.time.return_value = now
    mock_logging = mocker.patch(
        "arvr.bci.stimulus.psychopy.nodes.event_generator_node.logging"
    )
    mock_event = mocker.Mock()
    generator = MyEventGenerator()
    generator.heap.push((-0.1, 1, mock_event))
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    messages = event_loop.run_until_complete(
        _async_generator_to_list(node.publish_events)
    )
    assert messages == [
        (mock_event.topic, mock_event.message),
        (node.TERMINATION_TOPIC, TerminationMessage(timestamp=now)),
    ]
    mock_logging.warning.assert_called_with(
        f"Published event {mock_event} at 0.1 seconds later than expected."
    )


def test_publisher_publish_events_wait_begin(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch(
        "arvr.bci.stimulus.psychopy.nodes.event_generator_node.time"
    )
    mock_time.time.return_value = now
    mock_event = mocker.Mock()
    mock_event.message = WaitBeginMessage(timeout=0.2, timestamp=now)
    mock_wait_initiated = mocker.Mock()
    mock_wait_initiated.is_set.side_effect = [False, True, False]
    generator = MyEventGenerator()
    generator.heap.push((0.0, 1, mock_event))
    node = DerivedEventGeneratorNode()
    node.set_generator(generator)
    node.wait_initiated = mock_wait_initiated
    messages = event_loop.run_until_complete(
        _async_generator_to_list(node.publish_events)
    )
    assert messages == [
        (mock_event.topic, mock_event.message),
        (node.TERMINATION_TOPIC, TerminationMessage(timestamp=now)),
    ]
    assert mock_wait_initiated.mock_calls == [
        mocker.call.is_set(),
        mocker.call.set(),
        mocker.call.is_set(),
        mocker.call.is_set(),
    ]
