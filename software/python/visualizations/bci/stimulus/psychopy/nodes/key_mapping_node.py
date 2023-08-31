#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time
from typing import Optional

from bci.stimulus.psychopy.config import KeyMapping
from bci.stimulus.psychopy.messages import FingerTapMessage, KeyboardInputMessage
from labgraph.graphs import (
    AsyncPublisher,
    Node,
    Topic,
    publisher,
    subscriber,
)
from labgraph.util.error import LabgraphError


NAME_TO_SYMBOL = {
    "quoteleft": "`",
    "asciitilde": "~",
    "exclamation": "!",
    "at": "@",
    "pound": "#",
    "dollar": "$",
    "percent": "%",
    "asciicircum": "^",
    "ampersand": "&",
    "asterisk": "*",
    "parenleft": "(",
    "parenright": ")",
    "minus": "-",
    "underscore": "_",
    "equal": "=",
    "plus": "+",
    "bracketleft": "[",
    "braceleft": "{",
    "bracketright": "]",
    "braceright": "}",
    "backslash": "\\",
    # Note: No special name for |
    "semicolon": ";",
    "colon": ":",
    "apostrophe": "'",
    "doublequote": '"',
    "comma": ",",
    "less": "<",
    "period": ".",
    "greater": ">",
    "slash": "/",
    "question": "?",
}


class KeyMappingNode(Node):
    # Subscribed topics
    KEYBOARD_INPUT_TOPIC = Topic(KeyboardInputMessage)

    # Published topics
    FINGER_TAP_TOPIC = Topic(FingerTapMessage)

    def __init__(self) -> None:
        super().__init__()
        self._config: Optional[KeyMapping] = None

    def set_config(self, config: KeyMapping) -> None:
        self._config = config

    @subscriber(KEYBOARD_INPUT_TOPIC)
    @publisher(FINGER_TAP_TOPIC)
    async def finger_tap(self, message: KeyboardInputMessage) -> AsyncPublisher:
        if self._config is None:
            raise LabgraphError("Must call set_config at startup.")
        key = message.key.lower()
        key = NAME_TO_SYMBOL.get(key, key)
        event = self._config.get(key)
        yield (
            self.FINGER_TAP_TOPIC,
            FingerTapMessage(event=event, timestamp=time.time()),
        )
