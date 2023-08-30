#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import NamedTuple


class Simmons(NamedTuple):
    # TODO: Replace this by calling values when BCI SW team implement it
    # 1000 = 1kHz. This is the sampling frequency on the FPGA
    # 164 = 164 ms. This is the time it requires to go through 164 steps.
    # This is currently the default for Simmons system
    sample_rate = 1000 / 164  # Hz
