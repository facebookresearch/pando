#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.stimulus.psychopy.images import load_image
from PIL import Image


def test_load_image():
    output = load_image("null")
    expected = Image.open("bci/stimulus/psychopy/images/files/null.png")
    assert isinstance(output, Image.Image)
    assert output == expected
