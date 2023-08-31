#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

__all__ = [
    "FingerHapticStimulationCondition",
    "FingerTapEvent",
    "FingerTappingCondition",
    "BreathHoldingCondition",
    "Condition",
    "CorrectConditionEvents",
    "CuffOcclusionCondition",
    "DecoderFeedbackStatus",
    "HeadMotionRest",
    "HeadMotionFingerTapping",
    "Instructions",
    "MINIMUM_SLEEP_TIME",
    "MotorMappingCondition",
    "StimulusActivationsType",
    "StimulusGroupsType",
    "SuitCondition",
    "TrialMode",
    "VariableDurationFingerTappingCondition",
    "ViewChangeType",
    "BehavioralFeedbackStatus",
    "BAD_BEHAVIOR_RESET_TIME",
    "BEHAVIOR_BUFFER_TIME",
]

from .enums import (
    BehavioralFeedbackStatus,
    BreathHoldingCondition,
    Condition,
    CorrectConditionEvents,
    CuffOcclusionCondition,
    DecoderFeedbackStatus,
    FingerHapticStimulationCondition,
    FingerTapEvent,
    FingerTappingCondition,
    HeadMotionFingerTapping,
    HeadMotionRest,
    Instructions,
    MotorMappingCondition,
    StimulusActivationsType,
    StimulusGroupsType,
    SuitCondition,
    TrialMode,
    VariableDurationFingerTappingCondition,
    ViewChangeType,
)


MINIMUM_SLEEP_TIME = 0.05
# remove the "bad behavior" stimuli after this amount of seconds
BAD_BEHAVIOR_RESET_TIME = 0.3
# allow participant this amount of leeway when the cue changes
BEHAVIOR_BUFFER_TIME = 0.4
