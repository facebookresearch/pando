#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

COLUMN_TYPES = [
    ("Channel", "<U16"),
    ("Count Rate", "<U16"),
    ("CRI", "<U16"),
    ("TOF Mean (> 10% max)", int),
    ("TOF Std (> 10% max)", int),
    ("TOF @ Peak", int),
    ("TOF @ 10% of Peak", int),
]
