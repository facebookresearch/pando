#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest

import bci.systems.configuration.system_configuration_global_accessors as scga
import numpy as np
from bci.experiments.common.config import YAMLConfig
from bci.systems.configuration.distances import get_source_detector_distances
from bci.systems.configuration.system_configuration import SystemConfiguration
from bci.systems.configuration.tests import TEST_FILENAME


class DistancesTest(unittest.TestCase):
    def test_source_detector_distances(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        sd_distances = get_source_detector_distances(system_configuration)

        global_source_coordinates = scga.global_source_coordinates(system_configuration)
        global_detector_coordinates = scga.global_detector_coordinates(
            system_configuration
        )

        self.assertEqual(
            np.shape(sd_distances),
            (
                np.shape(global_source_coordinates)[0],
                np.shape(global_detector_coordinates)[0],
            ),
        )

        for source_index in range(np.shape(global_source_coordinates)[0]):
            source_coordinate = global_source_coordinates[source_index]
            for detector_index in range(np.shape(global_detector_coordinates)[0]):
                detector_coordinate = global_detector_coordinates[detector_index]

                expected_distance = np.linalg.norm(
                    source_coordinate - detector_coordinate
                )
                self.assertEqual(
                    sd_distances[source_index, detector_index], expected_distance
                )
