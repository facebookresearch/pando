#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass


@dataclass
class TimingParams:
    # TODO: Add training/feedback members
    initial_rest_duration: float
    final_rest_duration: float
    block_number_duration: float
