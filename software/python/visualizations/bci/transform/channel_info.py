#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

from typing import Dict, NamedTuple

import numpy as np
import yaml

from bci.systems.configuration.system_configuration import SystemConfiguration

from ..data.types.channel_info import DOTChannelInfo


SOHO3_WAVELENGTH_TABLE = {0: 830.0, 1: 685.0}
LUMO_SOURCE_GROUP_TO_WAVELENGTH = {"Infrared": 850.0, "Red": 735.0}


# Data class
class ConfigValues(NamedTuple):
    num_wavelengths: int
    num_canisters: int
    num_sources_per_wavelength_per_canister: int
    num_detectors_per_canister: int
    num_channels: int
    num_sources: int
    num_detectors: int


def system_configuration_to_channel_info(
    sysconfig: SystemConfiguration,
) -> DOTChannelInfo:

    if (sysconfig.config_version == "1.0.0") and ("lumo" in sysconfig.name.lower()):
        which_wave = LUMO_SOURCE_GROUP_TO_WAVELENGTH
    else:
        which_wave = sysconfig.canisters[0].wavelengths
    ConfigValues = calculate_config_values(sysconfig)

    # Construct the flattened source and detector geometry
    # Note: Then end result of this is that we have sources
    # flattened out for wavelengths and canisters, and detectors
    # flattened out for canisters
    # E.g for a two canister Simmons device it source positions would be ordered
    # as: canister 0, wl=680 ; canister 0, wl 850; canister 1, wl=680 ; canister 1, wl=850
    # The wavelengths array is the same length as the source positions, and would
    # be: 680 ... 680 ; 850 ... 850 ; 680 ... 680 ; 850 ... 850
    # Detector positions would be: canister 0 ; canister 1
    #
    source_pos = np.empty((ConfigValues.num_sources, 3))
    detector_pos = np.empty((ConfigValues.num_detectors, 3))
    wavelengths = np.empty(ConfigValues.num_sources)
    for canister in sysconfig.canisters:
        for group_name, source_group in canister.source_groups.items():
            source_indices = source_group.sources
            canister_source_pos = canister.coordinates[source_indices]
            source_pos[source_group.slice["source"]] = canister_source_pos
            wavelengths[source_group.slice["source"]] = which_wave[group_name]
        detector_pos[canister.slice["detector"]] = canister.coordinates[
            canister.detectors
        ]

    # Why not .zeros? -> Faster, no default values
    source_indices = np.empty(ConfigValues.num_channels, dtype=np.int)
    collapsed_source_indices = np.empty(ConfigValues.num_channels, dtype=np.int)
    detector_indices = np.empty(ConfigValues.num_channels, dtype=np.int)
    canister_indices = np.empty(ConfigValues.num_channels, dtype=np.int)
    wavelengths_final = np.empty(ConfigValues.num_channels)
    collapsed_source_indices = np.empty(ConfigValues.num_channels, dtype=np.int)
    collapsed_detector_indices = np.empty(ConfigValues.num_channels, dtype=np.int)
    # Prepare keys for wavelengths
    wave_keys = list(sysconfig.canisters[0].source_groups.keys())
    num_sd_per_wavelength = (
        ConfigValues.num_sources_per_wavelength_per_canister
        * ConfigValues.num_detectors_per_canister
        * ConfigValues.num_canisters
    )
    for wave_idx, which_wave_name in enumerate(wave_keys):
        for source_idx in range(ConfigValues.num_sources_per_wavelength_per_canister):
            for canister_idx, canister in enumerate(sysconfig.canisters):
                source_group = canister.source_groups[which_wave_name]
                for detector_idx in range(ConfigValues.num_detectors_per_canister):
                    # The global_idx defines the order in which each channel is stored
                    # in the DOT data
                    global_idx = (
                        wave_idx * num_sd_per_wavelength
                        + source_idx
                        * ConfigValues.num_detectors_per_canister
                        * ConfigValues.num_canisters
                        + canister_idx * ConfigValues.num_detectors_per_canister
                        + detector_idx
                    )
                    # This separates out the sources so different wavelengths/canisters are different source indices
                    # These are used to index into the source and detector position array calculated earlier, when we
                    # create the DOTChannelInfo object
                    source_indices[global_idx] = (
                        source_idx
                        + wave_idx
                        * ConfigValues.num_sources_per_wavelength_per_canister
                        + canister_idx
                        * ConfigValues.num_sources_per_wavelength_per_canister
                        * len(wave_keys)
                    )
                    # This separates out detectors for different canisters, and is used to index into the array
                    # of detector positions calculated earlier when we create the DOTChannelInfo object
                    detector_indices[global_idx] = (
                        detector_idx
                        + canister_idx * ConfigValues.num_detectors_per_canister
                    )
                    canister_indices[global_idx] = canister_idx
                    wavelengths_final[global_idx] = which_wave[which_wave_name]
                    # These source/detector indices are per wavelength, per canister, as is used in the channel_info
                    # structure
                    collapsed_source_indices[global_idx] = source_idx
                    collapsed_detector_indices[global_idx] = detector_idx

    # The rest are common across different configurations
    distances = np.sqrt(
        np.sum(
            (source_pos[source_indices] - detector_pos[detector_indices]) ** 2, axis=1
        )
    )

    return DOTChannelInfo(
        detector_idx=collapsed_detector_indices,
        source_idx=collapsed_source_indices,
        distance=distances,
        source_x=source_pos[source_indices, 0],
        source_y=source_pos[source_indices, 1],
        source_z=source_pos[source_indices, 2],
        detector_x=detector_pos[detector_indices, 0],
        detector_y=detector_pos[detector_indices, 1],
        detector_z=detector_pos[detector_indices, 2],
        carrier_freq=np.zeros(ConfigValues.num_channels, dtype=np.float),
        carrier_freq_idx=np.zeros(ConfigValues.num_channels, dtype=np.int),
        wavelength=wavelengths_final,
        canister_idx=canister_indices,
        # it seems like we can get rid of this map because the key isn't
        # actually relevant anywhere after the conversion to DOTChannelInfo
        # i.e. there are no longer any arrays that reference wavelengths
        # by index.  this is here just as a dummy map for now to adhere to
        # the existing soho3 stuff
        wavelength_table={
            idx: wavelength for idx, wavelength in enumerate(np.unique(wavelengths))
        },
        device_name=sysconfig.name,
    )


def dot_config_to_channel_info(local_path: str) -> DOTChannelInfo:
    with open(local_path, "r") as f:
        config_dict = yaml.safe_load(f)

    sysconfig = SystemConfiguration(config_dict)

    return system_configuration_to_channel_info(sysconfig)


def calculate_config_values(sysconfig: SystemConfiguration) -> ConfigValues:
    num_wavelengths = len(sysconfig.canisters[0].source_groups.keys())
    num_canisters = len(sysconfig.canisters)
    num_sources_per_wavelength_per_canister = int(
        sysconfig.num_sources / num_wavelengths / num_canisters
    )
    num_detectors_per_canister = int(sysconfig.num_detectors / num_canisters)
    # TODO: No support for cross-canisters (T77127096)
    # In the future, this may need to be modified for Simmons.
    # Currently, the Lumo considers all tiles to be one canister.
    # Simmons account for different canisters. However, current Simmons
    # doesn't allow different sources in different canisters to be detected
    # in different detectors. i.e. Canister A source -> Canister B detector
    # This makes the total number of channels to be divided by the number of
    # canisters to match the total number of channels that are actually
    # being recorded.
    num_channels = int(
        num_sources_per_wavelength_per_canister
        * num_detectors_per_canister
        * num_canisters
        * num_wavelengths
    )
    # Number of total sources
    num_sources = (
        num_sources_per_wavelength_per_canister * num_wavelengths * num_canisters
    )
    # Number of total detectors
    num_detectors = num_detectors_per_canister * num_canisters

    return ConfigValues(
        num_wavelengths=num_wavelengths,
        num_canisters=num_canisters,
        num_sources_per_wavelength_per_canister=num_sources_per_wavelength_per_canister,
        num_detectors_per_canister=num_detectors_per_canister,
        num_channels=num_channels,
        num_sources=num_sources,
        num_detectors=num_detectors,
    )
