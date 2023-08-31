#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.experiments.common.config import YAMLConfig


COLORADO2_PLACEHOLDER_FILENAME = "colorado2-placeholder.yaml"
COLORADO2_PLACEHOLDER_YAML_CONFIG = YAMLConfig(
    package_root="arvr.bci.systems.colorado2",
    package="configuration",
    filename=COLORADO2_PLACEHOLDER_FILENAME,
)
