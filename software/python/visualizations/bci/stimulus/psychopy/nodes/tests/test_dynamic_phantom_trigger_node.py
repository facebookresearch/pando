#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time
from typing import Any

import labgraph as df
from bci.constants.laterality import ActivationSide
from bci.messages.dynamic_phantom import DynamicPhantomTriggerMessage
from bci.stimulus.psychopy.constants import (
    TrialMode,
    VariableDurationFingerTappingCondition,
    ViewChangeType,
)
from bci.stimulus.psychopy.messages import ViewChangeMessage
from bci.stimulus.psychopy.nodes import (
    DynamicPhantomTriggerConfig,
    DynamicPhantomTriggerNode,
)


pytest_plugins = ["pytest_mock"]


def _get_test_config() -> DynamicPhantomTriggerConfig:
    test_stimulus_activations = {
        VariableDurationFingerTappingCondition.HAND_LEFT_MID: ActivationSide.RIGHT
    }
    return DynamicPhantomTriggerConfig(stimulus_activations=test_stimulus_activations)


def _get_test_harness() -> df.NodeTestHarness:
    return df.NodeTestHarness(DynamicPhantomTriggerNode)


def test_transformer_stimulus_to_trigger_non_stimulus() -> None:
    now = time.time()
    message = ViewChangeMessage(
        block=1,
        trial=1,
        condition=VariableDurationFingerTappingCondition.HAND_LEFT_MID,
        trial_mode=TrialMode.UNDEFINED,
        change_type=ViewChangeType.UNDEFINED,
        rendered_at=now - 0.1,
        timestamp=now,
    )
    test_config = _get_test_config()
    test_harness = _get_test_harness()
    with test_harness.get_node(config=test_config) as node:
        result = df.run_async(node.stimulus_to_trigger, args=[message])
    assert result == []


def test_transformer_stimulus_to_trigger_non_performance() -> None:
    now = time.time()
    message = ViewChangeMessage(
        block=1,
        trial=1,
        condition=VariableDurationFingerTappingCondition.HAND_LEFT_MID,
        trial_mode=TrialMode.UNDEFINED,
        change_type=ViewChangeType.STIMULUS,
        rendered_at=now - 0.1,
        timestamp=now,
    )
    test_config = _get_test_config()
    test_harness = _get_test_harness()
    with test_harness.get_node(config=test_config) as node:
        result = df.run_async(node.stimulus_to_trigger, args=[message])
    assert result == []


def test_transformer_stimulus_to_trigger_non_activation() -> None:
    now = time.time()
    message = ViewChangeMessage(
        block=1,
        trial=1,
        condition=VariableDurationFingerTappingCondition.UNDEFINED,
        trial_mode=TrialMode.PERFORMANCE,
        change_type=ViewChangeType.STIMULUS,
        rendered_at=now - 0.1,
        timestamp=now,
    )
    test_config = _get_test_config()
    test_harness = _get_test_harness()
    with test_harness.get_node(config=test_config) as node:
        result = df.run_async(node.stimulus_to_trigger, args=[message])
    assert result == []


def test_transformer_stimulus_to_trigger_success(mocker: Any) -> None:
    now = time.time()
    mock_time = mocker.patch(
        "bci.stimulus.psychopy.nodes.dynamic_phantom_trigger_node.time"
    )
    mock_time.time.return_value = now
    message = ViewChangeMessage(
        block=1,
        trial=1,
        condition=VariableDurationFingerTappingCondition.HAND_LEFT_MID,
        trial_mode=TrialMode.PERFORMANCE,
        change_type=ViewChangeType.STIMULUS,
        rendered_at=now - 0.1,
        timestamp=now,
    )
    test_config = _get_test_config()
    test_harness = _get_test_harness()
    with test_harness.get_node(config=test_config) as node:
        result = df.run_async(node.stimulus_to_trigger, args=[message])
    assert result == [
        (
            node.TRIGGER_TOPIC,
            DynamicPhantomTriggerMessage(timestamp=now, side=ActivationSide.RIGHT),
        )
    ]
