#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import math
from dataclasses import InitVar, dataclass, field
from typing import Any, Dict, List

import numpy as np


@dataclass
class SourceGroup(object):
    """
    A group of sources of the same type.
    The meaning of "type" is dependent on the system.
    For DOT systems "type" indicates wavelength, e.g. "Red" or "Infrared".

    sources: A list of source coordinate indices.
    source_offset: The offset into a global sources array
    num_sources: The total number of sources.
    wavelength: Frequency of the light wave.
    """

    sources: np.ndarray
    source_offset: int
    num_sources: int = field(init=False)
    wavelength: float

    def __post_init__(self) -> None:
        self.num_sources = np.shape(self.sources)[0]

    def __getitem__(self, key: int) -> Any:
        return self._sources[key]

    @property
    def slice(self) -> Dict[str, slice]:
        return {
            "source": slice(self.source_offset, self.source_offset + self.num_sources)
        }


@dataclass
class Canister(object):
    """
    A canister.

    name: The name of the canister.
    wavelengths: A dictionary of wavelength name to wavelength frequency.
    coordinates: An array of 3D coordinates with shape (#sources, 3).
    source_groups: A list of source groups.
    detectors: An index array which represents the detector to coordinate mapping.
    coordinates_offset: The offset into a global coordinates array
    source_offset: The offset into a global sources array
    detector_offset: The offset into a global detectors array
    num_coordinates: The total number of coordinates.
    num_sources: The total number of sources.
    num_detectors: The total number of detectors.
    """

    name: str
    wavelengths: Dict[str, float]
    coordinates: np.ndarray
    source_groups: Dict[str, SourceGroup]
    detectors: np.ndarray
    coordinates_offset: int
    source_offset: int
    detector_offset: int
    num_coordinates: int = field(init=False)
    num_sources: int = field(init=False)
    num_detectors: int = field(init=False)

    def __post_init__(self) -> None:
        self.num_coordinates = np.shape(self.coordinates)[0]

        num_sources = 0
        for _, source_group in self.source_groups.items():
            num_sources += source_group.num_sources
        self.num_sources = num_sources

        self.num_detectors = np.shape(self.detectors)[0]

    @property
    def slice(self) -> Dict[str, slice]:
        return {
            "source": slice(self.source_offset, self.source_offset + self.num_sources),
            "detector": slice(
                self.detector_offset, self.detector_offset + self.num_detectors
            ),
        }


@dataclass
class SystemConfiguration(object):
    """
    A full system configuration.

    config_version: Config version.
    name: Device name.
    canisters: A list of canisters.
    num_coordinates: The total number of coordinates.
    num_sources: The total number of sources.
    num_detectors: The total number of detectors.
    """

    config_version: str = field(init=False)
    name: str = field(init=False)
    canisters: List[Canister] = field(init=False)
    num_coordinates: int = field(init=False)
    num_sources: int = field(init=False)
    num_detectors: int = field(init=False)
    num_measurements: int = field(init=False)
    _raw_configuration: InitVar[Dict[str, Any]]

    # Homogeneous coordinates affine transformation
    # This uses a left-multiplication by the matrix.
    # http://graphics.cs.cmu.edu/nsp/course/15-462/Spring04/slides/04-transform.pdf
    def homogeneous_matrix_transformation(
        self, matrix: np.ndarray, coordinate: np.ndarray
    ) -> np.ndarray:
        # Append w coordinate
        coordinate = np.append(coordinate, 1.0)
        coordinate = np.matmul(matrix, coordinate)

        # Normalize by w coordinate
        coordinate[0:3] /= coordinate[3]

        return coordinate[0:3]

    def apply_affine_matrix(
        self,
        coordinates: np.ndarray,
        canister_coordinates: Dict[str, Any],
        affine_matrix: Dict[str, Any],
    ):
        matrix = np.array(affine_matrix)
        for c, coordinate in enumerate(canister_coordinates):
            coordinate_array = np.array(coordinate)
            assert np.shape(matrix) == (4, 4)
            assert np.shape(coordinate_array) == (3,)
            coordinate = self.homogeneous_matrix_transformation(
                matrix, coordinate_array
            )
            coordinates[c] = coordinate

    def apply_euler_angles(
        self,
        coordinates: np.ndarray,
        canister_coordinates: Dict[str, Any],
        euler_angles: Dict[str, Any],
    ):
        # Apply Euler angles transformation
        # https://en.wikipedia.org/wiki/Euler_angles
        # Rotations are applied in ZYX order first,
        # then translation is applied.

        # Apply rotation matrices
        # https://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

        x = euler_angles["Rotation"][0]
        y = euler_angles["Rotation"][1]
        z = euler_angles["Rotation"][2]

        if euler_angles["Units"] == "degrees":
            x = math.radians(x)
            y = math.radians(y)
            z = math.radians(z)

        cos_z = math.cos(z)
        sin_z = math.sin(z)
        z_matrix = np.array(
            [[cos_z, -sin_z, 0.0], [sin_z, cos_z, 0.0], [0.0, 0.0, 1.0]]
        )

        cos_y = math.cos(y)
        sin_y = math.sin(y)
        y_matrix = np.array(
            [[cos_y, 0.0, -sin_y], [0.0, 1.0, 0.0], [-sin_y, 0.0, cos_y]]
        )

        cos_x = math.cos(x)
        sin_x = math.sin(x)
        x_matrix = np.array(
            [[1.0, 0.0, 0.0], [0.0, cos_x, -sin_x], [0.0, sin_x, cos_x]]
        )

        static_transformation = np.matmul(x_matrix, np.matmul(y_matrix, z_matrix))

        for c, coordinate in enumerate(canister_coordinates):
            coordinate_array = np.array(coordinate)

            coordinate_array = np.matmul(static_transformation, coordinate_array)

            for x in range(3):
                coordinate_array[x] += euler_angles["Translation"][x]

            coordinates[c] = coordinate_array

    def apply_transformation(
        self,
        coordinates: np.ndarray,
        canister_coordinates: Dict[str, Any],
        transformation: Dict[str, Any],
    ):
        if "AffineMatrix" in transformation:
            self.apply_affine_matrix(
                coordinates, canister_coordinates, transformation["AffineMatrix"]
            )
        elif "EulerAngles" in transformation:
            self.apply_euler_angles(
                coordinates, canister_coordinates, transformation["EulerAngles"]
            )

    def __post_init__(self, _raw_configuration: Dict[str, Any]) -> None:
        # Parse raw config to build the system configuration
        self.config_version = _raw_configuration["ConfigVersion"]
        self.name = _raw_configuration["DeviceName"]
        self.canisters = []

        coordinates_offset = 0
        source_offset = 0
        detector_offset = 0

        for canister in _raw_configuration["Canisters"]:
            coordinates = np.full(
                (len(canister["Coordinates"]), 3), np.nan, dtype=np.float64
            )
            # Apply transformation
            if "Transformation" in canister.keys():
                self.apply_transformation(
                    coordinates, canister["Coordinates"], canister["Transformation"]
                )
            else:
                for c, coordinate in enumerate(canister["Coordinates"]):
                    coordinates[c] = coordinate

            wavelengths = {}
            # TODO(rsevile): Replace config version with 3 integers.
            if self.config_version != "1.0.0":
                assert "Wavelengths" in canister["Sources"]

            if "Wavelengths" in canister["Sources"]:
                wavelengths = canister["Sources"]["Wavelengths"]

            total_sources = 0
            source_groups = {}
            for key, raw_sources in canister["Sources"].items():
                if key == "Wavelengths":
                    continue
                sources_array = np.full(len(raw_sources), 0, np.int32)
                for s, source in enumerate(raw_sources):
                    sources_array[s] = source["coordinate"]
                if self.config_version == "1.0.0":
                    source_groups[key] = SourceGroup(
                        sources_array,
                        source_offset + total_sources,
                        wavelengths[key] if key in wavelengths else -1.0,
                    )
                else:
                    source_groups[key] = SourceGroup(
                        sources_array, source_offset + total_sources, wavelengths[key]
                    )
                total_sources += len(raw_sources)

            detectors = np.full(len(canister["Detectors"]), 0, np.int32)
            for d, detector in enumerate(canister["Detectors"]):
                detectors[d] = detector["coordinate"]

            self.canisters.append(
                Canister(
                    name=canister["Name"],
                    wavelengths=wavelengths,
                    coordinates=coordinates,
                    source_groups=source_groups,
                    detectors=detectors,
                    coordinates_offset=coordinates_offset,
                    source_offset=source_offset,
                    detector_offset=detector_offset,
                )
            )
            coordinates_offset += np.shape(coordinates)[0]
            source_offset += total_sources
            detector_offset += np.shape(detectors)[0]

        num_coordinates = 0
        for canister in self.canisters:
            num_coordinates += canister.num_coordinates
        self.num_coordinates = num_coordinates

        num_sources = 0
        for canister in self.canisters:
            num_sources += canister.num_sources
        self.num_sources = num_sources

        num_detectors = 0
        for canister in self.canisters:
            num_detectors += canister.num_detectors
        self.num_detectors = num_detectors

        self.num_measurements = self.num_detectors * self.num_sources
