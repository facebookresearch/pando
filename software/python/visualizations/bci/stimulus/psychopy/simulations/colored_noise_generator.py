#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass

import numpy as np
from labgraph.simulations import (
    FunctionChannelConfig,
    FunctionGenerator,
)

from .colored_noise import colored_noise


@dataclass
class ColoredNoiseChannelConfig(FunctionChannelConfig):
    exponent: float
    sample_rate: float  # Hz
    duration: float = 300  # s; repeating signal
    fmin: float = 0
    seed: int = 0  # np.random.normal used by colored_noise


class ColoredNoiseGenerator(FunctionGenerator):
    """
    A generator for producing colored noise.  Currently takes in
    specification for total number of channels and generates the
    same noise on every channel.
    """

    def set_channel_config(self, channel_config: ColoredNoiseChannelConfig) -> None:
        super().set_channel_config(channel_config)
        self._setup_distribution()

    def _setup_distribution(self) -> None:
        self._signal_index = 0
        self._signal_length = (
            self.channel_config.sample_rate * self.channel_config.duration
        )
        self._signal = colored_noise(
            self.channel_config.exponent,
            self.channel_config.duration,
            self.channel_config.sample_rate,
            self.channel_config.fmin,
            self.channel_config.seed,
        )

    def next_sample(self):
        next_value = self._signal[self._signal_index]
        self._signal_index = (self._signal_index + 1) % self._signal_length
        sample = np.full(self.channel_config.shape[0], next_value)
        return sample
