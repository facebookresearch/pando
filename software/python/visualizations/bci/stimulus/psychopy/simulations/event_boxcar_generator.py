#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass
from typing import List, Optional

import numpy as np
from labgraph.graphs import Node, State, Topic, subscriber
from labgraph.simulations import (
    FunctionChannelConfig,
    FunctionGenerator,
)

from ..constants import FingerTappingCondition, TrialMode
from ..messages import TrialModeMessage


class EventBoxCarGeneratorState(State):
    condition: Optional[FingerTappingCondition] = None
    trial_mode: Optional[TrialMode] = None


@dataclass
class EventBoxCarChannelConfig(FunctionChannelConfig):
    """
    Configuration object describing this generator's data generation.

    Args:
        condition_channels: The conditions that this generator publishes signals for.
    """

    condition_channels: List[FingerTappingCondition]


class EventBoxCarGenerator(FunctionGenerator, Node):
    """
    Outputs samples of 1/0 to indicate activation on each condition's channel,
    defined in this case to be when trial_mode=PERFORMANCE.
    """

    TRIAL_MODE_TOPIC = Topic(TrialModeMessage)

    # Function Generator Methods

    def set_channel_config(self, channel_config: EventBoxCarChannelConfig) -> None:
        super().set_channel_config(channel_config)
        self._setup_distribution()
        self._state = EventBoxCarGeneratorState()

    def _setup_distribution(self) -> None:
        total_channels = len(self.channel_config.condition_channels)
        self._baseline_sample = np.full(total_channels, 0.0)

        # Construct sample for each condition
        self._condition_samples = {}
        for channel_index, condition in enumerate(
            self.channel_config.condition_channels
        ):
            sample = np.full(total_channels, 0)
            sample[channel_index] = 1.0
            self._condition_samples[condition] = sample

    def next_sample(self) -> np.ndarray:
        if self._state.trial_mode == TrialMode.PERFORMANCE:
            current_condition = self._state.condition

            # Check to see if condition valid
            assert (
                current_condition in self.channel_config.condition_channels
            ), f"Condition {current_condition} not specified in generator config."
            return self._condition_samples[current_condition]
        else:
            return self._baseline_sample

    # Node Methods

    @subscriber(TRIAL_MODE_TOPIC)
    def trial_mode_sink(self, message: TrialModeMessage) -> None:
        self._state.trial_mode = message.trial_mode
        self._state.condition = message.condition
