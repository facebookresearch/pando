#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
from bci.constants.laterality import ActivationSide


class DynamicPhantomTriggerMessage(df.TimestampedMessage):
    side: ActivationSide
