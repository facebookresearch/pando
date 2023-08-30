#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

# Defines global accessors for the system configuration.

from typing import Generator

import numpy as np
from bci.systems.configuration.system_configuration import SystemConfiguration


# Returns coordinates for all canisters in the order they are specified
# in the config file.
def global_coordinates(system_configuration: SystemConfiguration) -> np.ndarray:
    return np.concatenate(
        [canister.coordinates for canister in system_configuration.canisters]
    )


# Returns source coordinate indices for all canisters in order they are
# specified in the config file.
def global_source_coordinate_indices(
    system_configuration: SystemConfiguration,
) -> Generator[int, None, None]:
    global_coordinate_offset = 0
    for canister in system_configuration.canisters:
        for source_group in canister.source_groups.values():
            for source_index in source_group.sources:
                yield source_index + global_coordinate_offset
        global_coordinate_offset += canister.num_coordinates


# Returns detector coordinate indices for all canisters in order they are
# specified in the config file.
def global_detector_coordinate_indices(
    system_configuration: SystemConfiguration,
) -> Generator[int, None, None]:
    global_coordinate_offset = 0
    for canister in system_configuration.canisters:
        for detector_index in canister.detectors:
            yield detector_index + global_coordinate_offset
        global_coordinate_offset += canister.num_coordinates


# Returns source coordinates for all canisters in order they are
# specified in the config file.
def global_source_coordinates(system_configuration: SystemConfiguration) -> np.ndarray:
    return global_coordinates(system_configuration)[
        list(global_source_coordinate_indices(system_configuration))
    ]


# Returns detector coordinates for all canisters in order they are
# specified in the config file.
def global_detector_coordinates(
    system_configuration: SystemConfiguration,
) -> np.ndarray:
    return global_coordinates(system_configuration)[
        list(global_detector_coordinate_indices(system_configuration))
    ]
