#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import random
from dataclasses import dataclass
from typing import List

from .trial_spec import TrialSpec


@dataclass
class FixedSequenceSpec:
    """
    Represent a sequence of `TrialSpec`, whose
    order is defined by that in `fixed_sequence` list

    """

    fixed_sequence: List[TrialSpec]

    def get_sequence(self, num_trial_specs: int = None) -> List[TrialSpec]:
        """
        Produces a list of TrialSpec from the fixed_sequence.

        Inputs:
        - num_trial_specs: Total number of TrialSpecs in the returned
            list. This will be produced by repeating the `fixed_sequence`
            and truncating the last reptition so the resulting
            list has this length.

            Default is len(fixed_sequence)
        """
        if num_trial_specs is None:
            num_trial_specs = len(self.fixed_sequence)
        return (
            self.fixed_sequence * int(num_trial_specs / len(self.fixed_sequence))
            + self.fixed_sequence[: (num_trial_specs % len(self.fixed_sequence))]
        )


@dataclass
class PermutableSequenceSpec:
    """
    Represent a sequence of length `sequence_length`,
    where the member sequence types can be any inside
    `sequence_spec_list`.
    """

    sequence_spec_list: List[FixedSequenceSpec]

    def get_sequence(
        self,
        num_sequences: int,
        allow_duplicates: bool,
        allow_consecutive: bool,
        num_sampling: int = 1000,
    ) -> List[TrialSpec]:
        """
        Generate a list of trial specs according to criteria by first
        sampling from `sequence_spec_list` a list of `FixedSequenceSpecs`,
        which can be used to generate the actual `TrialSpecs`.

        Inputs:
        - num_sequences: The flat list of trial specs returned will be
            formed by first sampling and concatenating FixedSequenceSpec
            from the possible list, `sequence_spec_list` this number of times.
            The sampled FixedSequenceSpecs will then yield the flat trial specs.
        - allow_duplicates: If true, then any member of `sequence_spec_list` can
            appear multiple times in the sampled sequence list.
            If false, that is not allowed. This also means if `allow_duplicates` is
            False, and `num_sequences` > `len(sequence_spec_list`, the requested
            sequence is impossible.
        - allow_consecutive: If true, then the sampled list of `FixedSequenceSpecs` can
            have consecutive identical members. Otherwise it's not allowed.
        - num_sampling: Number of times to try and obtain a sequence sampling fitting
            criteria. This should scale with len(sequence_spec_list).
        """
        if (not allow_consecutive) and len(self.sequence_spec_list) < 2:
            raise ValueError(
                "Non-consecutive sequences impossible with ",
                "less than 2 possible sub-sequences",
            )
        if not allow_duplicates and num_sequences > len(self.sequence_spec_list):
            raise ValueError(
                "num_sequences > len(sequence_spec_list), ",
                "cannot generate without duplicate sequences",
            )
        sequence_list = []
        if not allow_duplicates:
            sequence_list = random.sample(self.sequence_spec_list, num_sequences)
        else:
            i_sampling = 0
            while i_sampling < num_sampling:
                sequence_list = random.choices(self.sequence_spec_list, k=num_sequences)
                i_sampling += 1
                if allow_consecutive:
                    break
                elif not _has_repeat_sequences(sequence_list):
                    break
            if (
                not allow_consecutive
                and (i_sampling == num_sampling)
                and _has_repeat_sequences(sequence_list)
            ):
                raise RuntimeError(
                    f"Failed sequence generation with {num_sampling} samplings"
                )
        trial_list = sum((seq.get_sequence() for seq in sequence_list), [])
        return trial_list

    def get_sequence_with_repetition(
        self,
        n_reps_per_sequence: int,
        allow_consecutive: List[bool] = None,
        num_sampling: int = 1000,
    ) -> List[TrialSpec]:
        """
        Generate a list of trial specs from `sequence_spec_list`.
        Each FixedSequenceSpec member of `sequence_spec_list` will
        appear `n_reps_per_sequence` times in the result.
        The final returned list of TrialSpec will be the generated
        sequences flattened into trials.

        Ex:
        Calling this function with the following:
            sequence_spec_list = [s1, s2, s3]
            allow_consecutive = [True, False, False]
            n_reps_per_sequence = 2
        Can have potential result:
            [s1, s1, s2, s3, s2, s3]
        s1 can appear next to itself, but not s2 or s3. Each sequence
        is repeated 2 times.

        Inputs:
        - n_reps_per_sequence: Number of times each FixedSequenceSpec
            member of `sequence_spec_list` will be repeated. The returned
            result of this function will have length equal to:
                n_reps_per_sequence * sum(
                    len(seq.fixed_sequence) for seq in sequence_spec_list
                )
        - allow_consecutive: List[bool], indicates for each of `sequence_spec_list`
            members whether they can be sampled consecutively. Default will
            assume all sequences can be sampled consecutively.
        - num_sampling: Number of times to try and obtain a sequence sampling fitting
            criteria. This should scale with len(sequence_spec_list).
        """
        if allow_consecutive is None:
            allow_consecutive = [True] * len(self.sequence_spec_list)
        elif len(allow_consecutive) != len(self.sequence_spec_list):
            raise ValueError(
                "allow_consecutive needs to have same length as sequence_spec_list"
            )
        if (n_reps_per_sequence > 1) and allow_consecutive == [False]:
            raise ValueError(
                "Non-consecutive sequences impossible with ",
                "one sequence and multiple repetitions",
            )
        sequence_idx_list = (
            list(range(len(self.sequence_spec_list))) * n_reps_per_sequence
        )
        i_sampling = 0
        while i_sampling < num_sampling:
            # Take a balanced sample
            sequence_idx_list = random.sample(sequence_idx_list, len(sequence_idx_list))
            i_sampling += 1
            if all(allow_consecutive):
                break
            # Make sure we don't have disallowed consecutive
            if not _has_forbidden_consecutive(sequence_idx_list, allow_consecutive):
                break
        if (
            not all(allow_consecutive)
            and (i_sampling == num_sampling)
            and _has_forbidden_consecutive(sequence_idx_list, allow_consecutive)
        ):
            raise RuntimeError(
                "Failed sequence generation with repetition ",
                f"with {n_reps_per_sequence} repetitions ",
                f"and allow_consecutive={allow_consecutive}",
            )
        sequence_list = [self.sequence_spec_list[idx] for idx in sequence_idx_list]
        trial_list = sum((seq.get_sequence() for seq in sequence_list), [])
        return trial_list


def _has_repeat_sequences(sequence_list: List[FixedSequenceSpec]) -> bool:
    return any(x == y for x, y in zip(sequence_list[:-1], sequence_list[1:]))


def _has_forbidden_consecutive(
    sequence_index_list: List[int], allow_consecutive: List[bool],
) -> bool:
    return any(
        s1 == s2
        for s1, s2 in zip(sequence_index_list[:-1], sequence_index_list[1:])
        if not allow_consecutive[s1]
    )
