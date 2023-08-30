#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from collections import defaultdict
from typing import Any, Dict, List, Optional, Tuple

import numpy as np
from bci.experiments.common.config import YAMLConfig


class MeasurementTransformer(object):
    """
    Class takes our (logical source, detector, wavelength) np.ndarray, where both
    sides of the cap are contained in a single sample, and selects only the
    measurements that are physical sources for a single source pattern,
    """

    physical_source_positions: np.ndarray
    physical_source_wavelengths: np.ndarray

    detector_positions: np.ndarray

    # only used to select physical source measurements from an expanded array
    # indexed by [logical source, detector, wavelength]
    detector_indices: np.ndarray
    physical_source_filter: np.ndarray
    num_physical_sources_per_wavelength: Dict[float, int]

    def __init__(
        self,
        num_logical_sources: int,
        num_detectors: int,
        pattern: "_PatternGroup",
        wave_index_map: Dict[float, int],
    ) -> None:
        self.num_logical_sources = num_logical_sources
        self.detector_positions = np.stack([d.coordinates for d in pattern.detectors])
        self._compute_pair_info(
            num_logical_sources, num_detectors, pattern, wave_index_map
        )

    @property
    def num_physical_sources(self) -> int:
        return self.physical_source_positions.shape[0]

    @property
    def num_detectors(self) -> int:
        return self.detector_positions.shape[0]

    def get_source_index_matrix(self) -> np.ndarray:
        """
        returns an array with the same return shape of transform_measurements
        indicating the source index of each measurement
        """
        source_indices = np.arange(self.physical_source_positions.shape[0])
        return np.broadcast_to(
            source_indices, (self.detector_positions.shape[0], *source_indices.shape)
        ).T

    def get_detector_index_matrix(self) -> np.ndarray:
        """
        returns an array with the same return shape of transform_measurements
        indicating the detector index of each measurement
        """
        detector_indices = np.arange(self.detector_positions.shape[0])
        return np.broadcast_to(
            detector_indices,
            (self.physical_source_positions.shape[0], *detector_indices.shape),
        )

    def sdw_to_pxd(self, measurements: np.ndarray) -> np.ndarray:
        """
        measurements will be of shape (logical source, detector, wavelength)
        the result will be of shape (physical source x detector)
        """
        assert measurements.ndim == 3
        assert measurements.shape[0] == self.physical_source_filter.shape[0]
        assert measurements.shape[1] == self.physical_source_filter.shape[1]
        assert measurements.shape[2] == self.physical_source_filter.shape[2]

        # physical_source_filter has shape (num_logical_sources, num_detectors, num_wavelengths)
        return measurements[self.physical_source_filter]

    def tsdw_to_tpxd(self, measurements: np.ndarray) -> np.ndarray:
        """
        measurements will be of shape (time, logical source, detector, wavelength)
        the result will be of shape (time, physical source x detector)
        """
        assert measurements.ndim == 4
        assert measurements.shape[1] == self.physical_source_filter.shape[0]
        assert measurements.shape[2] == self.physical_source_filter.shape[1]
        assert measurements.shape[3] == self.physical_source_filter.shape[2]

        # physical_source_filter has shape (num_logical_sources, num_detectors, num_wavelengths)
        return measurements[:, self.physical_source_filter]

    def sdw_to_pd(self, measurements: np.ndarray) -> np.ndarray:
        """
        measurements will be of shape (physical source x detector)
        the result will be of shape (physical source, detector)
        """

        measurements_pxd = self.sdw_to_pxd(measurements)
        # physical_source_positions has shape (num_physical_sources, num_wavelengths, 3)
        return measurements_pxd.reshape(
            self.physical_source_positions.shape[0], self.detector_positions.shape[0]
        )

    def tsdw_to_tpd(self, measurements: np.ndarray) -> np.ndarray:
        """
        measurements will be of shape (time, physical source x detector)
        the result will be of shape (time, physical source, detector)
        """

        measurements_tpxd = self.tsdw_to_tpxd(measurements)
        # physical_source_positions has shape (num_physical_sources, num_wavelengths, 3)
        return measurements_tpxd.reshape(
            -1,
            self.physical_source_positions.shape[0],
            self.detector_positions.shape[0],
        )

    def transform_measurements(self, measurements: np.ndarray) -> np.ndarray:
        """
        measurements will be of shape (logical source, detector, wavelength)
        the result will be of shape (physical source, detector)
        """
        return self.sdw_to_pd(measurements)

    def transform_batch(self, measurements: np.ndarray) -> np.ndarray:
        """
        input measurements of shape (time, step/source, detector, wavelength)
            which is the raw DOT data format at the moment
        output measurements of shape (time, source, detector), where source and
            detector here index MeasurementTransformer.physical_source_positions and
            MeasurementTransformer.detector_positions respectively
            for only a single canister
        """
        return self.tsdw_to_tpd(measurements)

    def sd_distances_pd(self) -> np.ndarray:
        """
        returns the source-detector pair distances in matrix form with the
        shape (physical source, detector)
        """
        sources_repeated = np.broadcast_to(
            self.physical_source_positions,
            (self.detector_positions.shape[0], *self.physical_source_positions.shape),
        )
        detectors_repeated = np.broadcast_to(
            self.detector_positions,
            (self.physical_source_positions.shape[0], *self.detector_positions.shape),
        )
        sd_squared_diff = (
            detectors_repeated - np.swapaxes(sources_repeated, 0, 1)
        ) ** 2
        return np.sqrt(np.sum(sd_squared_diff, axis=2))

    def pd_wavelength_split(self, pd_array: np.ndarray) -> Dict[float, np.ndarray]:
        """
        This function untangles physical sources by wavelength
        returns a dict with wavelengths as keys and arrays of physical source/detector values
        """
        result = {}
        for (
            wavelength,
            num_physical_sources,
        ) in self.num_physical_sources_per_wavelength.items():
            if num_physical_sources == 0:
                continue
            pd_shape = (num_physical_sources, pd_array.shape[1])
            if pd_array.dtype == np.bool:
                result[wavelength] = np.full(pd_shape, False, dtype=np.bool)
            else:
                result[wavelength] = np.full(pd_shape, np.nan, dtype=np.float64)
            p = 0
            filtered_p = 0
            for logical_source in self.pattern.logical_sources:
                for physical_source in logical_source.physical_sources:
                    if physical_source.wavelength == wavelength:
                        result[wavelength][filtered_p, :] = pd_array[p, :]
                        filtered_p += 1
                    p += 1
        return result

    def _compute_pair_info(
        self,
        num_logical_sources: int,
        num_detectors: int,
        pattern: "_PatternGroup",
        wave_index_map: Dict[float, int],
    ) -> None:
        self.num_wavelengths = len(wave_index_map)
        logical_source_positions = np.zeros(
            (num_logical_sources, self.num_wavelengths, 3)
        )
        wavelengths = np.zeros((num_logical_sources, self.num_wavelengths))
        full_shape = (num_logical_sources, num_detectors, self.num_wavelengths)
        is_physical_source = np.full(full_shape, False)

        self.num_physical_sources_per_wavelength = {}
        for wavelength, index in wave_index_map.items():
            self.num_physical_sources_per_wavelength[wavelength] = 0

        # this is reconstructing the format that we use when parsing the
        # data that is published by betazoid, where the first 2 dimensions
        # are wavelength index and logical source index
        for logical_source in pattern.logical_sources:
            for physical_source in logical_source.physical_sources:
                wave_idx = wave_index_map[physical_source.wavelength]
                logical_source_positions[
                    logical_source.index, wave_idx, :
                ] = physical_source.coordinates
                wavelengths[logical_source.index, wave_idx] = physical_source.wavelength
                # pattern.detector_indices contains all detector indices
                is_physical_source[
                    logical_source.index, pattern.detector_indices, wave_idx
                ] = True
                self.num_physical_sources_per_wavelength[
                    physical_source.wavelength
                ] += 1

        physical_sources = np.any(is_physical_source, axis=1)

        self.physical_source_positions = logical_source_positions[physical_sources]
        self.physical_source_wavelengths = wavelengths[physical_sources]
        self.physical_source_filter = is_physical_source
        self.pattern = pattern


class CombinedMeasurementTransformer(object):
    detector_positions: np.ndarray
    physical_source_positions: np.ndarray
    physical_source_wavelengths: np.ndarray

    # metadata for measurements returned by transform_batch()
    transformed_detector_indices: np.ndarray
    transformed_source_indices: np.ndarray
    transformed_pair_distances: np.ndarray

    def __init__(self, transformers: List[MeasurementTransformer]) -> None:
        self.pattern_transformers = transformers

        self.detector_positions = np.concatenate(
            [t.detector_positions for t in transformers]
        )
        self.physical_source_positions = np.concatenate(
            [t.physical_source_positions for t in transformers]
        )
        self.physical_source_wavelengths = np.concatenate(
            [t.physical_source_wavelengths for t in transformers]
        )
        self.transformed_source_indices = self._get_pair_source_indices()
        self.transformed_detector_indices = self._get_pair_detector_indices()
        self.transformed_pair_distances = self._get_pair_distances()

    def transform_measurements(self, data: np.ndarray) -> np.ndarray:
        """
        input: data of shape (source, detector, wavelength)
        output: data of shape (sd pairs for all wavelengths)
        """
        pattern_measurements = []
        for transformer in self.pattern_transformers:
            pattern_data = transformer.transform_measurements(data)
            pattern_measurements.append(pattern_data.reshape(-1))

        return np.concatenate(pattern_measurements, axis=0)

    def transform_batch(self, data: np.ndarray) -> np.ndarray:
        """
        input: data of shape (time, source, detector, wavelength)
        output: data of shape (time, sd pairs for all wavelengths)
        """
        pattern_measurements = []
        for transformer in self.pattern_transformers:
            pattern_data = transformer.transform_batch(data)
            pattern_measurements.append(pattern_data.reshape(data.shape[0], -1))

        return np.concatenate(pattern_measurements, axis=1)

    def _get_pair_source_indices(self) -> np.ndarray:
        """
        returns a 1-D array representing the source index for each pair
        represented by a column in the transformed data returned from
        transform_batch()

        transform_batch() returns an array of shape (time, pair)
        to get the source associated with pair i (i.e. column i), we would
        get the source index from get_pair_source_indices()[i], which
        would index physical_source_positions for the geometry.  i.e.
        the source x, y, z would be retrieved from
        physical_source_positions[get_pair_source_indices()[i]]
        """
        index_offset = 0
        source_indices = []
        for transformer in self.pattern_transformers:
            indices = transformer.get_source_index_matrix()
            source_indices.append(indices.ravel() + index_offset)
            index_offset += transformer.num_physical_sources
        return np.concatenate(source_indices)

    def _get_pair_detector_indices(self) -> np.ndarray:
        """
        returns 1-D array of source indices for the detectors, see
        get_pair_source_indices() for description of how to use, but replace
        the "source" with "detector" in the description
        """
        index_offset = 0
        detector_indices = []
        for transformer in self.pattern_transformers:
            indices = transformer.get_detector_index_matrix()
            detector_indices.append(indices.ravel() + index_offset)
            index_offset += transformer.num_detectors
        return np.concatenate(detector_indices)

    def _get_pair_distances(self) -> np.ndarray:
        """
        returns the source-detector pair distances in matrix form with the
        shape (canister, physical source x detector)
        """
        distances = []
        for transformer in self.pattern_transformers:
            distances.append(transformer.sd_distances_pd().ravel())
        return np.concatenate(distances)


class SourceDetectorMap(object):
    cfg_dict: Dict[str, Any]
    detectors: List["_Detector"]
    sources: List["_Source"]
    canisters: List["_Canister"]
    patterns: List["_PatternGroup"]
    num_steps: int
    wavelengths: List[float]
    source_types: Optional[List[str]] = None
    num_canisters: int
    num_logical_sources_per_canister: int
    num_detectors_per_canister: int

    def __init__(self, yaml_config: YAMLConfig, num_canisters: int = 2) -> None:
        """
        num_canisters represents how many simultaneous source patterns
        we have on at one time point.  If we have left and right canisters (sides of the
        cap) with sources on at the same time (whether it's 1 or 2 wavelengths),
        num_canisters should be 2.  If we have only a single sequence of
        sources (regardless of # of wavelengths again), where we would go from
        turning on lasers on one canister / side THEN the other side, not
        simultaneously, then we would have num_canisters=1, since we are
        basically treating the whole of the device as a single canister / sequence
        of sources
        """
        self.num_canisters = num_canisters
        self.yaml_config = yaml_config

        assert "Detectors" in self.yaml_config.tree
        assert "Sources" in self.yaml_config.tree
        assert "Steps" in self.yaml_config.tree
        assert "Wavelengths" in self.yaml_config.tree

        self.num_steps = self.yaml_config.tree["Steps"]
        self.wavelengths = self.yaml_config.tree["Wavelengths"]
        if "SourceTypes" in self.yaml_config.tree.keys():
            self.source_types = self.yaml_config.tree["SourceTypes"]
        if "Canisters" in self.yaml_config.tree.keys():
            self.canisters = self.yaml_config.tree["Canisters"]
            self.num_canisters = len(self.canisters)

        self._parse_source_detector_info()
        self._compute_pattern_source_groups()
        self._compute_source_detector_map()

    @property
    def num_detectors(self) -> int:
        return len(self.detectors)

    @property
    def num_logical_sources(self) -> int:
        return self.num_steps * len(self.patterns)

    @property
    def num_physical_sources(self) -> int:
        return len(self.sources)

    @property
    def num_patterns(self) -> int:
        return len(self.patterns)

    @property
    def num_wavelengths(self) -> int:
        return len(self.wavelengths)

    @property
    def num_source_types(self) -> int:
        if self.source_types is not None:
            return len(self.source_types)
        else:
            return 0

    @property
    def num_logical_sources_per_canister(self) -> int:
        return self.num_logical_sources // len(self.patterns)

    @property
    def num_detectors_per_canister(self) -> int:
        return len(self.detectors) // len(self.patterns)

    def sdw_to_csdw(self, sdw_array: np.ndarray) -> np.ndarray:
        # Warning: The CSDW array does not contain inter-canister SD pairs.
        assert sdw_array.ndim == 3
        num_logical_sources_per_canister = self.num_logical_sources // self.num_patterns
        num_detectors_per_canister = self.num_detectors // self.num_patterns
        csdw_shape = (
            self.num_patterns,
            num_logical_sources_per_canister,
            num_detectors_per_canister,
            self.num_wavelengths,
        )
        if sdw_array.dtype == np.bool:
            csdw_array = np.full(csdw_shape, False, dtype=np.bool)
        else:
            csdw_array = np.full(csdw_shape, np.nan, dtype=np.float64)
        for p, pattern in enumerate(self.patterns):
            # Slice pattern out of sdw data
            sources_start = p * num_logical_sources_per_canister
            detectors_start = p * num_detectors_per_canister
            csdw_array[p, :, :, :] = sdw_array[
                sources_start : sources_start + num_logical_sources_per_canister,
                detectors_start : detectors_start + num_detectors_per_canister,
                :,
            ]
        return csdw_array

    def csdw_to_cpd(self, csdw_array: np.ndarray) -> np.ndarray:
        """
        csdw_array will be of shape (canister, logical source, detector, wavelength)
        the result will be of shape (canister, physical source, detector)

        Warning: This function assumes that each canister has the same number of physical sources
        """
        transformer_0 = self.get_transformer_for_pattern(0)
        num_physical_sources_per_canister = transformer_0.physical_source_positions.shape[
            0
        ]

        num_logical_sources_per_canister = self.num_logical_sources // self.num_patterns
        num_detectors_per_canister = self.num_detectors // self.num_patterns

        assert csdw_array.ndim == 4
        assert csdw_array.shape[0] == self.num_patterns
        assert csdw_array.shape[1] == num_logical_sources_per_canister
        assert csdw_array.shape[2] == num_detectors_per_canister
        assert csdw_array.shape[3] == self.num_wavelengths

        cpd_array = np.full(
            (
                self.num_patterns,
                num_physical_sources_per_canister,
                num_detectors_per_canister,
            ),
            np.nan,
            dtype=np.float64,
        )

        wavelength_index_map = self.get_wavelength_index_map()

        for c, pattern in enumerate(self.patterns):
            p = 0
            for s, logical_source in enumerate(pattern.logical_sources):
                for physical_source in logical_source.physical_sources:
                    w = wavelength_index_map[physical_source.wavelength]
                    for d in range(num_detectors_per_canister):
                        cpd_array[c, p, d] = csdw_array[c, s, d, w]
                    p += 1

        return cpd_array

    def get_transformer_for_pattern(self, pattern_idx: int) -> MeasurementTransformer:
        return MeasurementTransformer(
            self.num_logical_sources,
            self.num_detectors,
            self.patterns[pattern_idx],
            self.get_wavelength_index_map(),
        )

    def get_combined_transformer(self) -> CombinedMeasurementTransformer:
        return CombinedMeasurementTransformer(
            [self.get_transformer_for_pattern(i) for i in range(self.num_patterns)]
        )

    def get_all_distances_sdw(self) -> np.ndarray:
        """
        Returns distances in the shape (source, detector, wavelength)
        Includes cross-canister source-detector distances
        """
        result = np.full(
            (self.num_logical_sources, self.num_detectors, self.num_wavelengths),
            np.nan,
            dtype=np.float64,
        )
        detector_positions = np.stack([d.coordinates for d in self.detectors])
        wavelength_map = self.get_wavelength_index_map()
        for pattern in self.patterns:
            for logical_source in pattern.logical_sources:
                for real_source in logical_source.physical_sources:
                    sum_squared_error = np.sum(
                        (detector_positions - real_source.coordinates) ** 2, axis=1
                    )
                    freq_idx = wavelength_map[real_source.wavelength]
                    result[logical_source.index, :, freq_idx] = np.sqrt(
                        sum_squared_error
                    )

        return result

    def get_source_coords_swr(self) -> np.ndarray:
        """
        Returns (logical source, wavelength, real world coordinates) shape
        """
        result = np.full(
            (self.num_logical_sources, self.num_wavelengths, 3),
            np.nan,
            dtype=np.float64,
        )
        wavelength_map = self.get_wavelength_index_map()
        for pattern in self.patterns:
            for logical_source in pattern.logical_sources:
                for real_source in logical_source.physical_sources:
                    freq_idx = wavelength_map[real_source.wavelength]
                    result[logical_source.index, freq_idx, :] = real_source.coordinates
        return result

    def get_detector_coords_dr(self) -> np.ndarray:
        """
        Returns (detectors, real world coordinates) shape
        """
        return np.stack([d.coordinates for d in self.detectors])

    def get_step_info(self, step: int) -> List[Tuple[int, List[int]]]:
        # returns the (source, detectors) associated with the given step
        # in each pattern defined in the config
        return [
            (pattern.logical_sources[step].index, pattern.detector_indices)
            for pattern in self.patterns
        ]

    def get_wavelength_index_map(self) -> Dict[float, int]:
        return {wavelength: idx for idx, wavelength in enumerate(self.wavelengths)}

    def _get_source_type_index_map(self) -> Dict[str, int]:
        return {source_type: idx for idx, source_type in enumerate(self.source_types)}

    def _parse_source_detector_info(self) -> None:
        # first parse the detector positions
        # NOTE: this assumes the detector indices are contiguous, i.e.
        # their indices are from 0 to N-1 without skipping any numbers
        self.detectors = np.stack(
            [
                _Detector(idx, detector_config)
                for idx, detector_config in self.yaml_config.tree["Detectors"].items()
            ]
        )
        # Parse source information
        self.sources = [
            _Source(index, source_config)
            for index, source_config in self.yaml_config.tree["Sources"].items()
            if source_config["step"] >= 0
        ]
        # NOTE: This assumes that each canister has the same number of sources
        assert len(self.sources) % self.num_steps == 0
        assert len(self.sources) % self.num_canisters == 0

    def _compute_pattern_source_groups(self) -> None:
        # assign the sources to their respective pattern
        # NOTE: this assumes the different patterns are defined contiguously
        # and are of equal length (number of steps)
        # This is a dangerous assumption! This should be mitigated by
        # introducing the concept of canisters explicitly in the mapping.
        # T46144955
        self.patterns = []
        # assumes len(sources) % num_steps == 0, which is checked in
        # _parse_source_detector_info
        sources_per_canister = len(self.sources) // self.num_canisters
        for i in range(self.num_canisters):
            # Slice out the sources for the pattern
            start_idx = sources_per_canister * i
            end_idx = start_idx + sources_per_canister
            pattern_sources = self.sources[start_idx:end_idx]
            self.patterns.append(
                _PatternGroup(index_offset=self.num_steps * i, sources=pattern_sources)
            )

    def _compute_source_detector_map(self) -> None:
        # here we want to end up with a map from step -> (source, detectors)

        # assign the detectors to pattern based on min distance to source
        detectors_by_pattern = [[] for _ in range(len(self.patterns))]
        for detector in self.detectors:
            min_idx = 0
            min_dist = self.patterns[0].min_distance(detector.coordinates)
            for i in range(1, len(self.patterns)):
                dist = self.patterns[i].min_distance(detector.coordinates)
                if dist < min_dist:
                    min_idx = i
                    min_dist = dist

            detectors_by_pattern[min_idx].append(detector)

        # add the detectors to the pattern definitions
        for idx, detector_list in enumerate(detectors_by_pattern):
            self.patterns[idx].set_detectors(detector_list)


class _Detector(object):
    canister: Optional[int] = None
    index: int
    coordinates: np.ndarray

    def __init__(self, idx: int, detector_config: Dict[str, float]) -> None:
        """
        Represents one detector entry in the betazoid yaml config
        """
        self.index = idx
        if "canister" in detector_config.keys():
            self.canister = detector_config["canister"]
        self.coordinates = _parse_coordinates(detector_config)


class _Source(object):
    step: int
    canister: Optional[int] = None
    wavelength: float
    coordinates: np.ndarray
    index: Optional[int] = None
    source_type: Optional[str] = None

    def __init__(self, index: int, source_config: Dict[str, Any]) -> None:
        """
        Represents one source entry in the betazoid yaml config
        """
        self.step = source_config["step"]
        if "canister" in source_config.keys():
            self.canister = source_config["canister"]
        self.wavelength = source_config["wavelength"]
        self.coordinates = _parse_coordinates(source_config)
        self.index = index
        if "source_type" in source_config.keys():
            self.source_type = source_config["source_type"]


class _Canister(object):
    index: int
    data_topic: str

    def __init__(self, idx: int, canister_config: Dict[str, float]) -> None:
        """
        Represents one canister entry in the betazoid yaml config
        """
        self.index = idx
        self.data_topic = canister_config["data_topic"]


class _LogicalSource(object):
    index: int
    step: int
    physical_sources: List[_Source]

    def __init__(self, index_offset: int, step: int) -> None:
        """
        A logical source represents a set of sources on the same side of the cap.
        The betazoid YAML doesn't have the notion of different sides of our cap,
        so here we separate out the sources by cap side.  _LogicalSource.index
        represents a unique index for each (step, cap side) pair, where step is
        defined by the betazoid config (representing one time point for the
        source light emission pattern).  In the case of a double raster, one
        step leads to an ambiguity about which side of the cap a given source
        belongs to, hence the need for this "logical" source index.
        """
        self.index = index_offset + step
        self.step = step
        self.physical_sources = []

    def add(self, source: _Source) -> None:
        self.physical_sources.append(source)


class _PatternGroup(object):
    sources: List[_Source]
    source_coordinates: np.ndarray

    detectors: List[_Detector]
    detector_indices: List[int]
    logical_sources: List[_LogicalSource]

    def __init__(self, index_offset: int, sources: List[_Source]) -> None:
        self.sources = []
        self.source_coordinates = None
        self.detectors = []
        self.detector_indices = []
        self.logical_sources = []
        # index_offset
        self.index_offset = index_offset

        # Iterate over all physical sources for the pattern and add them to sources and logical_sources
        for physical_source in sorted(sources, key=lambda x: x.step):
            self.add_source(physical_source)
        self.finalize()

    def add_source(self, source: _Source) -> None:
        self.sources.append(source)

        # Assumes that all physical sources for a logical source are contiguous
        if (
            len(self.logical_sources) == 0
            or source.step != self.logical_sources[-1].step
        ):
            # Append logical source
            # We should be going through the steps in order
            assert (
                len(self.logical_sources) == 0
                or source.step == self.logical_sources[-1].step + 1
            )
            self.logical_sources.append(_LogicalSource(self.index_offset, source.step))

        # Keep the association between logical and real sources
        # Add physical source to logical source
        self.logical_sources[-1].add(source)

    def finalize(self) -> None:
        # self.source_coordinates is just used for faster distance checking
        self.source_coordinates = np.stack(
            [source.coordinates for source in self.sources]
        )

    def min_distance(self, x: np.ndarray) -> float:
        # compute euclidean distance per source
        sum_squared_error = np.sum((self.source_coordinates - x) ** 2, axis=1)
        return np.sqrt(np.min(sum_squared_error))

    def set_detectors(self, detectors: List[_Detector]) -> None:
        self.detectors = detectors
        self.detector_indices = [d.index for d in detectors]


def _parse_coordinates(config: Dict[str, Any]) -> np.ndarray:
    return np.array([config["x"], config["y"], config["z"]])
