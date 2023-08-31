#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest

import yaml
from bci.experiments.common.config import YAMLConfig

PACKAGE = "bci.systems.colorado2.configuration"


class TestValidYaml(unittest.TestCase):
    def test_colorado2_placeholder(self) -> None:
        try:
            YAMLConfig(
                package_root="bci.systems.colorado2",
                package="configuration",
                filename="colorado2-placeholder.yaml",
            )
        except yaml.YAMLError as err:
            self.fail(f"Error parsing yaml file: {str(err)}")


if __name__ == "__main__":
    unittest.main()
