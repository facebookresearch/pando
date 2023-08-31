#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass
from typing import Callable, List, Optional

from bci.stimulus.psychopy.constants import (
    CorrectConditionEvents,
    StimulusGroupsType,
)

from .display_config import DisplayConfig
from .timing_params import TimingParams
from .trial_spec import TrialSpec


@dataclass
class TrialBasedProtocolSpec:
    """
    Describe a Labgraph v1 experiment protocol.
    """

    display_config: DisplayConfig
    stimulus_groups: StimulusGroupsType
    timing_params: TimingParams
    # This is needed when behavior feedback is needed
    correct_condition_events: Optional[CorrectConditionEvents]


@dataclass
class RandomTrialOrderProtocolSpec(TrialBasedProtocolSpec):
    trial_spec_set: List[TrialSpec]
    trial_spec_sets_per_block: int
    disallow_consecutive: List[int]


@dataclass
class DefinedTrialOrderProtocolSpec(TrialBasedProtocolSpec):
    # List of trial sequences for each of the
    # experiment block
    # The function should take an argument of number of blocks
    func_get_trial_sequence: Callable[[int], List[List[TrialSpec]]]
