#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time
from contextlib import contextmanager
from typing import Any

from bci.stimulus.psychopy.util import ClockConverter
from psychopy.core import MonotonicClock

pytest_plugins = ["pytest_mock"]


@contextmanager
def _mock_time(mocker: Any, now: float) -> None:
    mock_time = mocker.patch("arvr.bci.stimulus.psychopy.util.clock_converter.time")
    mock_time.time.return_value = now
    yield
    mock_time.time.assert_called()


def test_clock_converter(mocker: Any) -> None:
    now = time.time()
    with _mock_time(mocker, now):
        clock_converter = ClockConverter()
    assert isinstance(clock_converter.clock, MonotonicClock)
    assert clock_converter.create_time_abs == now
    assert clock_converter.to_system(0.1) == now + 0.1
    assert clock_converter.to_clock(now) == 0.0
