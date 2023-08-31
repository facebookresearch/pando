#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import pkg_resources
from PIL import Image

PKG = "bci.stimulus.psychopy.images.files"


def load_image(filename: str, ext: str = "png") -> Image:
    return Image.open(pkg_resources.resource_stream(PKG, f"{filename}.{ext}"))
