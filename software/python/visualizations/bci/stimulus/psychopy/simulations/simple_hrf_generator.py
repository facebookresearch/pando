#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass, field
from typing import List

import numpy as np
from labgraph.simulations import (
    FunctionChannelConfig,
    FunctionGenerator,
)

from .event_boxcar_generator import EventBoxCarGenerator
from .hemodynamics import simulated_hemodynamics


@dataclass
class SimpleHRFChannelConfig(FunctionChannelConfig):
    """
    Configuration class for a SimpleHRFGenerator.
    """

    amplitudes: np.ndarray
    sample_rate: float  # Hz
    input_generator: EventBoxCarGenerator

    def __post_init__(self) -> None:

        # Validate amplitude configuration array shape
        amplitude_shape = self.amplitudes.shape

        # n_stimulus x n_channels (2 dimensions)
        assert len(amplitude_shape) == 2

        # n_stimulus should match number of output channels for event_boxcar_generator
        assert amplitude_shape[0] == self.input_generator.channel_config.shape[0]


@dataclass
class HRFProgress:
    hrf_index: int = 0
    value_history: List[float] = field(default_factory=list)


class Channel:
    """
    Intermediate class providing necessary information for
    calculating the value of the waveform at each time point
    for a single channel.
    """

    def __init__(self, hrf_values: np.ndarray) -> None:
        # May help specifying unique channel parameters
        self.hrf_values = hrf_values
        self.hrf_progressors: List[HRFProgress] = []

    def add_progressor(self) -> None:
        """
        Adds a new progressor to the channel.
        """

        new_progressor = HRFProgress()
        self.hrf_progressors.append(new_progressor)

    def next_convolved_value(
        self, hrf_progress: HRFProgress, stimulus_active: bool = False
    ) -> float:
        """
        Calculates and returns the value for one occurence of an
        HRF waveform on this channel, based on the values specified
        in `hrf_progress`.
        """

        last_value = 0
        if hrf_progress.value_history:
            last_value = hrf_progress.value_history[-1]

        hrf_value = 0
        if hrf_progress.hrf_index < len(self.hrf_values):
            hrf_value = self.hrf_values[hrf_progress.hrf_index]

        next_value = last_value + hrf_value
        if not stimulus_active:
            subtract_value = 0
            if hrf_progress.hrf_index - len(hrf_progress.value_history) < len(
                self.hrf_values
            ):
                subtract_value = self.hrf_values[
                    hrf_progress.hrf_index - len(hrf_progress.value_history)
                ]

            next_value -= subtract_value
            if len(hrf_progress.value_history) > 0:
                # Oldest value no longer necessary for tracking
                hrf_progress.value_history = hrf_progress.value_history[1:]

        hrf_progress.value_history.append(next_value)  # Save used value
        return next_value

    def get_next_value(self, stimulus_active: bool = False) -> float:
        """
        Calculates and returns value at this channel for the next sample
        by calculating the values for all active waveform instances on this
        channel and return the sum.
        """

        waveform_sum = 0
        for i, hrf_progress in enumerate(self.hrf_progressors):
            if i == len(self.hrf_progressors) - 1:
                # Only convolve if last waveform
                waveform_sum += self.next_convolved_value(hrf_progress, stimulus_active)
            else:
                waveform_sum += self.next_convolved_value(hrf_progress)

            # Increment progress
            hrf_progress.hrf_index += 1

        # Remove "completed" waveforms
        self.hrf_progressors[:] = [
            hrf_progress
            for hrf_progress in self.hrf_progressors
            if hrf_progress.hrf_index < len(self.hrf_values)
            or len(hrf_progress.value_history) > 0
        ]

        return waveform_sum


class SimpleHRFGenerator(FunctionGenerator):
    """
    Generator which produces a convolved HRF on each channel.  The samples produced by
    this generator are 2-dimensional, with dimensions n_stimulus x n_voxels.  The number
    of stimuli is the same as the number of stimulus boxcars generated by its input
    EventBoxCarGenerator, while the number of voxels is specified by the second
    dimension of this generator's `amplitudes` parameter in its configuration.

    As the sample returned is a simple numpy array, whether the representation is
    actually in the voxel space or measurement space shouldn't matter--it is up to the
    user to specify the length of the second dimension of this generator's output
    matrix.
    """

    HRF_LENGTH = 30  # seconds

    def set_channel_config(self, channel_config: SimpleHRFChannelConfig) -> None:
        super().set_channel_config(channel_config)
        self._setup_distribution()
        self._previous_activation_sample = np.full(self.channel_config.shape[0], 0.0)

    def _setup_distribution(self) -> None:
        # Initialize channel list, reference HRFs
        self.channels = np.empty(self.channel_config.amplitudes.shape, dtype=object)
        self.hrf_values = np.empty(self.channel_config.amplitudes.shape, dtype=object)

        # Initialize reference HRF (same currently applied to all channels)
        for index, amplitude in np.ndenumerate(self.channel_config.amplitudes):
            # Set HRF reference array for channel
            channel_hrf = simulated_hemodynamics(
                amplitude, self.HRF_LENGTH, self.channel_config.sample_rate
            )
            self.hrf_values[index] = channel_hrf

            # Initialize channel
            self.channels[index] = Channel(channel_hrf)

    def next_sample(self) -> np.ndarray:
        sample = np.full(self.channel_config.amplitudes.shape, 0.0)

        # Get upstream generated result
        activation_sample = self.channel_config.input_generator.next_sample()

        for index, channel in np.ndenumerate(self.channels):
            # len(index) == 2 due to post-init check in config
            stimulus_index = index[0]  # Stimulus index is first dimension
            is_activated = activation_sample[stimulus_index]

            # Add new progressor if initial encounter/activation on channel
            if is_activated and not self._previous_activation_sample[stimulus_index]:
                channel.add_progressor()

            next_value = channel.get_next_value(is_activated)
            sample[index] = next_value

        # Save last known activation sample
        self._previous_activation_sample = activation_sample
        return sample
