#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

# BCI source-detector distances computation

import bci.systems.configuration.system_configuration as sc
import bci.systems.configuration.system_configuration_global_accessors as scga
import numpy as np


# Returns a 2D array with shape (#sources, #detectors) containing the distance
# between each source and detector.
# This includes cross-canister source-detector distances. To get only single
# canister distances, use the slices defined for a canister.
def get_source_detector_distances(
    system_configuration: sc.SystemConfiguration,
) -> np.ndarray:
    sd_distances = np.ndarray(
        shape=(system_configuration.num_sources, system_configuration.num_detectors),
        dtype=float,
        order="C",  # C-style array ordering (row-major)
    )

    # Broadcast sources and detectors into the shape
    # (#sources, #detectors, #coordinates = 3),
    # subtract them, and take the norm in the coordinate axis
    sd_distances = np.linalg.norm(
        scga.global_source_coordinates(system_configuration)[:, np.newaxis, :]
        - scga.global_detector_coordinates(system_configuration)[np.newaxis, :, :],
        axis=2,
    )

    return sd_distances
