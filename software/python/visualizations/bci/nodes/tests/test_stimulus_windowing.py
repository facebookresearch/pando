#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import numpy as np
from bci.messages.generic_signal_sample import SignalSampleMessage
from bci.nodes.signal_view_joiner import ArraySignalViewMessage
from bci.nodes.stimulus_windowing import (
    ArrayStimulusWindowTriggerNode,
    StimulusWindowTriggerConfig,
    TriggerCommand,
    TriggerMessage,
)

SAMPLE_RATE_HZ = 10.0
DT = 1 / SAMPLE_RATE_HZ


def test_stimulus_windowing_setup() -> None:
    """
    Trial length of 30 seconds, stimulus onset every 10 seconds
    Config to trigger window of 6 seconds, 5 seconds after onset,
    so the window is [-1, 5] wrt stimulus onset.

    The trigger should return windows 2 times
    """
    window_length = int(6 * SAMPLE_RATE_HZ)
    samples_after_onset = int(5 * SAMPLE_RATE_HZ)
    timestamps = np.arange(0, 30, DT) + DT
    samples_since_stimulus_onset = (
        [None] * np.sum(timestamps < 10)
        + list(range(np.sum((timestamps >= 10) & (timestamps < 20))))
        + list(range(np.sum(timestamps >= 20)))
    )
    array_signal_view_messages = [
        ArraySignalViewMessage(
            block_number=0,
            trial_number=1,
            timestamp=timestamps[i],
            samples_since_stimulus_onset=samples_since_stimulus_onset[i],
            sample=np.random.randn(100),
        )
        for i in range(len(timestamps))
    ]
    test_harness = df.NodeTestHarness(ArrayStimulusWindowTriggerNode)
    n_triggered = 0
    with test_harness.get_node(
        StimulusWindowTriggerConfig(window_length, samples_after_onset)
    ) as node:
        assert node.state.window_length == window_length
        assert node.state.samples_since_stimulus_onset == samples_after_onset
        for input_message in array_signal_view_messages:
            return_list = df.run_async(node.run, args=[input_message])
            cur_samples_after_onset = input_message.samples_since_stimulus_onset
            should_trigger = (
                cur_samples_after_onset is not None
                and cur_samples_after_onset == samples_after_onset
            )
            if should_trigger:
                # we should get a window!
                assert len(return_list) == 1
                window_message = return_list[-1][1]
                assert len(window_message.sample) == window_length
                n_triggered += 1
            else:
                assert len(return_list) == 0
    assert n_triggered == 2


def test_stimulus_windowing_basic() -> None:
    """
    Assume a trial of 70 seconds, stimulus onset at
    10 and 40 seconds.
    We want to trigger for a window of 25 seconds post-stimulus.
    The trigger should return windows twice.

    Note the scenario above where stimulus happens multiple times
    within the same trial is not yet possible with current TrialSpec,
    but WindowTriggerNodes don't care!
    """
    expected_sample_count = int(25 * SAMPLE_RATE_HZ)
    timestamps = np.arange(0, 70, DT) + DT
    samples_since_stimulus_onset = (
        [None] * np.sum(timestamps < 10)
        + list(range(np.sum((timestamps >= 10) & (timestamps < 40))))
        + list(range(np.sum(timestamps >= 40)))
    )
    array_signal_view_messages = [
        ArraySignalViewMessage(
            block_number=0,
            trial_number=1,
            timestamp=timestamps[i],
            samples_since_stimulus_onset=samples_since_stimulus_onset[i],
            sample=np.random.randn(100),
        )
        for i in range(len(timestamps))
    ]

    test_harness = df.NodeTestHarness(ArrayStimulusWindowTriggerNode)
    n_triggered = 0
    with test_harness.get_node() as node:
        assert node.state.window is None
        assert node.state.window_length is None
        assert node.state.samples_since_stimulus_onset is None
        # start triggering
        df.run_async(
            node.trigger_control,
            args=[
                TriggerMessage(
                    timestamp=DT,
                    trigger_command=TriggerCommand.START_TRIGGER,
                    window_length=expected_sample_count,
                    samples_since_stimulus_onset=expected_sample_count,
                )
            ],
        )
        assert node.state.window is not None
        assert node.state.window_length == expected_sample_count
        assert node.state.samples_since_stimulus_onset == expected_sample_count

        for input_message in array_signal_view_messages:
            return_list = df.run_async(node.run, args=[input_message])
            samples_since_onset = input_message.samples_since_stimulus_onset
            should_trigger = (
                samples_since_onset is not None
                and samples_since_onset % expected_sample_count == 0
                and samples_since_onset > 0
            )
            if should_trigger:
                # we should get a window!
                assert len(return_list) == 1
                window_message = return_list[-1][1]
                assert len(window_message.sample) == expected_sample_count
                n_triggered += 1
            else:
                assert len(return_list) == 0

        # stop triggering
        df.run_async(
            node.trigger_control,
            args=[
                TriggerMessage(
                    timestamp=timestamps[-1] + DT,
                    trigger_command=TriggerCommand.STOP_TRIGGER,
                )
            ],
        )
        assert node.state.window is None
        assert node.state.window_length is None
        assert node.state.samples_since_stimulus_onset is None

    assert n_triggered == 2


def test_stimulus_windowing_multiple() -> None:
    """
    Assume a trial of 75 seconds, stimulus onset at
    10 and 40 seconds.

    For trigger 1: we want to trigger for a window of 25 seconds
    post-stimulus, the trigger should return windows twice.

    For trigger 2: we want to trigger for window of 32 seconds
    post-stimulus, the trigger should return windows only once,
    after the second stimulus.
    """
    expected_sample_count1 = int(25 * SAMPLE_RATE_HZ)
    expected_sample_count2 = int(32 * SAMPLE_RATE_HZ)

    timestamps = np.arange(0, 75, DT) + DT
    samples_since_stimulus_onset = (
        [None] * np.sum(timestamps < 10)
        + list(range(np.sum((timestamps >= 10) & (timestamps < 40))))
        + list(range(np.sum(timestamps >= 40)))
    )
    array_signal_view_messages = [
        ArraySignalViewMessage(
            block_number=0,
            trial_number=1,
            timestamp=timestamps[i],
            samples_since_stimulus_onset=samples_since_stimulus_onset[i],
            sample=np.random.randn(100),
        )
        for i in range(len(timestamps))
    ]

    # node 1
    test_harness1 = df.NodeTestHarness(ArrayStimulusWindowTriggerNode)
    n_triggered1 = 0
    trigger_start_msg1 = TriggerMessage(
        timestamp=DT,
        trigger_command=TriggerCommand.START_TRIGGER,
        window_length=expected_sample_count1,
        samples_since_stimulus_onset=expected_sample_count1,
    )

    # node 2
    test_harness2 = df.NodeTestHarness(ArrayStimulusWindowTriggerNode)
    n_triggered2 = 0
    trigger_start_msg2 = TriggerMessage(
        timestamp=DT,
        trigger_command=TriggerCommand.START_TRIGGER,
        window_length=expected_sample_count2,
        samples_since_stimulus_onset=expected_sample_count2,
    )

    trigger_stop_msg = TriggerMessage(
        timestamp=timestamps[-1] + DT, trigger_command=TriggerCommand.STOP_TRIGGER
    )

    with test_harness1.get_node() as node1, test_harness2.get_node() as node2:

        for node in [node1, node2]:
            assert node.state.window is None
            assert node.state.window_length is None
            assert node.state.samples_since_stimulus_onset is None

        # start triggering
        df.run_async(node1.trigger_control, args=[trigger_start_msg1])
        assert node1.state.window is not None
        assert node1.state.window_length == expected_sample_count1
        assert node1.state.samples_since_stimulus_onset == expected_sample_count1
        df.run_async(node2.trigger_control, args=[trigger_start_msg2])
        assert node2.state.window is not None
        assert node2.state.window_length == expected_sample_count2
        assert node2.state.samples_since_stimulus_onset == expected_sample_count2

        for input_message in array_signal_view_messages:
            return_list1 = df.run_async(node1.run, args=[input_message])
            return_list2 = df.run_async(node2.run, args=[input_message])

            samples_since_onset = input_message.samples_since_stimulus_onset
            should_trigger1 = (
                samples_since_onset is not None
                and samples_since_onset % expected_sample_count1 == 0
                and samples_since_onset > 0
            )
            should_trigger2 = (
                samples_since_onset is not None
                and samples_since_onset % expected_sample_count2 == 0
                and samples_since_onset > 0
            )

            if should_trigger1:
                # we should get a window!
                assert len(return_list1) == 1
                window_message = return_list1[-1][1]
                assert len(window_message.sample) == expected_sample_count1
                n_triggered1 += 1
            else:
                assert len(return_list1) == 0

            if should_trigger2:
                assert len(return_list2) == 1
                window_message = return_list2[-1][1]
                assert len(window_message.sample) == expected_sample_count2
                n_triggered2 += 1
            else:
                assert len(return_list2) == 0

        # stop triggering
        for node in [node1, node2]:
            df.run_async(node.trigger_control, args=[trigger_stop_msg])
            assert node.state.window is None
            assert node.state.window_length is None
            assert node.state.samples_since_stimulus_onset is None

    assert n_triggered1 == 2
    assert n_triggered2 == 1
