#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import logging
import time

from bci.stimulus.psychopy.constants import MINIMUM_SLEEP_TIME
from bci.stimulus.psychopy.messages import RecordingInfoMessage
from labgraph import NormalTermination
from labgraph.events import TerminationMessage
from labgraph.graphs import (
    AsyncPublisher,
    main,
    Node,
    publisher,
    subscriber,
    Topic,
)
from psychopy import gui


class RecordingInfoNode(Node):
    RECORDING_INFO_TOPIC = Topic(RecordingInfoMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)

    def __init__(self) -> None:
        super().__init__()
        self._message = None
        self._message_available = asyncio.Event()
        self._shutdown = asyncio.Event()

    @main
    def open_dialog(self) -> None:
        dialog = gui.Dlg(title="BCI Experiment")
        dialog.addText("Experiment Info")
        dialog.addField("Session ID: ")
        dialog.addField("Recording: ")
        response = dialog.show()
        if not dialog.OK:
            # TODO: Raise an error here (currently makes the process hang)
            logging.warning("Experiment cancelled by user")
            session_id = ""
            recording = -1
        else:
            session_id, recording = response
        self._message = RecordingInfoMessage(
            session_id=session_id, recording=int(recording), timestamp=time.time()
        )
        self._message_available.set()

    @publisher(RECORDING_INFO_TOPIC)
    async def recording_info(self) -> AsyncPublisher:
        while not self._shutdown.is_set():
            if self._message_available.is_set():
                yield self.RECORDING_INFO_TOPIC, self._message
                break
            await asyncio.sleep(MINIMUM_SLEEP_TIME)

    @subscriber(TERMINATION_TOPIC)
    def terminate_sink(self, message: TerminationMessage) -> None:
        self._shutdown.set()
        raise NormalTermination()
