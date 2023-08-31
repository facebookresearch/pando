#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest

import numpy as np
from bci.experiments.common.config import YAMLConfig
from bci.systems.configuration.system_configuration import SystemConfiguration
from bci.systems.configuration.tests import TEST_FILENAME
from bci.systems.simmons.configuration import SIMMONS_MOTOR_LEFT_DORSAL_FILENAME


TEST_NAME = "test"
TEST_CONFIG_VERSION = "1.1.0"
TEST_NUM_CANISTERS = 3
TEST_CANISTERS_NAME = ("Test0", "Test1", "Test2")

TEST_CANISTERS_WAVELENGTHS = (
    {"Red": 50.0, "Infrared": 100.0},
    {"Red": 50.0},
    {"Red": 50.0},
)
TEST_CANISTERS_COORDINATES = (8, 4, 4)

TEST_CANISTERS_COORDINATES_SHAPE = (
    (TEST_CANISTERS_COORDINATES[0], 3),
    (TEST_CANISTERS_COORDINATES[1], 3),
    (TEST_CANISTERS_COORDINATES[2], 3),
)

TEST_CANISTERS_NUM_SOURCE_GROUPS = (2, 1, 1)

TEST_CANISTERS_SOURCE_GROUPS_TYPE = (("Red", "Infrared"), ("Red",), ("Red",))
TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES = ((9, 8), (4,), (4,))
TEST_CANISTERS_SOURCE_GROUPS_SHAPE = (
    (
        (TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[0][0],),
        (TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[0][1],),
    ),
    ((TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[1][0],),),
    ((TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[2][0],),),
)
TEST_CANISTERS_SOURCE_GROUPS_WAVELENGTHS = ((50.0, 100.0), (50.0,), (50.0,))

TEST_CANISTERS_NUM_SOURCES = (
    TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[0][0]
    + TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[0][1],
    TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[1][0],
    TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[2][0],
)

TEST_CANISTERS_SOURCE_GROUPS_SLICE = (
    (
        {"source": slice(0, TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[0][0])},
        {
            "source": slice(
                TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[0][0],
                TEST_CANISTERS_NUM_SOURCES[0],
            )
        },
    ),
    (
        {
            "source": slice(
                TEST_CANISTERS_NUM_SOURCES[0],
                TEST_CANISTERS_NUM_SOURCES[0]
                + TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[1][0],
            )
        },
    ),
    (
        {
            "source": slice(
                TEST_CANISTERS_NUM_SOURCES[0] + TEST_CANISTERS_NUM_SOURCES[1],
                TEST_CANISTERS_NUM_SOURCES[0]
                + TEST_CANISTERS_NUM_SOURCES[1]
                + TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[2][0],
            )
        },
    ),
)

TEST_CANISTERS_NUM_DETECTORS = (8, 4, 4)

TEST_CANISTERS_DETECTORS_SHAPE = (
    (TEST_CANISTERS_NUM_DETECTORS[0],),
    (TEST_CANISTERS_NUM_DETECTORS[1],),
    (TEST_CANISTERS_NUM_DETECTORS[2],),
)

TEST_CANISTERS_SLICE = (
    {
        "source": slice(0, TEST_CANISTERS_NUM_SOURCES[0]),
        "detector": slice(0, TEST_CANISTERS_NUM_DETECTORS[0]),
    },
    {
        "source": slice(
            TEST_CANISTERS_NUM_SOURCES[0],
            TEST_CANISTERS_NUM_SOURCES[0] + TEST_CANISTERS_NUM_SOURCES[1],
        ),
        "detector": slice(
            TEST_CANISTERS_NUM_DETECTORS[0],
            TEST_CANISTERS_NUM_DETECTORS[0] + TEST_CANISTERS_NUM_DETECTORS[1],
        ),
    },
    {
        "source": slice(
            TEST_CANISTERS_NUM_SOURCES[0] + TEST_CANISTERS_NUM_SOURCES[1],
            TEST_CANISTERS_NUM_SOURCES[0]
            + TEST_CANISTERS_NUM_SOURCES[1]
            + TEST_CANISTERS_NUM_SOURCES[2],
        ),
        "detector": slice(
            TEST_CANISTERS_NUM_DETECTORS[0] + TEST_CANISTERS_NUM_DETECTORS[1],
            TEST_CANISTERS_NUM_DETECTORS[0]
            + TEST_CANISTERS_NUM_DETECTORS[1]
            + TEST_CANISTERS_NUM_DETECTORS[2],
        ),
    },
)

TEST_NUM_COORDINATES = (
    TEST_CANISTERS_COORDINATES[0]
    + TEST_CANISTERS_COORDINATES[1]
    + TEST_CANISTERS_COORDINATES[2]
)
TEST_NUM_SOURCES = (
    TEST_CANISTERS_NUM_SOURCES[0]
    + TEST_CANISTERS_NUM_SOURCES[1]
    + TEST_CANISTERS_NUM_SOURCES[2]
)
TEST_NUM_DETECTORS = (
    TEST_CANISTERS_NUM_DETECTORS[0]
    + TEST_CANISTERS_NUM_DETECTORS[1]
    + TEST_CANISTERS_NUM_DETECTORS[2]
)

SIMMONS_NAME = "simmons"
SIMMONS_CONFIG_VERSION = "1.1.0"
SIMMONS_CANISTERS = 1
SIMMONS_COORDINATES = 80
SIMMONS_SOURCES = 82
SIMMONS_DETECTORS = 39
SIMMONS_CANISTER_NAME = "MotorLeftDorsal"
SIMMONS_CANISTER_COORDINATES = 80
SIMMONS_CANISTER_SOURCES = 82
SIMMONS_CANISTER_DETECTORS = 39
SIMMONS_CANISTER_SLICE = {"source": slice(0, 82), "detector": slice(0, 39)}
SIMMONS_COORDINATES_SHAPE = (80, 3)
SIMMONS_SOURCE_GROUPS = 2
SIMMONS_SOURCE_GROUP_RED_TYPE = "Red"
SIMMONS_SOURCE_GROUP_RED_SHAPE = (41,)
SIMMONS_SOURCE_GROUP_RED_SOURCES = 41
SIMMONS_SOURCE_GROUP_RED_SLICE = {"source": slice(0, 41)}
SIMMONS_SOURCE_GROUP_INFRARED_TYPE = "Infrared"
SIMMONS_SOURCE_GROUP_INFRARED_SHAPE = (41,)
SIMMONS_SOURCE_GROUP_INFRARED_SOURCES = 41
SIMMONS_SOURCE_GROUP_INFRARED_SLICE = {"source": slice(41, 82)}
SIMMONS_DETECTORS_SHAPE = (39,)
SIMMONS_SOURCE_GROUP_RED_WAVELENGTH = 680.0
SIMMONS_SOURCE_GROUP_INFRARED_WAVELENGTH = 850.0


class SystemConfigurationTest(unittest.TestCase):
    def test_system_configuration(self) -> None:
        raw_configuration = YAMLConfig(package="testing", filename=TEST_FILENAME).tree

        system_configuration = SystemConfiguration(raw_configuration)

        self.assertEqual(system_configuration.name, TEST_NAME)
        self.assertEqual(system_configuration.config_version, TEST_CONFIG_VERSION)

        self.assertEqual(len(system_configuration.canisters), TEST_NUM_CANISTERS)
        self.assertEqual(system_configuration.num_coordinates, TEST_NUM_COORDINATES)
        self.assertEqual(system_configuration.num_sources, TEST_NUM_SOURCES)
        self.assertEqual(system_configuration.num_detectors, TEST_NUM_DETECTORS)

        # Test canisters
        for c, canister in enumerate(system_configuration.canisters):
            self.assertEqual(canister.name, TEST_CANISTERS_NAME[c])

            self.assertEqual(canister.wavelengths, TEST_CANISTERS_WAVELENGTHS[c])
            self.assertEqual(canister.num_coordinates, TEST_CANISTERS_COORDINATES[c])
            self.assertEqual(canister.num_sources, TEST_CANISTERS_NUM_SOURCES[c])
            self.assertEqual(canister.num_detectors, TEST_CANISTERS_NUM_DETECTORS[c])
            self.assertEqual(canister.slice, TEST_CANISTERS_SLICE[c])
            self.assertEqual(
                np.shape(canister.coordinates), TEST_CANISTERS_COORDINATES_SHAPE[c]
            )

            for x, coordinate in enumerate(
                raw_configuration["Canisters"][c]["Coordinates"]
            ):
                if c == 1:
                    # Canisters 0,1 has an affine transformation matrix
                    matrix = np.array(
                        raw_configuration["Canisters"][c]["Transformation"][
                            "AffineMatrix"
                        ]
                    )
                    coordinate_array = np.array(coordinate)
                    coordinate_array = np.append(coordinate_array, 1.0)

                    coordinate_array = np.matmul(matrix, coordinate_array)
                    coordinate_array[0:3] /= coordinate_array[3]
                    coordinate = coordinate_array[0:3]
                elif c == 2:
                    # Canister 2 has Euler angles, but they are equivalent to the following matrix
                    matrix = np.array(
                        [
                            [0.0, -1.0, 0.0, 100.0],
                            [1.0, 0.0, 0.0, 0.0],
                            [0.0, 0.0, 1.0, 0.0],
                            [0.0, 0.0, 0.0, 1.0],
                        ]
                    )

                    coordinate_array = np.array(coordinate)
                    coordinate_array = np.append(coordinate_array, 1.0)

                    coordinate_array = np.matmul(matrix, coordinate_array)
                    coordinate_array[0:3] /= coordinate_array[3]
                    coordinate = coordinate_array[0:3]
                for i in range(3):
                    self.assertAlmostEqual(canister.coordinates[x][i], coordinate[i])

            self.assertEqual(
                len(canister.source_groups), TEST_CANISTERS_NUM_SOURCE_GROUPS[c]
            )

            self.assertEqual(
                np.shape(canister.detectors), TEST_CANISTERS_DETECTORS_SHAPE[c]
            )

            for d, raw_detector in enumerate(
                raw_configuration["Canisters"][c]["Detectors"]
            ):
                self.assertEqual(canister.detectors[d], raw_detector["coordinate"])

            # Test source groups

            for g, (source_type, source_group) in enumerate(
                canister.source_groups.items()
            ):
                self.assertTrue(
                    TEST_CANISTERS_SOURCE_GROUPS_TYPE[c][g]
                    in canister.source_groups.keys()
                )
                self.assertEqual(
                    np.shape(source_group.sources),
                    TEST_CANISTERS_SOURCE_GROUPS_SHAPE[c][g],
                )
                self.assertEqual(
                    source_group.num_sources,
                    TEST_CANISTERS_SOURCE_GROUPS_NUM_SOURCES[c][g],
                )
                self.assertEqual(
                    source_group.slice, TEST_CANISTERS_SOURCE_GROUPS_SLICE[c][g]
                )
                self.assertEqual(
                    source_group.wavelength,
                    TEST_CANISTERS_SOURCE_GROUPS_WAVELENGTHS[c][g],
                )
                for s, source in enumerate(
                    raw_configuration["Canisters"][c]["Sources"][source_type]
                ):
                    self.assertEqual(source_group.sources[s], source["coordinate"])

    def test_simmons_system_configuration(self) -> None:
        raw_configuration = YAMLConfig(
            package="testing", filename=SIMMONS_MOTOR_LEFT_DORSAL_FILENAME
        ).tree

        system_configuration = SystemConfiguration(raw_configuration)

        self.assertEqual(system_configuration.name, SIMMONS_NAME)
        self.assertEqual(system_configuration.config_version, SIMMONS_CONFIG_VERSION)

        self.assertEqual(len(system_configuration.canisters), SIMMONS_CANISTERS)
        self.assertEqual(system_configuration.canisters[0].name, SIMMONS_CANISTER_NAME)
        self.assertEqual(system_configuration.num_coordinates, SIMMONS_COORDINATES)
        self.assertEqual(system_configuration.num_sources, SIMMONS_SOURCES)
        self.assertEqual(system_configuration.num_detectors, SIMMONS_DETECTORS)
        self.assertEqual(
            system_configuration.canisters[0].num_coordinates,
            SIMMONS_CANISTER_COORDINATES,
        )
        self.assertEqual(
            system_configuration.canisters[0].num_sources, SIMMONS_CANISTER_SOURCES,
        )
        self.assertEqual(
            system_configuration.canisters[0].num_detectors, SIMMONS_CANISTER_DETECTORS,
        )
        self.assertEqual(
            system_configuration.canisters[0].slice, SIMMONS_CANISTER_SLICE
        )

        self.assertEqual(
            np.shape(system_configuration.canisters[0].coordinates),
            SIMMONS_COORDINATES_SHAPE,
        )
        # NOTE: The motor left canister has a transformation,
        # so we need to account for that when checking coordinates
        matrix = np.array(
            raw_configuration["Canisters"][0]["Transformation"]["AffineMatrix"]
        )
        for c, coordinate in enumerate(
            raw_configuration["Canisters"][0]["Coordinates"]
        ):
            coordinate_array = np.array(coordinate)
            coordinate_array = np.append(coordinate_array, 1.0)
            coordinate_array = np.dot(matrix, coordinate_array)
            coordinate_array[0:3] /= coordinate_array[3]
            np.testing.assert_equal(
                np.array(system_configuration.canisters[0].coordinates[c]),
                coordinate_array[:3],
            )

        self.assertEqual(
            len(system_configuration.canisters[0].source_groups), SIMMONS_SOURCE_GROUPS,
        )

        self.assertTrue(
            SIMMONS_SOURCE_GROUP_RED_TYPE
            in system_configuration.canisters[0].source_groups.keys()
        )
        self.assertEqual(
            system_configuration.canisters[0].source_groups["Red"].wavelength,
            SIMMONS_SOURCE_GROUP_RED_WAVELENGTH,
        )
        self.assertEqual(
            np.shape(system_configuration.canisters[0].source_groups["Red"].sources),
            SIMMONS_SOURCE_GROUP_RED_SHAPE,
        )
        self.assertEqual(
            system_configuration.canisters[0].source_groups["Red"].num_sources,
            SIMMONS_SOURCE_GROUP_RED_SOURCES,
        )
        self.assertEqual(
            system_configuration.canisters[0].source_groups["Red"].slice,
            SIMMONS_SOURCE_GROUP_RED_SLICE,
        )
        for s, source in enumerate(raw_configuration["Canisters"][0]["Sources"]["Red"]):
            self.assertEqual(
                system_configuration.canisters[0].source_groups["Red"].sources[s],
                source["coordinate"],
            )

        self.assertTrue(
            SIMMONS_SOURCE_GROUP_INFRARED_TYPE
            in system_configuration.canisters[0].source_groups.keys()
        )
        self.assertEqual(
            system_configuration.canisters[0].source_groups["Infrared"].wavelength,
            SIMMONS_SOURCE_GROUP_INFRARED_WAVELENGTH,
        )
        self.assertEqual(
            np.shape(
                system_configuration.canisters[0].source_groups["Infrared"].sources
            ),
            SIMMONS_SOURCE_GROUP_INFRARED_SHAPE,
        )
        self.assertEqual(
            system_configuration.canisters[0].source_groups["Infrared"].num_sources,
            SIMMONS_SOURCE_GROUP_INFRARED_SOURCES,
        )
        self.assertEqual(
            system_configuration.canisters[0].source_groups["Infrared"].slice,
            SIMMONS_SOURCE_GROUP_INFRARED_SLICE,
        )
        for s, source in enumerate(
            raw_configuration["Canisters"][0]["Sources"]["Infrared"]
        ):
            self.assertEqual(
                system_configuration.canisters[0].source_groups["Infrared"].sources[s],
                source["coordinate"],
            )

        self.assertEqual(
            np.shape(system_configuration.canisters[0].detectors),
            SIMMONS_DETECTORS_SHAPE,
        )

        for d, detector in enumerate(raw_configuration["Canisters"][0]["Detectors"]):
            self.assertEqual(
                system_configuration.canisters[0].detectors[d], detector["coordinate"]
            )


if __name__ == "__main__":
    unittest.main()
