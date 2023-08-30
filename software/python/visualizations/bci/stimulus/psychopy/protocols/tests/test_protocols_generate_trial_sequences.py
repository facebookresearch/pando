#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List

from bci.stimulus.psychopy.config import TrialSpec
from bci.stimulus.psychopy.constants import (
    Condition,
    FingerHapticStimulationCondition,
    VariableDurationFingerTappingCondition,
)
from bci.stimulus.psychopy.protocols.digit_tapping_fixed_duration import (
    generate_experiment_trial_sequence as generate_charizard,
    TRIAL_CONDITIONS as charizard_trial_conditions,
    TRIALS_PER_CONDITION as charizard_trials_per_condition,
    REST_BLOCK_DURATION as charizard_rest_block_duration,
    INITIAL_REST_DURATION as charizard_initial_rest_duration,
)
from bci.stimulus.psychopy.protocols.finger_tapping_two_durations import (
    generate_experiment_trial_sequence as generate_charmeleon,
    TRIALS_PER_CONDITION as charmeleon_trials_per_condition,
    REST_BLOCK_DURATION as charmeleon_rest_block_duration,
    INITIAL_REST_DURATION as charmeleon_initial_rest_duration,
)


def check_performance_block(
    trial_spec_list: List[List[TrialSpec]],
    trial_conditions: List[Condition],
    trials_per_condition: int,
    forbidden_consecutive: List[Condition],
) -> None:
    """
    Check the generated trial_spec_list is acceptable according
    to criteria.

    Inputs:
    - trial_spec_list: List of TrialSpec for each block
    - trial_conditions: The possible trial conditions
    - trials_per_conditions: Number of trials per each trial condition in each block.
        Assume this number is the same for all trials.
    - forbidden_consecutive: List[Condition]. Conditions for which consecutive trials
        are not allowed.
    """
    condition_table = dict.fromkeys(trial_conditions, None)
    for block in trial_spec_list:
        condition_table.update({}.fromkeys(condition_table, 0))
        last_trial = None
        for trial_spec in block:
            if (
                last_trial is not None
                and last_trial.condition == trial_spec.condition
                and last_trial.condition in forbidden_consecutive
            ):
                raise RuntimeError(f"Found consecutive {trial_spec.condition} trials")
            condition_table[trial_spec.condition] += 1
            last_trial = trial_spec
        # Make sure we have each condition the required number of times
        assert all(
            n_trials == trials_per_condition
            for n_trials in list(condition_table.values())
        )


# =============== Charmeleon ===============

charmeleon_trial_conditions = [
    VariableDurationFingerTappingCondition.HAND_LEFT_MID,
    VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    VariableDurationFingerTappingCondition.HAND_RIGHT_MID,
    VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
    VariableDurationFingerTappingCondition.NULL,
]
charmeleon_forbidden_consecutive = [VariableDurationFingerTappingCondition.NULL]


def test_charmeleon_with_rest() -> None:
    n_performance_blocks = 6
    trial_spec_list = generate_charmeleon(n_performance_blocks, rest_block=True)
    assert len(trial_spec_list) == n_performance_blocks + 1
    # First block is rest only
    assert len(trial_spec_list[0]) == 1
    assert (
        trial_spec_list[0][0].condition == VariableDurationFingerTappingCondition.NULL
    )
    # 3 minutes minus 20s initial rest duration
    assert trial_spec_list[0][0].performance_duration_range[0] == (
        charmeleon_rest_block_duration - charmeleon_initial_rest_duration
    )

    check_performance_block(
        trial_spec_list[1:],
        charmeleon_trial_conditions,
        charmeleon_trials_per_condition,
        charmeleon_forbidden_consecutive,
    )


def test_charmeleon_with_no_rest() -> None:
    n_performance_blocks = 3
    trial_spec_list = generate_charmeleon(n_performance_blocks, rest_block=False)
    assert len(trial_spec_list) == n_performance_blocks
    check_performance_block(
        trial_spec_list,
        charmeleon_trial_conditions,
        charmeleon_trials_per_condition,
        charmeleon_forbidden_consecutive,
    )


# =============== Charizard ===============

charizard_forbidden_consecutive = [FingerHapticStimulationCondition.NULL]


def test_charizard_with_rest() -> None:
    n_performance_blocks = 6
    trial_spec_list = generate_charizard(n_performance_blocks, rest_block=True)
    assert len(trial_spec_list) == n_performance_blocks + 1
    # First block is rest only
    assert len(trial_spec_list[0]) == 1
    assert trial_spec_list[0][0].condition == FingerHapticStimulationCondition.NULL
    # 3 minutes minus 20s initial rest duration
    assert trial_spec_list[0][0].performance_duration_range[0] == (
        charizard_rest_block_duration - charizard_initial_rest_duration
    )

    check_performance_block(
        trial_spec_list[1:],
        charizard_trial_conditions,
        charizard_trials_per_condition,
        charizard_forbidden_consecutive,
    )


def test_charizard_with_no_rest() -> None:
    n_performance_blocks = 3
    trial_spec_list = generate_charizard(n_performance_blocks, rest_block=False)
    assert len(trial_spec_list) == n_performance_blocks
    check_performance_block(
        trial_spec_list,
        charizard_trial_conditions,
        charizard_trials_per_condition,
        charizard_forbidden_consecutive,
    )
