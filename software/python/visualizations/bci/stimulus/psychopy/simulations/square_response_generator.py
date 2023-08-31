#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass
from typing import Dict, List, Optional

import numpy as np
from bci.stimulus.psychopy.constants import Condition, TrialMode
from labgraph.graphs import State
from labgraph.simulations import FunctionGenerator
from labgraph.util.error import LabgraphError


class SquareResponseGeneratorState(State):
    """
    State object describing relevant state variables for a SquareResponseGenerator.

    Args:
        condition: The current condition in the experiment.
        trial_mode: The current trial mode.
    """

    condition: Optional[Condition] = None
    trial_mode: Optional[TrialMode] = None


@dataclass
class SquareResponseGeneratorConfig:
    """
    Configuration object describing a SquareResponseGenerator's data generation.

    Args:
        constant: The constant value to assign to all channels as the baseline.
        total_channels: The total number of channels in the signal.
        condition_to_channel_indices:
            A mapping of which channels are associated with each condition.
        channel_to_activated_value:
            A mapping of which constant value a channel has to indicate activation.
    """

    total_channels: int
    condition_to_channel_indicies: Dict[Condition, List[int]]
    channel_to_activated_value: Dict[int, float]
    constant: float = 1.0


class SquareResponseGenerator(FunctionGenerator):
    """
    Generates samples representing a constant-valued signal.  During
    trial_mode=PERFORMANCE, channels associated with the current
    condition will change to other constant values that are randomly
    determined on initialization.

    Args:
        config: The configuration object for this generator.
    """

    def __init__(self, config: SquareResponseGeneratorConfig) -> None:
        super().__init__()
        self.config = config
        self._validate_config()
        self._setup_distribution()

    def _validate_config(self) -> None:
        # Assigned channels shouldn't overlap
        used_channel_indicies: List[int] = []

        # All assigned channels should be a valid index
        for condition, indicies in self.config.condition_to_channel_indicies.items():
            for index in indicies:
                if index < 0 or index > self.config.total_channels:
                    raise LabgraphError(
                        f"Condition {condition} is assigned to index {index},"
                        "which is an invalid channel index."
                    )
                if index in used_channel_indicies:
                    raise LabgraphError(f"Channel index {index} was assigned to twice.")

                used_channel_indicies.append(index)

        # All assigned channels should have an activation value
        for channel_index in used_channel_indicies:
            if channel_index not in self.config.channel_to_activated_value.keys():
                raise LabgraphError(
                    f"Activated value for channel index {channel_index} not specified."
                )

    def _setup_distribution(self) -> None:
        # Construct baseline sample using configuration constant
        self._baseline_sample = np.full(
            self.config.total_channels, self.config.constant
        )

        # Construct sample for each condition
        self._condition_samples = {}
        for condition, indicies in self.config.condition_to_channel_indicies.items():
            sample = np.full(self.config.total_channels, self.config.constant)

            # Set values on corresponding channels to their configured
            # activated value.
            for channel_index in indicies:
                sample[channel_index] = self.config.channel_to_activated_value[
                    channel_index
                ]

            # Set condition sample
            self._condition_samples[condition] = sample

    def next_sample(self, state: SquareResponseGeneratorState) -> np.ndarray:
        if state.trial_mode == TrialMode.PERFORMANCE:
            current_condition = state.condition

            # Check to see if condition valid
            if current_condition not in self._condition_samples.keys():
                raise LabgraphError(
                    f"Condition {current_condition} not specified in generator config."
                )
            return self._condition_samples[current_condition]
        else:
            return self._baseline_sample
