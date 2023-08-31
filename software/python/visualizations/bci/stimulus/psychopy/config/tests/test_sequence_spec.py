#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import random
from collections import defaultdict

import pytest
from bci.stimulus.psychopy.config import (
    FixedSequenceSpec,
    PermutableSequenceSpec,
    TrialSpec,
)
from bci.stimulus.psychopy.constants import VariableDurationFingerTappingCondition


# max_duration is 14.0
trial1 = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_LEFT_LONG,
    pre_stimulus_duration_range=(5.0, 5.0),
    performance_duration_range=(5.0, 5.0),
    post_stimulus_duration_range=(4.0, 4.0),
)

# max_duration is 14.0
trial2 = TrialSpec(
    condition=VariableDurationFingerTappingCondition.HAND_RIGHT_LONG,
    pre_stimulus_duration_range=(5.0, 5.0),
    performance_duration_range=(4.0, 5.0),
    post_stimulus_duration_range=(4.0, 4.0),
)

# max_duration is 11.0
trial3 = TrialSpec(
    condition=VariableDurationFingerTappingCondition.NULL,
    pre_stimulus_duration_range=(5.0, 5.0),
    performance_duration_range=(2.0, 2.0),
    post_stimulus_duration_range=(4.0, 4.0),
)

fixed_sequence_12 = FixedSequenceSpec(fixed_sequence=[trial1, trial2])
fixed_sequence_31 = FixedSequenceSpec(fixed_sequence=[trial3, trial1])


def test_fixed_sequence_spec() -> None:
    fixed_sequence = FixedSequenceSpec(fixed_sequence=[trial1, trial3])

    trial_13131 = fixed_sequence.get_sequence(5)
    assert trial_13131 == [trial1, trial3, trial1, trial3, trial1]


def test_permutable_sequence_spec_allow_duplicates() -> None:
    # asking for length of 2 without duplicates when
    # only 1 possible sequence is available should exception
    bad_sequence = PermutableSequenceSpec([fixed_sequence_12])
    with pytest.raises(ValueError):
        bad_sequence.get_sequence(
            num_sequences=2, allow_duplicates=False, allow_consecutive=True
        )


def test_permutable_sequence_spec_impossible_without_consecutive() -> None:
    # asking for length of 2 without consecutives when
    # there's only 1 possible sequences throw error
    bad_sequence = PermutableSequenceSpec([fixed_sequence_12])
    with pytest.raises(ValueError):
        bad_sequence.get_sequence(
            num_sequences=2, allow_duplicates=True, allow_consecutive=False
        )


def test_permutable_sequence_spec_duplicates0_consecutive1() -> None:
    permutable_sequence = PermutableSequenceSpec(
        sequence_spec_list=[fixed_sequence_12, fixed_sequence_31, fixed_sequence_12]
    )
    # output's number of fixed_sequence_12 is <= 2,
    # number of fixed_sequence_31 is <=1
    result_seq = permutable_sequence.get_sequence(
        num_sequences=2, allow_duplicates=False, allow_consecutive=True, num_sampling=1
    )
    assert sum(seq == fixed_sequence_12 for seq in result_seq) <= 2
    assert sum(seq == fixed_sequence_31 for seq in result_seq) <= 1


def test_permutable_sequence_spec_duplicates0_consecutive0() -> None:
    permutable_sequence = PermutableSequenceSpec(
        sequence_spec_list=[fixed_sequence_12, fixed_sequence_31, fixed_sequence_12]
    )
    # no consecutive allowed
    # Each of the sequence_spec_list is treated as unique
    # so the result would be the same
    result_seq = permutable_sequence.get_sequence(
        num_sequences=2, allow_duplicates=False, allow_consecutive=False, num_sampling=1
    )
    assert sum(seq == fixed_sequence_12 for seq in result_seq) <= 2
    assert sum(seq == fixed_sequence_31 for seq in result_seq) <= 1


def test_permutable_sequence_spec_duplicates1_consecutive1() -> None:
    # Allowing sampling with replacement, and
    # consecutive means all combinations possible
    # Possible: (1231), (1212), (3112), (3131)
    target_sequences_all = [
        [trial1, trial2, trial3, trial1],
        [trial1, trial2, trial1, trial2],
        [trial3, trial1, trial1, trial2],
        [trial3, trial1, trial3, trial1],
    ]
    permutable_sequence = PermutableSequenceSpec(
        sequence_spec_list=[fixed_sequence_12, fixed_sequence_31]
    )
    result_seq = permutable_sequence.get_sequence(
        num_sequences=2, allow_duplicates=True, allow_consecutive=True, num_sampling=1
    )
    assert any(result_seq == target_seq for target_seq in target_sequences_all)


def test_permutable_sequence_spec_duplicates1_consecutive0_fail() -> None:
    permutable_sequence = PermutableSequenceSpec(
        sequence_spec_list=[fixed_sequence_12, fixed_sequence_31]
    )
    # set seed, force the generated sequence to have repeat, and
    # make sure exception is raised.
    random.seed(0)
    with pytest.raises(RuntimeError):
        _ = permutable_sequence.get_sequence(
            num_sequences=2,
            allow_duplicates=True,
            allow_consecutive=False,
            num_sampling=1,
        )


def test_permutable_sequence_spec_duplicates1_consecutive0_success() -> None:
    target_sequences = [
        [trial1, trial2, trial3, trial1],
        [trial3, trial1, trial1, trial2],
    ]

    permutable_sequence = PermutableSequenceSpec(
        sequence_spec_list=[fixed_sequence_12, fixed_sequence_31]
    )
    # set seed, force the generated sequence to be valid
    random.seed(1)
    result_seq = permutable_sequence.get_sequence(
        num_sequences=2, allow_duplicates=True, allow_consecutive=False, num_sampling=1
    )
    assert any(result_seq == target_seq for target_seq in target_sequences)


def test_sequence_spec() -> None:
    fixed_sequence_1 = FixedSequenceSpec(fixed_sequence=[trial1])
    fixed_sequence_12 = FixedSequenceSpec(fixed_sequence=[trial1, trial2])
    fixed_sequence_31 = FixedSequenceSpec(fixed_sequence=[trial3, trial1])
    fixed_sequence_3 = FixedSequenceSpec(fixed_sequence=[trial3])

    permutable_sequence = PermutableSequenceSpec([fixed_sequence_12, fixed_sequence_31])

    result_seq = sum(
        [
            fixed_sequence_1.get_sequence(1),
            permutable_sequence.get_sequence(
                num_sequences=2, allow_consecutive=False, allow_duplicates=False
            ),
            fixed_sequence_3.get_sequence(1),
        ],
        [],
    )

    target_sequences = [
        [trial1, trial1, trial2, trial3, trial1, trial3],
        [trial1, trial3, trial1, trial1, trial2, trial3],
    ]

    random.seed(1)
    assert any(result_seq == target_seq for target_seq in target_sequences)


def test_permutable_sequence_spec_reptition_allow_consecutive() -> None:
    """
    Consecutives allowed, any sequence should be fine
    """
    random.seed(0)
    fixed_sequence_1 = FixedSequenceSpec(fixed_sequence=[trial1])
    fixed_sequence_2 = FixedSequenceSpec(fixed_sequence=[trial2])
    fixed_sequence_3 = FixedSequenceSpec(fixed_sequence=[trial3])
    permutable_sequence = PermutableSequenceSpec(
        [fixed_sequence_1, fixed_sequence_2, fixed_sequence_3]
    )
    result_seq = permutable_sequence.get_sequence_with_repetition(
        n_reps_per_sequence=2, allow_consecutive=[True, True, True],
    )
    condition_table = defaultdict(int)
    for trial in result_seq:
        condition_table[trial.condition] += 1
    assert len(condition_table.keys()) == 3
    assert all(rep == 2 for rep in list(condition_table.values()))


def test_permutable_sequence_spec_bad_reptition() -> None:
    """
    Impossible scenario when only one sequence needs to be
    repeated twice, but no consecutive is allowed
    """
    random.seed(0)
    fixed_sequence_1 = FixedSequenceSpec(fixed_sequence=[trial1])
    permutable_sequence = PermutableSequenceSpec([fixed_sequence_1])
    with pytest.raises(ValueError):
        result_seq = permutable_sequence.get_sequence_with_repetition(
            n_reps_per_sequence=2, allow_consecutive=[False],
        )


def test_permutable_sequence_spec_reptition_bad_consecutive() -> None:
    """
    Calling get_sequence_with_reptition requires `allow_consecutive`
    parameter to be same length as number of member sequences
    """
    random.seed(0)
    fixed_sequence_1 = FixedSequenceSpec(fixed_sequence=[trial1])
    fixed_sequence_2 = FixedSequenceSpec(fixed_sequence=[trial2])
    fixed_sequence_3 = FixedSequenceSpec(fixed_sequence=[trial3])
    permutable_sequence = PermutableSequenceSpec(
        [fixed_sequence_1, fixed_sequence_2, fixed_sequence_3]
    )
    with pytest.raises(ValueError):
        result_seq = permutable_sequence.get_sequence_with_repetition(
            n_reps_per_sequence=2, allow_consecutive=[False, False],
        )


def test_permutable_sequence_spec_repetition_no_consecutive() -> None:
    """
    Three sequences, each need to be repeated twice, but
    neither can be consecutive. This is doable!
    """
    random.seed(0)
    fixed_sequence_1 = FixedSequenceSpec(fixed_sequence=[trial1])
    fixed_sequence_2 = FixedSequenceSpec(fixed_sequence=[trial2])
    fixed_sequence_3 = FixedSequenceSpec(fixed_sequence=[trial3])
    permutable_sequence = PermutableSequenceSpec(
        [fixed_sequence_1, fixed_sequence_2, fixed_sequence_3]
    )
    result_seq = permutable_sequence.get_sequence_with_repetition(
        n_reps_per_sequence=2, allow_consecutive=[False, False, False],
    )
    condition_table = defaultdict(int)
    last_trial = None
    for trial in result_seq:
        if last_trial is not None and last_trial.condition == trial.condition:
            raise RuntimeError("Found consecutive sequences!")
        condition_table[trial.condition] += 1
        last_trial = trial
    assert len(condition_table.keys()) == 3
    assert all(rep == 2 for rep in list(condition_table.values()))
