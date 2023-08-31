#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest

import bci.systems.configuration.system_configuration_global_accessors as scga
import numpy as np
from bci.experiments.common.config import YAMLConfig
from bci.systems.configuration.system_configuration import SystemConfiguration
from bci.systems.configuration.tests import TEST_FILENAME


class SystemConfigurationGeneratorsTest(unittest.TestCase):
    def test_global_coordinates(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        global_coordinates = scga.global_coordinates(system_configuration)

        total_num_coordinates = 0
        for canister in system_configuration.canisters:
            total_num_coordinates += canister.num_coordinates

        self.assertEqual(np.shape(global_coordinates), (total_num_coordinates, 3))

        global_coordinate_index = 0
        for canister in system_configuration.canisters:
            coordinates_shape = np.shape(canister.coordinates)
            end_coordinate_index = global_coordinate_index + coordinates_shape[0]
            np.testing.assert_array_equal(
                global_coordinates[global_coordinate_index:end_coordinate_index, :],
                canister.coordinates,
            )

            global_coordinate_index = end_coordinate_index

    def test_global_source_coordinate_indices(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        global_source_coordinate_indices = list(
            scga.global_source_coordinate_indices(system_configuration)
        )

        global_coordinate_index = 0
        global_coordinate_offset = 0
        for canister in system_configuration.canisters:
            for source_group in canister.source_groups.values():
                for source_index in source_group.sources:
                    self.assertEqual(
                        global_source_coordinate_indices[global_coordinate_index],
                        source_index + global_coordinate_offset,
                    )

                    global_coordinate_index += 1
            global_coordinate_offset += canister.num_coordinates

    def test_global_detector_coordinate_indices(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        global_detector_coordinate_indices = list(
            scga.global_detector_coordinate_indices(system_configuration)
        )

        global_coordinate_index = 0
        global_coordinate_offset = 0
        for canister in system_configuration.canisters:
            for detector_index in canister.detectors:
                self.assertEqual(
                    global_detector_coordinate_indices[global_coordinate_index],
                    detector_index + global_coordinate_offset,
                )

                global_coordinate_index = global_coordinate_index + 1
            global_coordinate_offset += canister.num_coordinates

    def test_global_source_coordinates(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        global_source_coordinates = scga.global_source_coordinates(system_configuration)

        global_coordinate_index = 0
        for canister in system_configuration.canisters:
            for source_group in canister.source_groups.values():
                for source_index in source_group.sources:
                    self.assertEqual(
                        np.shape(global_source_coordinates[global_coordinate_index]),
                        (3,),
                    )

                    expected_coordinates = canister.coordinates[source_index, :]
                    self.assertEqual(
                        global_source_coordinates[global_coordinate_index, 0],
                        expected_coordinates[0],
                    )
                    self.assertEqual(
                        global_source_coordinates[global_coordinate_index, 1],
                        expected_coordinates[1],
                    )
                    self.assertEqual(
                        global_source_coordinates[global_coordinate_index, 2],
                        expected_coordinates[2],
                    )

                    global_coordinate_index += 1

    def test_global_detector_coordinates(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        global_detector_coordinates = scga.global_detector_coordinates(
            system_configuration
        )

        global_coordinate_index = 0
        for canister in system_configuration.canisters:
            for detector_index in canister.detectors:
                self.assertEqual(
                    np.shape(global_detector_coordinates[global_coordinate_index]), (3,)
                )

                expected_coordinates = canister.coordinates[detector_index, :]
                self.assertEqual(
                    global_detector_coordinates[global_coordinate_index, 0],
                    expected_coordinates[0],
                )
                self.assertEqual(
                    global_detector_coordinates[global_coordinate_index, 1],
                    expected_coordinates[1],
                )
                self.assertEqual(
                    global_detector_coordinates[global_coordinate_index, 2],
                    expected_coordinates[2],
                )

                global_coordinate_index += 1
