#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass, field
from typing import Tuple

from bci.stimulus.psychopy.constants import Condition


@dataclass
class TrialSpec:
    condition: Condition
    pre_stimulus_duration_range: Tuple[float, float]
    post_stimulus_duration_range: Tuple[float, float]
    performance_duration_range: Tuple[float, float]
    condition_type_name: str = field(init=False)
    max_duration: float = field(init=False)

    def __post_init__(self):
        self.condition_type_name = self.condition.__class__.__name__
        self.max_duration = (
            self.pre_stimulus_duration_range[1]
            + self.performance_duration_range[1]
            + self.post_stimulus_duration_range[1]
        )
