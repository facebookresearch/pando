#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import numpy as np
import pytest
from labgraph.util.error import LabgraphError

from ...constants import FingerTappingCondition, TrialMode
from ..square_response_generator import (
    SquareResponseGenerator,
    SquareResponseGeneratorConfig,
    SquareResponseGeneratorState,
)


def test_validate_config() -> None:
    total_channels = 2
    channel_to_value = {0: 0, 1: 1}

    # Overlapping assigned channels
    condition_to_channel_indicies = {
        FingerTappingCondition.HAND_LEFT: [0, 1],
        FingerTappingCondition.HAND_RIGHT: [1],  # Overlap at index 1
    }
    config = SquareResponseGeneratorConfig(
        total_channels, condition_to_channel_indicies, channel_to_value
    )
    with pytest.raises(LabgraphError):  # Should throw
        SquareResponseGenerator(config)

    # Invalid assigned channel indicies (negative)
    condition_to_channel_indicies = {
        FingerTappingCondition.HAND_LEFT: [0, -1]  # Negative value
    }
    config = SquareResponseGeneratorConfig(
        total_channels, condition_to_channel_indicies, channel_to_value
    )
    with pytest.raises(LabgraphError):  # Should throw
        SquareResponseGenerator(config)

    # Invalid assigned channel indicies (too big)
    condition_to_channel_indicies = {
        FingerTappingCondition.HAND_LEFT: [total_channels, 0]  # Index too large
    }
    config = SquareResponseGeneratorConfig(
        total_channels, condition_to_channel_indicies, channel_to_value
    )
    with pytest.raises(LabgraphError):  # Should throw
        SquareResponseGenerator(config)

    # Condition channel lacks activated value
    channel_to_value = {0: 0}  # Missing index 1
    condition_to_channel_indicies = {FingerTappingCondition.HAND_LEFT: [0, 1]}
    config = SquareResponseGeneratorConfig(
        total_channels, condition_to_channel_indicies, channel_to_value
    )
    with pytest.raises(LabgraphError):  # Should throw
        SquareResponseGenerator(config)

    # Valid configuration
    channel_to_value = {0: 0, 1: 1}
    config = SquareResponseGeneratorConfig(
        total_channels, condition_to_channel_indicies, channel_to_value
    )
    SquareResponseGenerator(config)  # Shouldn't throw


def test_generate_sample_invalid_condition() -> None:
    TOTAL_CHANNELS = 1
    DEFAULT_VALUE = 1
    CONDITION_TO_CHANNEL_INDICIES = {FingerTappingCondition.NULL: [0]}
    CHANNEL_TO_VALUE = {0: DEFAULT_VALUE}

    config = SquareResponseGeneratorConfig(
        TOTAL_CHANNELS, CONDITION_TO_CHANNEL_INDICIES, CHANNEL_TO_VALUE, DEFAULT_VALUE
    )
    generator = SquareResponseGenerator(config)  # Should not throw
    state = SquareResponseGeneratorState(
        FingerTappingCondition.HAND_LEFT, TrialMode.PERFORMANCE  # HAND_LEFT invalid
    )

    with pytest.raises(LabgraphError):  # Should throw due to invalid condition
        generator.next_sample(state)


def test_generate_samples() -> None:
    TOTAL_CHANNELS = 10
    DEFAULT_VALUE = 10
    CONDITION_TO_CHANNEL_INDICIES = {
        FingerTappingCondition.NULL: [0, 9],
        FingerTappingCondition.HAND_LEFT: [1, 8],
        FingerTappingCondition.HAND_RIGHT: [2, 7],
    }
    CHANNEL_TO_VALUE = {0: DEFAULT_VALUE, 1: 1, 2: 2, 7: 7, 8: 8, 9: DEFAULT_VALUE}

    config = SquareResponseGeneratorConfig(
        TOTAL_CHANNELS, CONDITION_TO_CHANNEL_INDICIES, CHANNEL_TO_VALUE, DEFAULT_VALUE
    )
    generator = SquareResponseGenerator(config)  # Should not throw
    state = SquareResponseGeneratorState(
        FingerTappingCondition.HAND_LEFT, TrialMode.PRE_STIMULUS
    )

    # LEFT, trial_mode = FIXATION_START
    baseline_sample = np.full(TOTAL_CHANNELS, DEFAULT_VALUE)
    next_sample = generator.next_sample(state)
    assert np.array_equal(next_sample, baseline_sample)

    # LEFT, trial_mode = PERFORMANCE
    state.trial_mode = TrialMode.PERFORMANCE
    next_sample = generator.next_sample(state)
    expected_sample = np.full(TOTAL_CHANNELS, DEFAULT_VALUE)
    activated_indicies = CONDITION_TO_CHANNEL_INDICIES[state.condition]
    for i in activated_indicies:
        expected_sample[i] = CHANNEL_TO_VALUE[i]
    assert np.array_equal(next_sample, expected_sample)

    # RIGHT, trial_mode = PERFORMANCE
    state.condition = FingerTappingCondition.HAND_RIGHT
    next_sample = generator.next_sample(state)
    expected_sample = np.full(TOTAL_CHANNELS, DEFAULT_VALUE)
    activated_indicies = CONDITION_TO_CHANNEL_INDICIES[state.condition]
    for i in activated_indicies:
        expected_sample[i] = CHANNEL_TO_VALUE[i]
    assert np.array_equal(next_sample, expected_sample)
