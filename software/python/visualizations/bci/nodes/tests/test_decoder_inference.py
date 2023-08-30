#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import numpy as np
from bci.decoding.tests.mock import (
    N_CHANNELS,
    SAMPLE_RATE,
    train_and_save_mock_data,
)
from bci.messages.generic_signal_sample import SignalSampleMessage
from bci.nodes.decoder_inference import (
    DecoderConfig,
    DecoderInferenceNode,
    ArrayStimulusWindowMessage,
)
from bci.nodes.signal_view_joiner import ArraySignalViewMessage


PLACEHOLDER_WINDOW_LEN = 150
EPOCH_RANGE = (0.0, PLACEHOLDER_WINDOW_LEN / SAMPLE_RATE)


def test_inference_basic() -> None:
    with train_and_save_mock_data(EPOCH_RANGE) as model_path:
        config = DecoderConfig(model_path=model_path)
        message = ArrayStimulusWindowMessage(
            100.0,  # timestamp
            # window
            [
                ArraySignalViewMessage(
                    block_number=0,
                    trial_number=10,
                    sample_timestamp=80.0 + i / SAMPLE_RATE,
                    timestamp=80.0 + i / SAMPLE_RATE + 0.01,
                    samples_since_stimulus_onset=i,
                    sample=np.random.randn(N_CHANNELS),
                )
                for i in range(PLACEHOLDER_WINDOW_LEN)
            ],
        )

        test_harness = df.NodeTestHarness(DecoderInferenceNode)
        with test_harness.get_node(config=config) as node:
            return_msg = df.run_async(node.infer, args=[message])[0][1]
            assert return_msg.block_number == 0
            assert return_msg.trial_number == 10
            assert len(return_msg.conditions) == 3
            assert len(return_msg.probabilities) == 3
