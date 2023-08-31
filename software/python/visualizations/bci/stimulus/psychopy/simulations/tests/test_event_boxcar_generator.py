#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from ...constants import FingerTappingCondition, TrialMode
from ..event_boxcar_generator import (
    EventBoxCarChannelConfig,
    EventBoxCarGenerator,
    EventBoxCarGeneratorState,
)


def test_event_boxcar_generation() -> None:
    """
    Tests that the output of the generator responds as expected
    to state changes.
    """

    SHAPE = (2,)
    LEFT_HAND_CHANNEL = 0
    RIGHT_HAND_CHANNEL = 1
    CHANNELS = [FingerTappingCondition.HAND_LEFT, FingerTappingCondition.HAND_RIGHT]

    assert CHANNELS[LEFT_HAND_CHANNEL] == FingerTappingCondition.HAND_LEFT
    assert CHANNELS[RIGHT_HAND_CHANNEL] == FingerTappingCondition.HAND_RIGHT

    config = EventBoxCarChannelConfig(SHAPE, CHANNELS)
    generator = EventBoxCarGenerator()
    generator.set_channel_config(config)

    # default state
    state = EventBoxCarGeneratorState()
    generator._state = state

    next_sample = generator.next_sample()
    assert next_sample[LEFT_HAND_CHANNEL] == 0
    assert next_sample[RIGHT_HAND_CHANNEL] == 0

    # trial_mode=PERFORMANCE
    state.condition = FingerTappingCondition.HAND_RIGHT
    state.trial_mode = TrialMode.PERFORMANCE
    next_sample = generator.next_sample()
    assert next_sample[LEFT_HAND_CHANNEL] == 0
    assert next_sample[RIGHT_HAND_CHANNEL] == 1
