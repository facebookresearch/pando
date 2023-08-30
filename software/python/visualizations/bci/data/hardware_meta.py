#!/usr/bin/env python3
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

from typing import Any, Dict, List

from bci.experiments.common.dot.source_detector_map import SourceDetectorMap


DEFAULT_SOHO3_SERIAL = "Soho3.1"


class HardwareMetadata(object):
    """
    Represents the hardware specification for a given recording.  All of the
    data here is backed by a Gaia Hardware entry, and it handles some lazy
    retrieval of the YAML config spec used during data collection
    """

    metadata: Dict[str, object]

    def __init__(self, hw_ent: Dict[str, Any]) -> None:
        self.metadata = hw_ent
        self._source_detector_map = None

    @property
    def id(self) -> int:
        return self.metadata["id"]

    @property
    def tags(self) -> List[str]:
        return self.metadata["tags"]

    @property
    def serial(self) -> str:
        return self.metadata["serial"]

    @property
    def source_detector_map(self) -> SourceDetectorMap:
        if not self._source_detector_map:
            self._load_config(self._get_attachment_id(self.metadata))
        return self._source_detector_map

    def _get_attachment_id(self, hw_ent: Dict[str, Any]) -> int:
        first_key = next(iter(hw_ent["attachments"]))
        return int(first_key)
