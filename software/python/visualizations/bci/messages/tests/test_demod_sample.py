#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time

import numpy as np
import pytest
from bci.messages.demod_sample import SimmonsDemodSampleMessage


CORRECT_CHANNEL_COUNT = 3198
INCORRECT_CHANNEL_COUNT = 100


def test_simmons_demod_sample() -> None:
    _ = SimmonsDemodSampleMessage(time.time(), np.random.randn(CORRECT_CHANNEL_COUNT))
    with pytest.raises(TypeError):
        _ = SimmonsDemodSampleMessage(
            time.time(), np.random.randn(INCORRECT_CHANNEL_COUNT)
        )
