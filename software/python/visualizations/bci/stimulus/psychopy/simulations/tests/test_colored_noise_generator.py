#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from ..colored_noise import colored_noise
from ..colored_noise_generator import ColoredNoiseChannelConfig, ColoredNoiseGenerator


def test_colored_noise_generation() -> None:
    """
    Tests that the values produced is the same on every channel, and
    matches the values resulting from a direct call to the
    generating function.
    """

    EXPONENT = 1
    SAMPLE_RATE = 5

    shape = (2,)
    channel_config = ColoredNoiseChannelConfig(shape, EXPONENT, SAMPLE_RATE)
    generator = ColoredNoiseGenerator()
    generator.set_channel_config(channel_config)

    expected_signal = colored_noise(
        channel_config.exponent,
        channel_config.duration,
        channel_config.sample_rate,
        channel_config.fmin,
    ).tolist()

    for i in range(channel_config.duration * channel_config.sample_rate):
        next_sample = generator.next_sample()
        assert len(next_sample) == 2
        assert expected_signal[i] == next_sample[0] == next_sample[1]

    # Next sample "restarts" from beginning of expected signal
    next_sample = generator.next_sample()
    assert expected_signal[0] == next_sample[0] == next_sample[1]
