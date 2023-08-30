#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time
from contextlib import contextmanager
from typing import Any, List

from bci.stimulus.psychopy.constants import MINIMUM_SLEEP_TIME
from bci.stimulus.psychopy.messages import RecordingInfoMessage
from bci.stimulus.psychopy.nodes import RecordingInfoNode
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
    RECORDING_INFO_TOPIC = Topic(RecordingInfoMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)

    def __init__(self) -> None:
        super(MySink, self).__init__()
        self.received: List[Message] = []
        self._shutdown = asyncio.Event()

    @subscriber(RECORDING_INFO_TOPIC)
    def recording_info_sink(self, message: RecordingInfoMessage) -> None:
        self.received.append(message)
        self._shutdown.set()

    @publisher(TERMINATION_TOPIC)
    async def terminate_source(self) -> AsyncPublisher:
        while not self._shutdown.is_set():
            await asyncio.sleep(MINIMUM_SLEEP_TIME)
        yield self.TERMINATION_TOPIC, TerminationMessage(timestamp=time.time())


class MyGraph(Graph):
    SOURCE: RecordingInfoNode
    SINK: MySink

    def get_received(self) -> List[Message]:
        return self.SINK.received

    def connections(self) -> Connections:
        return (
            (self.SINK.TERMINATION_TOPIC, self.SOURCE.TERMINATION_TOPIC),
            (self.SOURCE.RECORDING_INFO_TOPIC, self.SINK.RECORDING_INFO_TOPIC),
        )


@contextmanager
def _mock_psychopy(mocker: Any, ok: bool, now: float) -> None:
    mock_time = mocker.patch("bci.stimulus.psychopy.nodes.recording_info_node.time")
    mock_Dlg = mocker.patch("bci.stimulus.psychopy.nodes.recording_info_node.gui.Dlg")
    mock_time.time.return_value = now
    mock_dialog = mocker.Mock()
    mock_OK = mocker.PropertyMock(return_value=ok)
    type(mock_dialog).OK = mock_OK
    mock_dialog.show.return_value = ("unittest", "1")
    mock_Dlg.return_value = mock_dialog
    yield
    assert mock_Dlg.mock_calls == [
        mocker.call(title="BCI Experiment"),
        mocker.call().addText("Experiment Info"),
        mocker.call().addField("Session ID: "),
        mocker.call().addField("Recording: "),
        mocker.call().show(),
    ]


def test_dialog_ok(mocker: Any) -> None:
    now = time.time()
    expected_messages = [
        RecordingInfoMessage(session_id="unittest", recording=1, timestamp=now)
    ]
    with _mock_psychopy(mocker, ok=True, now=now):
        graph = MyGraph()
        runner = LocalRunner(module=graph)
        runner.run()
        received_messages = graph.get_received()
    assert received_messages == expected_messages


def test_dialog_cancel(mocker: Any) -> None:
    now = time.time()
    expected_messages = [
        RecordingInfoMessage(session_id="", recording=-1, timestamp=now)
    ]
    with _mock_psychopy(mocker, ok=False, now=now):
        graph = MyGraph()
        runner = LocalRunner(module=graph)
        runner.run()
        received_messages = graph.get_received()
    assert received_messages == expected_messages
