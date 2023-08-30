#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time
from contextlib import contextmanager
from typing import Any, List

from bci.stimulus.psychopy.constants import MINIMUM_SLEEP_TIME
from bci.stimulus.psychopy.messages import KeyboardInputMessage
from bci.stimulus.psychopy.nodes import KeyboardInputNode
from labgraph.events import TerminationMessage
from labgraph.graphs import (
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


pytest_plugins = ["pytest_mock"]


class MySink(Node):
    KEYBOARD_INPUT_TOPIC = Topic(KeyboardInputMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)

    def __init__(self) -> None:
        super(MySink, self).__init__()
        self.received: List[Message] = []
        self.shutdown = asyncio.Event()

    @subscriber(KEYBOARD_INPUT_TOPIC)
    def keyboard_input_sink(self, message: KeyboardInputMessage) -> None:
        if not self.shutdown.is_set():
            self.received.append(message)
            self.shutdown.set()

    @publisher(TERMINATION_TOPIC)
    async def terminate(self) -> AsyncPublisher:
        while not self.shutdown.is_set():
            await asyncio.sleep(MINIMUM_SLEEP_TIME)
        yield self.TERMINATION_TOPIC, TerminationMessage(timestamp=time.time())


class MyGraph(Graph):
    SOURCE: KeyboardInputNode
    SINK: MySink

    def get_received(self) -> List[Message]:
        return self.SINK.received

    def connections(self) -> Connections:
        return (
            (self.SOURCE.KEYBOARD_INPUT_TOPIC, self.SINK.KEYBOARD_INPUT_TOPIC),
            (self.SINK.TERMINATION_TOPIC, self.SOURCE.TERMINATION_TOPIC),
        )


@contextmanager
def _mock_psychopy(mocker: Any, now: float) -> None:
    mock_time = mocker.patch("bci.stimulus.psychopy.util.clock_converter.time")
    mock_time.time.return_value = now
    mock_get_keys = mocker.patch(
        "bci.stimulus.psychopy.nodes.keyboard_input_node.event.getKeys"
    )
    mock_keypress = mocker.Mock()
    mock_keypress.name = "space"
    mock_keypress.tDown = 0.1
    mock_get_keys.return_value = [(mock_keypress.name, mock_keypress.tDown)]
    yield
    mock_get_keys.assert_called()


def test_reading_keyboard_input(mocker: Any) -> None:
    now = time.time()
    expected_messages = [KeyboardInputMessage(key="space", timestamp=now + 0.1)]
    with _mock_psychopy(mocker, now):
        graph = MyGraph()
        runner = LocalRunner(module=graph)
        runner.run()
        received_messages = graph.get_received()
    assert received_messages == expected_messages
