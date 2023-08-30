#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import numpy as np
from bci.systems.simmons.configuration import SIMMONS_MOTOR_LEFT_DORSAL_SYSTEM_CONFIG


class SimmonsDemodSampleMessage(df.Message):
    timestamp: float
    sample: df.NumpyType(
        shape=(SIMMONS_MOTOR_LEFT_DORSAL_SYSTEM_CONFIG.num_measurements,),
        dtype=np.float64,
    )
