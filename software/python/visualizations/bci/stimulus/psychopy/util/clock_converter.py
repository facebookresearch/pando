#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time

from psychopy.core import MonotonicClock


class ClockConverter:
    clock: MonotonicClock
    create_time_abs: float

    def __init__(self) -> None:
        self.clock = MonotonicClock()
        self.create_time_abs = time.time()

    def to_system(self, clock_ts: float) -> float:
        return self.create_time_abs + clock_ts

    def to_clock(self, system_ts: float) -> float:
        return system_ts - self.create_time_abs
