#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Tuple, Union


TIMER_INTERVAL = 33  # Unit is ms. Updates plot at around 30 FPS.


class AutoRange:
    pass


AxisRange = Union[AutoRange, Tuple[float, float]]
