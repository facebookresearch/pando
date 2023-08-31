#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import enum
from dataclasses import dataclass
from typing import Dict, List

from bci.stimulus.psychopy.constants import ViewChangeType


class StimType(enum.Enum):
    FIXATION = "fixation"
    IMAGE = "image"
    TEXT = "text"
    INCORRECT = "incorrect"


@dataclass
class StimInfo:
    type_: StimType
    attrs: Dict[str, str]


class DrawingManager:
    def __init__(self) -> None:
        self._to_draw = []
        self._is_dirty = False
        self.view_change_type = ViewChangeType.UNDEFINED

    @property
    def is_dirty(self) -> bool:
        return self._is_dirty

    @property
    def current_view(self) -> List[StimInfo]:
        self._is_dirty = False
        return self._to_draw

    def replace_view(
        self,
        stims: List[StimInfo],
        view_change_type: ViewChangeType = ViewChangeType.UNDEFINED,
    ) -> None:
        self._to_draw = stims
        self._is_dirty = True
        self.view_change_type = view_change_type

    def add_to_view(
        self,
        stims: List[StimInfo],
        view_change_type: ViewChangeType = ViewChangeType.UNDEFINED,
    ) -> None:
        self.view_change_type = view_change_type
        for stim in stims:
            if stim in self._to_draw:
                continue
            else:
                self._to_draw.append(stim)
                self._is_dirty = True

    def remove_from_view(
        self,
        stims: List[StimInfo],
        view_change_type: ViewChangeType = ViewChangeType.UNDEFINED,
    ) -> None:
        self.view_change_type = view_change_type
        new_to_draw = []
        for stim in self._to_draw:
            if stim in stims:
                self._is_dirty = True
                continue
            else:
                new_to_draw.append(stim)
        self._to_draw = new_to_draw
