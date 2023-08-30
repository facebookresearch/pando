#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from bci.experiments.common.config import YAMLConfig
from bci.systems.configuration.system_configuration import SystemConfiguration


TEST_FILENAME = "arvr/bci/systems/configuration/tests/test_configuration.yaml"
TEST_YAML_CONFIG = YAMLConfig(
    package_root="bci.systems.configuration", package="tests", filename=TEST_FILENAME,
)
TEST_SYSTEM_CONFIG = SystemConfiguration(TEST_YAML_CONFIG.tree)
