#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Any, Dict, Type

import pkg_resources
import yaml
from bci.systems.configuration.system_configuration import SystemConfiguration


def from_yaml(
    pkg: str, filename: str, config_cls: Type[SystemConfiguration] = SystemConfiguration
) -> Dict[str, Any]:
    with pkg_resources.resource_stream(pkg, filename) as stream:
        _raw_configuration = yaml.load(stream, Loader=yaml.FullLoader)
    return config_cls(_raw_configuration)
