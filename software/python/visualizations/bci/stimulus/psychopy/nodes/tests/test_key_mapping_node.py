#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time
from typing import Any

import pytest
from bci.stimulus.psychopy.config import KeyMapping, get_participant_key_mapping
from bci.stimulus.psychopy.constants import FingerTapEvent
from bci.stimulus.psychopy.messages import FingerTapMessage, KeyboardInputMessage
from bci.stimulus.psychopy.nodes import KeyMappingNode
from labgraph.util.error import LabgraphError


pytest_plugins = ["pytest_mock"]


@pytest.fixture
def event_loop():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    yield loop
    loop.close()


async def _async_generator_to_list(func, *args, **kwargs):
    result = []
    async for retval in func(*args, **kwargs):
        result.append(retval)
    return result


def test_transformer_finger_tap_no_config(event_loop: Any) -> None:
    node = KeyMappingNode()
    message = KeyboardInputMessage(key="space", timestamp=time.time())
    with pytest.raises(LabgraphError):
        _ = event_loop.run_until_complete(
            _async_generator_to_list(node.finger_tap, message)
        )


def test_transformer_finger_tap_undefined(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.key_mapping_node.time")
    mock_time.time.return_value = now
    config = KeyMapping({})
    node = KeyMappingNode()
    node.set_config(config)
    message = KeyboardInputMessage(key="N/A", timestamp=time.time())
    result = event_loop.run_until_complete(
        _async_generator_to_list(node.finger_tap, message)
    )
    assert result == [
        (
            node.FINGER_TAP_TOPIC,
            FingerTapMessage(event=FingerTapEvent.UNDEFINED, timestamp=now),
        )
    ]


def test_transformer_finger_tap(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.key_mapping_node.time")
    mock_time.time.return_value = now
    config = KeyMapping({"0": FingerTapEvent.LEFT_PINKY})
    node = KeyMappingNode()
    node.set_config(config)
    message = KeyboardInputMessage(key="0", timestamp=time.time())
    result = event_loop.run_until_complete(
        _async_generator_to_list(node.finger_tap, message)
    )
    assert result == [
        (
            node.FINGER_TAP_TOPIC,
            FingerTapMessage(event=FingerTapEvent.LEFT_PINKY, timestamp=now),
        )
    ]


def test_transformer_finger_tap_default_mapping(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.key_mapping_node.time")
    mock_time.time.return_value = now
    config = get_participant_key_mapping("MST704")
    node = KeyMappingNode()
    node.set_config(config)
    message = KeyboardInputMessage(key="4", timestamp=time.time())
    result = event_loop.run_until_complete(
        _async_generator_to_list(node.finger_tap, message)
    )
    assert result == [
        (
            node.FINGER_TAP_TOPIC,
            FingerTapMessage(event=FingerTapEvent.LEFT_INDEX, timestamp=now),
        )
    ]


def test_transformer_finger_tap_participant_mapping(
    event_loop: Any, mocker: Any
) -> None:
    now = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.key_mapping_node.time")
    mock_time.time.return_value = now
    config = get_participant_key_mapping("XIH289")
    node = KeyMappingNode()
    node.set_config(config)
    message = KeyboardInputMessage(key="5", timestamp=time.time())
    result = event_loop.run_until_complete(
        _async_generator_to_list(node.finger_tap, message)
    )
    assert result == [
        (
            node.FINGER_TAP_TOPIC,
            FingerTapMessage(event=FingerTapEvent.LEFT_INDEX, timestamp=now),
        )
    ]
