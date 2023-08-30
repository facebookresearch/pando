#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import numpy as np
from bci.messages.generic_signal_sample import SignalSampleMessage
from bci.nodes.signal_view_joiner import SIGNAL_VIEW_JOINERS
from bci.stimulus.psychopy.constants import (
    TrialMode,
    ViewChangeType,
)
from bci.stimulus.psychopy.messages import ViewChangeMessage


def test_signal_view_joiner_basic() -> None:
    # Use SignalSampleMessage with sample_timestamp because in testing,
    # time.time() that's used to stamp SignalSampleMessage
    # is not consistent with the mock timestamps
    filtered_signal_messages = [
        SignalSampleMessage(
            timestamp=float(i),  # This doesn't matter here
            sample_timestamp=100.0 + i * 2.0,
            sample=np.random.randn(100).astype(np.float64),
        )
        for i in range(4)
    ]

    view_change1 = ViewChangeMessage(
        103.0,  # timestamp
        0,  # block
        TrialMode.PRE_STIMULUS,
        1,  # trial
        2,  # condition
        103.2,  # rendered at
        ViewChangeType.STIMULUS,
    )
    view_change2 = ViewChangeMessage(
        105.0,  # timestamp
        0,  # block
        TrialMode.PERFORMANCE,
        1,  # trial
        2,  # condition
        105.2,  # rendered at
        ViewChangeType.STIMULUS,
    )
    _, _, signal_view_joiner_node = SIGNAL_VIEW_JOINERS[SignalSampleMessage]
    test_harness = df.NodeTestHarness(signal_view_joiner_node)
    with test_harness.get_node() as node:
        joined_msg = df.run_async(node.join_signal, args=[filtered_signal_messages[0]])
        assert len(joined_msg) == 1
        assert joined_msg[0][1].samples_since_stimulus_onset is None
        assert node.state.block_number is None
        assert node.state.trial_number is None

        # View change not associated with stimulus onset
        df.run_async(node.view_change, args=[view_change1])
        assert node.state.block_number is None
        assert node.state.trial_number is None
        joined_msg2 = df.run_async(node.join_signal, args=[filtered_signal_messages[1]])
        assert len(joined_msg2) == 1
        assert joined_msg[0][1].samples_since_stimulus_onset is None

        # View change associated with stimulus onset!
        df.run_async(node.view_change, args=[view_change2])
        # Now when we pass signals in, we will get SignalViewMessages
        # with message fields that are all positive, and non-null
        joined_msg3 = df.run_async(
            node.join_signal, args=[filtered_signal_messages[2]]
        )[0][1]

        assert joined_msg3.samples_since_stimulus_onset == 1
        assert joined_msg3.sample_timestamp == 104.0
        assert len(joined_msg3.sample) == 100

        joined_msg4 = df.run_async(
            node.join_signal, args=[filtered_signal_messages[3]]
        )[0][1]

        assert joined_msg4.samples_since_stimulus_onset == 2
        assert joined_msg4.sample_timestamp == 106.0
        assert len(joined_msg4.sample) == 100
