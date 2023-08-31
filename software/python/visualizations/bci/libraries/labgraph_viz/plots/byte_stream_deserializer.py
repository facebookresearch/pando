#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import numpy as np


def byte_stream_deserializer(
    width: int, height: int, bytes_per_pixel: int, image_data: bytearray
):
    if bytes_per_pixel == 1:
        data = np.frombuffer(image_data, dtype=np.uint8).reshape(height, width)
    else:
        data = np.frombuffer(image_data, dtype=np.uint16).reshape(height, width)
    return data
