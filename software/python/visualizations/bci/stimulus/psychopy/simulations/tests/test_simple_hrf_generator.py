#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List

import numpy as np
from labgraph.util.testing import get_test_filename
from matplotlib import pyplot

from ...constants import FingerTappingCondition
from ..event_boxcar_generator import EventBoxCarChannelConfig, EventBoxCarGenerator
from ..simple_hrf_generator import SimpleHRFChannelConfig, SimpleHRFGenerator


def _get_time(curr_iters: int, sample_freq: int) -> float:
    # TODO: Would need to match node's clock ticks
    return curr_iters * (1.0 / sample_freq)


def _plot_multiple(x: np.ndarray, y_s: List[np.ndarray], fname: str) -> None:
    """
    Plots all arrays in y_s to x.
    """

    for y in y_s:
        pyplot.plot(x, y)
    pyplot.savefig(fname)


def _get_next_samples(
    iters: int,
    generator: SimpleHRFGenerator,
    x: List[float],
    hrf_samples: np.ndarray,
    curr_iters: int,
    sample_freq: int,
) -> int:
    for _ in range(iters):
        next_sample = generator.next_sample()
        for index, sample in np.ndenumerate(next_sample):
            if hrf_samples[index] is None:
                hrf_samples[index] = []
            hrf_samples[index].append(sample)

        x.append(_get_time(curr_iters, sample_freq))
        curr_iters += 1

    return curr_iters


def _add_next_samples(
    iters: int, samples: List[np.ndarray], new_sample: np.ndarray
) -> int:
    for _ in range(iters):
        samples.append(new_sample)

    return iters


def test_mock_convolved_generation() -> None:
    # Build mock event activation signal output
    event_signal_output = []
    total_iters = 0
    total_iters += _add_next_samples(300, event_signal_output, [0, 0])
    # Left hand = 1
    total_iters += _add_next_samples(50, event_signal_output, [1, 0])
    total_iters += _add_next_samples(50, event_signal_output, [0, 0])
    # Left hand = 1
    total_iters += _add_next_samples(200, event_signal_output, [1, 0])
    total_iters += _add_next_samples(200, event_signal_output, [0, 0])
    # Right hand = 1
    total_iters += _add_next_samples(200, event_signal_output, [0, 1])
    total_iters += _add_next_samples(500, event_signal_output, [0, 0])

    class MockEventBoxCarGenerator(EventBoxCarGenerator):
        def __init__(self) -> None:
            super().__init__()
            self.index = 0
            self.mock_output = event_signal_output

        def next_sample(self) -> np.ndarray:
            sample = self.mock_output[self.index]
            self.index += 1
            return sample

    # Boxcar Generator setup
    BOXCAR_SHAPE = (2,)
    input_generator = MockEventBoxCarGenerator()
    ea_gen_config = EventBoxCarChannelConfig(
        BOXCAR_SHAPE,
        [FingerTappingCondition.HAND_LEFT, FingerTappingCondition.HAND_RIGHT],
    )
    input_generator.set_channel_config(ea_gen_config)

    # Construct random amplitudes array
    HRF_SHAPE = (2, 6)
    AMPLITUDES = np.random.rand(2, 3)
    SAMPLE_RATE = 10  # Hz
    config = SimpleHRFChannelConfig(HRF_SHAPE, AMPLITUDES, SAMPLE_RATE, input_generator)
    hrf_generator = SimpleHRFGenerator()
    hrf_generator.set_channel_config(config)

    # Setup helper values for storing samples
    x = []
    curr_iters = 0
    hrf_samples = np.empty(config.amplitudes.shape, dtype=object)

    # Fetch all samples
    curr_iters = _get_next_samples(
        total_iters, hrf_generator, x, hrf_samples, curr_iters, SAMPLE_RATE
    )

    # Pad stimulus boxcar signals for graphing
    padding = [0 for _ in range(len(x) - len(event_signal_output))]
    e1 = [signal[0] for signal in event_signal_output]
    e2 = [signal[1] for signal in event_signal_output]

    # Specify number of rows and columns in output graph
    n_rows = 2
    n_columns = 2

    # Plot boxcar signals
    pyplot.subplot(n_rows, n_columns, 1)
    pyplot.plot(x, e1 + padding)
    pyplot.plot(x, e2 + padding)
    pyplot.title("Stimulus Boxcars")

    sample_index = 0
    # Plot convolved HRF signals
    for plot_index in range(2, n_rows * n_columns + 1):
        pyplot.subplot(n_rows, n_columns, plot_index)
        pyplot.plot(x, hrf_samples[0][sample_index])
        pyplot.plot(x, hrf_samples[1][sample_index])
        pyplot.title(f"HRF channel {sample_index}")
        sample_index += 1

    # Save figure
    pyplot.savefig(get_test_filename("png"))
