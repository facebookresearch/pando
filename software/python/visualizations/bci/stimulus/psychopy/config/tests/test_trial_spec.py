#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.


from bci.stimulus.psychopy.config.trial_spec import TrialSpec
from bci.stimulus.psychopy.constants import VariableDurationFingerTappingCondition


trial1 = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    pre_stimulus_duration_range=(5.0, 5.0),
    performance_duration_range=(5.0, 5.0),
    post_stimulus_duration_range=(4.0, 4.0),
)

trial2 = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
    pre_stimulus_duration_range=(5.0, 5.0),
    performance_duration_range=(4.0, 5.0),
    post_stimulus_duration_range=(4.0, 4.0),
)

trial3 = TrialSpec(
    condition=VariableDurationFingerTappingCondition.NULL,
    pre_stimulus_duration_range=(5.0, 5.0),
    performance_duration_range=(2.0, 2.0),
    post_stimulus_duration_range=(4.0, 4.0),
)


def test_trial_spec_duration() -> None:
    assert trial1.max_duration == 14.0
    assert trial2.max_duration == 14.0
    assert trial3.max_duration == 11.0


def test_condition_type_name() -> None:
    assert trial1.condition_type_name == VariableDurationFingerTappingCondition.__name__
