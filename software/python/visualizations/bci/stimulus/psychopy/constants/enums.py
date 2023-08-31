#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from enum import Enum
from typing import Dict, Set

from bci.constants.laterality import ActivationSide


# TODO: Find a way to add UNDEFINED to an enum with subclassing support.
class Condition(int, Enum):
    """
    Represents a condition that can be displayed to the user; It's left to a subclass
    to define what the conditions in this enum actually are.
    """

    pass


class SuitCondition(Condition):
    UNDEFINED = -1
    SPADE = 0
    HEART = 1
    NULL = 2


class FingerTappingCondition(Condition):
    UNDEFINED = -1
    HAND_LEFT = 0
    HAND_RIGHT = 1
    NULL = 2


class FingerHapticStimulationCondition(Condition):
    UNDEFINED = -1
    LEFT_THUMB = 0
    LEFT_INDEX = 1
    LEFT_MIDDLE = 2
    LEFT_PINKY = 3
    RIGHT_THUMB = 4
    RIGHT_INDEX = 5
    RIGHT_MIDDLE = 6
    RIGHT_PINKY = 7
    NULL = 8


class MotorMappingCondition(Condition):
    UNDEFINED = -1
    HAND_LEFT = 0
    HAND_RIGHT = 1
    FOOT_LEFT = 2
    FOOT_RIGHT = 3
    TONGUE = 4
    NULL = 5


class VariableDurationFingerTappingCondition(Condition):
    UNDEFINED = -1
    HAND_LEFT_SHORT = 0
    HAND_LEFT_MID = 1
    HAND_LEFT_LONG = 2
    HAND_RIGHT_SHORT = 3
    HAND_RIGHT_MID = 4
    HAND_RIGHT_LONG = 5
    NULL = 6


class CuffOcclusionCondition(Condition):
    UNDEFINED = -1
    CUFF = 0


class BreathHoldingCondition(Condition):
    UNDEFINED = -1
    DEEP_BREATHS = 0
    HOLD_BREATH = 1


class TrialMode(int, Enum):
    """
    Describes what mode of the trial the experiment is in, i.e.,
    the state of the experiment.
    """

    UNDEFINED = -1
    PRE_STIMULUS = 0
    PERFORMANCE = 1
    POST_STIMULUS = 2
    BLOCK_DONE = 3


class Instructions(int, Enum):
    UNDEFINED = -1
    PROTOCOL = 0
    KEY_MAPPING = 1


class FingerTapEvent(int, Enum):
    UNDEFINED = -1
    LEFT_PINKY = 0
    LEFT_RING = 1
    LEFT_MIDDLE = 2
    LEFT_INDEX = 3
    LEFT_THUMB = 4
    RIGHT_THUMB = 5
    RIGHT_INDEX = 6
    RIGHT_MIDDLE = 7
    RIGHT_RING = 8
    RIGHT_PINKY = 9


class BehavioralFeedbackStatus(int, Enum):
    RESET = -1
    INCORRECT = 0
    CORRECT = 1


class DecoderFeedbackStatus(int, Enum):
    OFF = 0
    ON = 1


class ViewChangeType(int, Enum):
    UNDEFINED = -1
    BEHAVIORAL_FEEDBACK = 0
    STIMULUS = 1
    DECODER_FEEDBACK = 2


class HeadMotionFingerTapping(Condition):
    UNDEFINED = -1
    LEFT_HAND_PITCH = 0
    LEFT_HAND_ROLL = 1
    LEFT_HAND_YAW = 2
    RIGHT_HAND_PITCH = 3
    RIGHT_HAND_ROLL = 4
    RIGHT_HAND_YAW = 5
    LEFT_HAND = 6
    RIGHT_HAND = 7
    REST = 8


class HeadMotionRest(Condition):
    UNDEFINED = -1
    PITCH_UP = 0
    PITCH_DOWN = 1
    ROLL_LEFT = 2
    ROLL_RIGHT = 3
    YAW_LEFT = 4
    YAW_RIGHT = 5
    REST = 6


StimulusActivationsType = Dict[Condition, ActivationSide]
StimulusGroupsType = Dict[Condition, int]
CorrectConditionEvents = Dict[Condition, Set[FingerTapEvent]]
