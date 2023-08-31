#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time

from bci.stimulus.psychopy.constants import MINIMUM_SLEEP_TIME
from bci.stimulus.psychopy.messages import KeyboardInputMessage
from bci.stimulus.psychopy.util import ClockConverter
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
from psychopy import event


class KeyboardInputNode(Node):
    KEYBOARD_INPUT_TOPIC = Topic(KeyboardInputMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)

    def __init__(self) -> None:
        super().__init__()
        self._clock_converter = ClockConverter()
        self._keys = []
        self._keys_available = asyncio.Event()
        self._shutdown = asyncio.Event()

    @main
    def render(self):
        while not self._shutdown.is_set():
            if not self._keys_available.is_set():
                self._keys = event.getKeys(timeStamped=self._clock_converter.clock)
                self._keys_available.set()
            time.sleep(MINIMUM_SLEEP_TIME)

    @publisher(KEYBOARD_INPUT_TOPIC)
    async def keyboard_input(self) -> AsyncPublisher:
        while not self._shutdown.is_set():
            if self._keys_available.is_set():
                for keypress in self._keys:
                    keypress_name = keypress[0]
                    keypress_tDown = keypress[1]
                    yield (
                        self.KEYBOARD_INPUT_TOPIC,
                        KeyboardInputMessage(
                            key=keypress_name,
                            timestamp=self._clock_converter.to_system(keypress_tDown),
                        ),
                    )
                self._keys = []
                self._keys_available.clear()
            await asyncio.sleep(MINIMUM_SLEEP_TIME)

    @subscriber(TERMINATION_TOPIC)
    def terminate(self, message: TerminationMessage) -> None:
        self._shutdown.set()
        raise NormalTermination()
