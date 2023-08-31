#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import functools
import os
from typing import Any, Dict, NamedTuple, Optional, TextIO

import pkg_resources
import yaml


CONFIG_ROOT = "arvr.bci.experiments.common.config"


class Config(NamedTuple):
    """
    Represents a configuration file. The file can either be a config stored in
    this config package, or a file on disk.

    Args:
        package: The subpackage in which to find the configuration file
        filename: The filename for the configuration file
    """

    filename: str
    package_root: str = CONFIG_ROOT
    package: Optional[str] = None

    def open(self) -> TextIO:
        """
        Returns a file-like object for reading from the configuration file.
        """
        if not os.path.isabs(self.filename) and pkg_resources.resource_exists(
            self._full_package, self.filename
        ):
            return pkg_resources.resource_stream(self._full_package, self.filename)
        else:
            return open(self.filename, "r")

    @property
    def _full_package(self) -> str:
        if self.package is not None:
            return f"{self.package_root}.{self.package}"
        else:
            return self.package_root


class YAMLConfig(Config):
    """
    Represents a YAML configuration file. The file can either be a config stored
    in this config package, or a file on disk.

    Args:
        package: The subpackage in which to find the configuration file
        filename: The filename for the configuration file
    """

    @property
    @functools.lru_cache()
    def tree(self) -> Dict[str, Any]:
        """
        The YAML tree in the configuration file.
        """
        with self.open() as yaml_file:
            return yaml.load(yaml_file, Loader=yaml.FullLoader)
