#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Dict, Set

from bci.stimulus.psychopy.constants import (
    FingerHapticStimulationCondition,
    FingerTapEvent,
    VariableDurationFingerTappingCondition,
)

# Finger and digit tapping protocol feedback parameters
LEFT_FINGERS = {
    finger_tap for finger_tap in FingerTapEvent if "LEFT" in finger_tap.name
}

RIGHT_FINGERS = {
    finger_tap for finger_tap in FingerTapEvent if "RIGHT" in finger_tap.name
}

VALID_FINGERS_BY_CONDITION: Dict[
    VariableDurationFingerTappingCondition, Set[FingerTapEvent]
] = {
    VariableDurationFingerTappingCondition.HAND_LEFT_SHORT: LEFT_FINGERS,
    VariableDurationFingerTappingCondition.HAND_LEFT_MID: LEFT_FINGERS,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG: LEFT_FINGERS,
    VariableDurationFingerTappingCondition.HAND_RIGHT_SHORT: RIGHT_FINGERS,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID: RIGHT_FINGERS,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG: RIGHT_FINGERS,
}

VALID_DIGIT_BY_CONDITION: Dict[
    FingerHapticStimulationCondition, Set[FingerTapEvent]
] = {
    FingerHapticStimulationCondition.LEFT_PINKY: {FingerTapEvent.LEFT_PINKY},
    FingerHapticStimulationCondition.LEFT_MIDDLE: {FingerTapEvent.LEFT_MIDDLE},
    FingerHapticStimulationCondition.LEFT_INDEX: {FingerTapEvent.LEFT_INDEX},
    FingerHapticStimulationCondition.LEFT_THUMB: {FingerTapEvent.LEFT_THUMB},
    FingerHapticStimulationCondition.RIGHT_PINKY: {FingerTapEvent.RIGHT_PINKY},
    FingerHapticStimulationCondition.RIGHT_MIDDLE: {FingerTapEvent.RIGHT_MIDDLE},
    FingerHapticStimulationCondition.RIGHT_INDEX: {FingerTapEvent.RIGHT_INDEX},
    FingerHapticStimulationCondition.RIGHT_THUMB: {FingerTapEvent.RIGHT_THUMB},
}
