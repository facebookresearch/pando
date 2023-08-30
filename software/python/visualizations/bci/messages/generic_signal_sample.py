#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Optional

import labgraph as df
import numpy as np


class SignalSampleMessage(df.TimestampedMessage):
    # The actual shape of this depends on the configuration
    sample: df.NumpyDynamicType()
    """
    This field is useful in filtering and signal processing nodes.
    - `timestamp` (inherited from df.TimestampedMessage) represents
        time when the message is created, this is useful probably
        for labgraph message syncing purposes.
    - `sample_timestamp` here represents the absolute timestamp for which
        the current message corresponds to (for causal filtering, this is
        in the past compared to `__super__.timestamp.
        The default value cannot be nan because writing None-type does not
        have dtype and cannot be written to hdf5.
    """
    sample_timestamp: float = np.nan
