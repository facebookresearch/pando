#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import time
from contextlib import contextmanager
from typing import Any

import pytest
from bci.stimulus.psychopy.constants import (
    BehavioralFeedbackStatus,
    Instructions,
    TrialMode,
    VariableDurationFingerTappingCondition,
    ViewChangeType,
)
from bci.stimulus.psychopy.messages import (
    BehavioralFeedbackMessage,
    BlockMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    KeyboardInputMessage,
    TrialModeMessage,
    ViewChangeMessage,
)
from bci.stimulus.psychopy.nodes.display_node import (
    DisplayNode,
    StimInfo,
    StimType,
    ViewChangeParameters,
)
from bci.stimulus.psychopy.protocols.finger_tapping_duration import DISPLAY_CONFIG
from labgraph import NormalTermination
from labgraph.events import (
    TerminationMessage,
    WaitBeginMessage,
    WaitEndMessage,
)
from PIL import Image


MINIMUM_SLEEP_TIME = 0.2
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


def test_subscriber_experiment_info() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    message = ExperimentInfoMessage(
        total_blocks=1,
        trial_spec_sets_per_block=2,
        expected_experiment_duration=60.0,
        timestamp=time.time(),
        condition_set=VariableDurationFingerTappingCondition.__name__,
    )
    display_node.experiment_info(message)
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.TEXT,
            attrs={"text": DISPLAY_CONFIG.experiment_info.format(1.0)},
        )
    ]


def test_subscriber_block() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    message = BlockMessage(block=0, timestamp=time.time())
    display_node.block(message)
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.TEXT,
            attrs={"text": DISPLAY_CONFIG.block_information.format(1)},
        )
    ]


def test_subscriber_instructions_key_mapping() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    message = InstructionsMessage(
        instructions=Instructions.KEY_MAPPING, timestamp=time.time()
    )
    display_node.instructions(message)
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.TEXT, attrs={"text": DISPLAY_CONFIG.key_mapping_instructions}
        )
    ]


def test_subscriber_instructions_protocol() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    message = InstructionsMessage(
        instructions=Instructions.PROTOCOL, timestamp=time.time()
    )
    display_node.instructions(message)
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.TEXT, attrs={"text": DISPLAY_CONFIG.protocol_instructions}
        )
    ]


def test_subscriber_trial_mode_fixation() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    assert display_node._block == -1
    assert display_node._trial == -1
    assert display_node._condition == VariableDurationFingerTappingCondition.UNDEFINED
    assert display_node._trial_mode == TrialMode.UNDEFINED
    message = TrialModeMessage(
        block=0,
        trial=0,
        condition=VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
        trial_mode=TrialMode.PRE_STIMULUS,
        timestamp=time.time(),
    )
    display_node.trial_mode(message)
    assert display_node._block == 0
    assert display_node._trial == 0
    assert (
        display_node._condition
        == VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    )
    assert display_node._trial_mode == TrialMode.PRE_STIMULUS
    assert display_node._drawing_manager._to_draw == [
        StimInfo(type_=StimType.FIXATION, attrs={})
    ]


def test_subscriber_trial_mode_performance() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    assert display_node._block == -1
    assert display_node._trial == -1
    assert display_node._condition == VariableDurationFingerTappingCondition.UNDEFINED
    assert display_node._trial_mode == TrialMode.UNDEFINED
    message = TrialModeMessage(
        block=0,
        trial=0,
        condition=VariableDurationFingerTappingCondition.HAND_LEFT_SHORT,
        trial_mode=TrialMode.PERFORMANCE,
        timestamp=time.time(),
    )
    display_node.trial_mode(message)
    assert display_node._block == 0
    assert display_node._trial == 0
    assert (
        display_node._condition
        == VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    )
    assert display_node._trial_mode == TrialMode.PERFORMANCE
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.IMAGE,
            attrs={
                "image": Image.open(
                    "arvr/bci/stimulus/psychopy/images/files/hand_left.png"
                ),
                "pos": (-75, 0),
            },
        )
    ]


def test_subscriber_trial_mode_block_done() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    display_node._total_blocks = 3
    display_node._block = 0
    display_node._trial = 0
    display_node._condition = VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    display_node._trial_mode = TrialMode.PERFORMANCE
    message = TrialModeMessage(
        block=0,
        trial=-1,
        condition=VariableDurationFingerTappingCondition.UNDEFINED,
        trial_mode=TrialMode.BLOCK_DONE,
        timestamp=time.time(),
    )
    display_node.trial_mode(message)
    assert display_node._trial == -1
    assert display_node._condition == VariableDurationFingerTappingCondition.UNDEFINED
    assert display_node._trial_mode == TrialMode.BLOCK_DONE
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.TEXT,
            attrs={"text": DISPLAY_CONFIG.block_done_instructions.format(1, 3)},
        )
    ]


def test_subscriber_trial_mode_experiment_done() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    display_node._total_blocks = 2
    display_node._block = 1
    display_node._trial = 0
    display_node._condition = VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    display_node._trial_mode = TrialMode.PERFORMANCE
    message = TrialModeMessage(
        block=1,
        trial=-1,
        condition=VariableDurationFingerTappingCondition.UNDEFINED,
        trial_mode=TrialMode.BLOCK_DONE,
        timestamp=time.time(),
    )
    display_node.trial_mode(message)
    assert display_node._trial == -1
    assert display_node._condition == VariableDurationFingerTappingCondition.UNDEFINED
    assert display_node._trial_mode == TrialMode.BLOCK_DONE
    assert display_node._drawing_manager._to_draw == [
        StimInfo(
            type_=StimType.TEXT,
            attrs={"text": DISPLAY_CONFIG.experiment_done_instructions.format(2, 3)},
        )
    ]


def test_subscriber_wait_for_input(mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.display_node.time")
    mock_time.time.return_value = now
    display_node = DisplayNode()
    message = WaitBeginMessage(timeout=0.0, timestamp=time.time())
    display_node.wait_for_input(message)
    assert display_node._wait_start == now


def test_subscriber_terminate() -> None:
    display_node = DisplayNode()
    display_node.setup()
    message = TerminationMessage(timestamp=time.time())
    display_node._wait_start = time.time()
    with pytest.raises(NormalTermination):
        display_node.terminate(message)
    assert display_node._shutdown.is_set()
    assert display_node._wait_start is None
    assert display_node._received_input.is_set()


def test_subscriber_behavioral_feedback() -> None:
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    message = BehavioralFeedbackMessage(
        timestamp=time.time(), status=BehavioralFeedbackStatus.INCORRECT
    )
    display_node.behavioral_feedback(message)
    assert display_node._drawing_manager.is_dirty is True
    assert display_node._drawing_manager._to_draw == [
        StimInfo(type_=StimType.INCORRECT, attrs={})
    ]


def test_publisher_view_change(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    view_change_parameters = ViewChangeParameters(
        block=-1,
        trial=-1,
        condition=VariableDurationFingerTappingCondition.UNDEFINED,
        trial_mode=TrialMode.UNDEFINED,
        rendered_at=now + 0.1,
        change_type=ViewChangeType.UNDEFINED,
    )
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.display_node.time")
    mock_time.time.return_value = now
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    display_node._shutdown = mocker.Mock()
    display_node._shutdown.is_set.side_effect = [False, True]
    display_node._view_changes = [view_change_parameters]  # [now + 0.1]
    messages = event_loop.run_until_complete(
        _async_generator_to_list(display_node.view_change)
    )

    assert messages == [
        (
            display_node.VIEW_CHANGE_TOPIC,
            ViewChangeMessage(
                block=-1,
                trial=-1,
                condition=VariableDurationFingerTappingCondition.UNDEFINED,
                trial_mode=TrialMode.UNDEFINED,
                rendered_at=now + 0.1,
                timestamp=now,
                change_type=ViewChangeType.UNDEFINED,
            ),
        )
    ]


def test_publisher_keyboard_input(event_loop: Any, mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.util.clock_converter.time")
    mock_time.time.return_value = now
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    display_node._shutdown = mocker.Mock()
    display_node._shutdown.is_set.side_effect = [False, True]
    display_node._keys = [("space", 0.1)]
    display_node._keys_available.set()
    messages = event_loop.run_until_complete(
        _async_generator_to_list(display_node.keyboard_input)
    )
    assert display_node._keys == []
    assert not display_node._keys_available.is_set()
    assert messages == [
        (
            display_node.KEYBOARD_INPUT_TOPIC,
            KeyboardInputMessage(key="space", timestamp=now + 0.1),
        )
    ]


def test_publisher_wait_end(event_loop: Any, mocker: Any) -> None:
    wait_start = time.time()
    wait_end = time.time()
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.nodes.display_node.time")
    mock_time.time.return_value = wait_end
    display_node = DisplayNode()
    display_node.set_config(DISPLAY_CONFIG)
    display_node.setup()
    display_node._shutdown = mocker.Mock()
    display_node._shutdown.is_set.side_effect = [False, True]
    display_node._received_input = mocker.Mock()
    display_node._received_input.is_set.side_effect = [False, True]
    display_node._wait_start = wait_start
    messages = event_loop.run_until_complete(
        _async_generator_to_list(display_node.wait_end)
    )
    assert display_node._wait_start is None
    display_node._received_input.clear.assert_called()
    assert messages == [
        (
            display_node.WAIT_END_TOPIC,
            WaitEndMessage(wait_length=wait_end - wait_start, timestamp=wait_end),
        )
    ]


@contextmanager
def _mock_psychopy(mocker: Any, now: float) -> None:
    mock_clock_converter_time = mocker.patch(
        "arvr.bci.stimulus.psychopy.util.clock_converter.time"
    )
    mock_clock_converter_time.time.return_value = now
    mock_get_keys = mocker.patch(
        "arvr.bci.stimulus.psychopy.nodes.display_node.event.getKeys"
    )
    mock_monitors = mocker.patch(
        "arvr.bci.stimulus.psychopy.nodes.display_node.monitors"
    )
    mock_visual = mocker.patch("arvr.bci.stimulus.psychopy.nodes.display_node.visual")
    mock_keypress = mocker.Mock()
    mock_keypress.name = "space"
    mock_keypress.tDown = 1.0
    mock_get_keys.return_value = [(mock_keypress.name, mock_keypress.tDown)]
    DISPLAY_CONFIG.stimulus_images[
        VariableDurationFingerTappingCondition.HAND_LEFT_SHORT
    ] = "unittest"
    mock_monitor = mocker.Mock()
    mock_monitors.Monitor.return_value = mock_monitor
    mock_window = mocker.Mock()
    mock_window.flip.return_value = 0.1
    mock_visual.Window.return_value = mock_window
    mock_fixation_autoDraw = mocker.PropertyMock()
    mock_fixation_stim = mocker.Mock()
    type(mock_fixation_stim).autoDraw = mock_fixation_autoDraw
    mock_text = mocker.PropertyMock()
    mock_text_autoDraw = mocker.PropertyMock()
    mock_text_stim = mocker.Mock()
    type(mock_text_stim).text = mock_text
    type(mock_text_stim).autoDraw = mock_text_autoDraw
    mock_incorrect_autoDraw = mocker.PropertyMock()
    mock_incorrect_stim = mocker.Mock()
    type(mock_incorrect_stim).autoDraw = mock_incorrect_autoDraw
    mock_visual.TextStim.side_effect = [
        mock_fixation_stim,
        mock_text_stim,
        mock_incorrect_stim,
    ]
    mock_image = mocker.PropertyMock()
    mock_image_autoDraw = mocker.PropertyMock()
    mock_image_stim = mocker.Mock()
    type(mock_image_stim).image = mock_image
    type(mock_image_stim).autoDraw = mock_image_autoDraw
    mock_visual.ImageStim.return_value = mock_image_stim
    yield
    assert mock_monitors.mock_calls == [
        mocker.call.Monitor("BCIMonitor", distance=57, width=33.1),
        mocker.call.Monitor().setSizePix((1920, 1080)),
    ]
    assert mock_visual.mock_calls == [
        mocker.call.Window(
            color=(-0.25, -0.25, -0.25),
            fullscr=True,
            monitor=mock_monitor,
            size=(1920, 1080),
            units="pix",
        ),
        mocker.call.TextStim(
            mock_window, text="+", color=(-1, 1, -1), pos=(0, 0), height=100
        ),
        mocker.call.ImageStim(
            mock_window, image=None, units="pix", size=(300, 300), pos=(0, 0)
        ),
        mocker.call.TextStim(mock_window, text="", color=(1, 1, 1), pos=(0, 0)),
        mocker.call.TextStim(
            mock_window, text="X", color=(1, -1, -1), pos=(0, -100), height=100
        ),
        mocker.call.Window().flip(),
        mocker.call.Window().close(),
    ]
    assert mock_fixation_autoDraw.mock_calls == [mocker.call(False)]
    assert mock_fixation_stim.mock_calls == []
    assert mock_text.mock_calls == [mocker.call("unittest")]
    assert mock_text_autoDraw.mock_calls == [mocker.call(False), mocker.call(True)]
    assert mock_text_stim.mock_calls == []
    assert mock_image.mock_calls == []
    assert mock_image_autoDraw.mock_calls == [mocker.call(False)]
    assert mock_image_stim.mock_calls == []
    assert mock_incorrect_autoDraw.mock_calls == [mocker.call(False)]
    assert mock_incorrect_stim.mock_calls == []


def test_main_render(mocker: Any) -> None:
    now = time.time()
    with _mock_psychopy(mocker, now):
        display_node = DisplayNode()
        display_node.set_config(DISPLAY_CONFIG)
        display_node.setup()
        display_node._shutdown = mocker.Mock()
        display_node._shutdown.is_set.side_effect = [False, True]
        display_node._drawing_manager._to_draw = [
            StimInfo(type_=StimType.TEXT, attrs={"text": "unittest"})
        ]
        display_node._drawing_manager._is_dirty = True
        display_node.render()
    assert display_node._keys_available.is_set()
    assert display_node._view_changes[0].rendered_at == now + 0.1
