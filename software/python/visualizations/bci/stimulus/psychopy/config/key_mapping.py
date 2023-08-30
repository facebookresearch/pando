#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from dataclasses import dataclass
from enum import Enum
from typing import Dict, Optional

from bci.stimulus.psychopy.constants import FingerTapEvent


# TODO: Use labgraph node configuration thingy
@dataclass
class KeyMapping:
    mapping: Dict[str, FingerTapEvent]

    def get(self, key: str) -> FingerTapEvent:
        try:
            return self.mapping[key]
        except KeyError:
            return FingerTapEvent.UNDEFINED

    def __str__(self) -> str:
        mapping_description = "Your key mapping is: \n"
        for (key, value) in self.mapping.items():
            mapping_description += f"  {key}: {value.name}\n"
        return mapping_description


class InputDevice(str, Enum):
    KEYBOARD = "KEYBOARD"
    PYKA = "PYKA"


def get_participant_key_mapping(participant_id, input_device=None) -> KeyMapping:
    if not input_device:
        input_device = InputDevice.KEYBOARD
    participant_id = participant_id.upper()
    return INPUT_DEVICE_KEY_MAPPING[input_device]["participant_mapping"].get(
        participant_id, INPUT_DEVICE_KEY_MAPPING[input_device]["default"]
    )


KEYBOARD_KEY_MAPPING = KeyMapping(
    {
        "q": FingerTapEvent.LEFT_PINKY,
        "2": FingerTapEvent.LEFT_RING,
        "3": FingerTapEvent.LEFT_MIDDLE,
        "4": FingerTapEvent.LEFT_INDEX,
        "v": FingerTapEvent.LEFT_THUMB,
        "b": FingerTapEvent.RIGHT_THUMB,
        "7": FingerTapEvent.RIGHT_INDEX,
        "8": FingerTapEvent.RIGHT_MIDDLE,
        "9": FingerTapEvent.RIGHT_RING,
        "p": FingerTapEvent.RIGHT_PINKY,
    }
)

ARL824_KEY_MAPPING = KeyMapping(
    {
        "q": FingerTapEvent.LEFT_PINKY,
        "3": FingerTapEvent.LEFT_RING,
        "4": FingerTapEvent.LEFT_MIDDLE,
        "5": FingerTapEvent.LEFT_INDEX,
        "v": FingerTapEvent.LEFT_THUMB,
        "b": FingerTapEvent.RIGHT_THUMB,
        "7": FingerTapEvent.RIGHT_INDEX,
        "8": FingerTapEvent.RIGHT_MIDDLE,
        "9": FingerTapEvent.RIGHT_RING,
        "p": FingerTapEvent.RIGHT_PINKY,
    }
)

BJH630_KEY_MAPPING = KeyMapping(
    {
        "q": FingerTapEvent.LEFT_PINKY,
        "2": FingerTapEvent.LEFT_RING,
        "3": FingerTapEvent.LEFT_MIDDLE,
        "4": FingerTapEvent.LEFT_INDEX,
        "v": FingerTapEvent.LEFT_THUMB,
        "n": FingerTapEvent.RIGHT_THUMB,
        "7": FingerTapEvent.RIGHT_INDEX,
        "8": FingerTapEvent.RIGHT_MIDDLE,
        "9": FingerTapEvent.RIGHT_RING,
        "p": FingerTapEvent.RIGHT_PINKY,
    }
)

CRP353_KEY_MAPPING = KeyMapping(
    {
        "q": FingerTapEvent.LEFT_PINKY,
        "2": FingerTapEvent.LEFT_RING,
        "3": FingerTapEvent.LEFT_MIDDLE,
        "4": FingerTapEvent.LEFT_INDEX,
        "v": FingerTapEvent.LEFT_THUMB,
        "n": FingerTapEvent.RIGHT_THUMB,
        "7": FingerTapEvent.RIGHT_INDEX,
        "8": FingerTapEvent.RIGHT_MIDDLE,
        "9": FingerTapEvent.RIGHT_RING,
        "p": FingerTapEvent.RIGHT_PINKY,
    }
)

EBS159_KEY_MAPPING = KeyMapping(
    {
        "1": FingerTapEvent.LEFT_PINKY,
        "2": FingerTapEvent.LEFT_RING,
        "3": FingerTapEvent.LEFT_MIDDLE,
        "4": FingerTapEvent.LEFT_INDEX,
        "b": FingerTapEvent.LEFT_THUMB,
        "n": FingerTapEvent.RIGHT_THUMB,
        "9": FingerTapEvent.RIGHT_INDEX,
        "0": FingerTapEvent.RIGHT_MIDDLE,
        "-": FingerTapEvent.RIGHT_RING,
        "=": FingerTapEvent.RIGHT_PINKY,
    }
)

UYH816_KEY_MAPPING = KeyMapping(
    {
        "q": FingerTapEvent.LEFT_PINKY,
        "3": FingerTapEvent.LEFT_RING,
        "4": FingerTapEvent.LEFT_MIDDLE,
        "5": FingerTapEvent.LEFT_INDEX,
        "v": FingerTapEvent.LEFT_THUMB,
        "b": FingerTapEvent.RIGHT_THUMB,
        "7": FingerTapEvent.RIGHT_INDEX,
        "8": FingerTapEvent.RIGHT_MIDDLE,
        "9": FingerTapEvent.RIGHT_RING,
        "p": FingerTapEvent.RIGHT_PINKY,
    }
)

XIH289_KEY_MAPPING = KeyMapping(
    {
        "q": FingerTapEvent.LEFT_PINKY,
        "3": FingerTapEvent.LEFT_RING,
        "4": FingerTapEvent.LEFT_MIDDLE,
        "5": FingerTapEvent.LEFT_INDEX,
        "b": FingerTapEvent.LEFT_THUMB,
        "n": FingerTapEvent.RIGHT_THUMB,
        "9": FingerTapEvent.RIGHT_INDEX,
        "0": FingerTapEvent.RIGHT_MIDDLE,
        "-": FingerTapEvent.RIGHT_RING,
        "]": FingerTapEvent.RIGHT_PINKY,
    }
)

PARTICIPANT_MAPPING = {
    "ARL824": ARL824_KEY_MAPPING,
    "BJH630": BJH630_KEY_MAPPING,
    "CRP353": CRP353_KEY_MAPPING,
    "EBS159": EBS159_KEY_MAPPING,
    "UYH816": UYH816_KEY_MAPPING,
    "XIH289": XIH289_KEY_MAPPING,
}

MRI_PYKA_KEY_MAPPING = KeyMapping(
    {
        # TODO
    }
)

USB_PYKA_KEY_MAPPING = KeyMapping(
    {
        "0": FingerTapEvent.LEFT_PINKY,
        "9": FingerTapEvent.LEFT_RING,
        "8": FingerTapEvent.LEFT_MIDDLE,
        "7": FingerTapEvent.LEFT_INDEX,
        "6": FingerTapEvent.LEFT_THUMB,
        "1": FingerTapEvent.RIGHT_THUMB,
        "2": FingerTapEvent.RIGHT_INDEX,
        "3": FingerTapEvent.RIGHT_MIDDLE,
        "4": FingerTapEvent.RIGHT_RING,
        "5": FingerTapEvent.RIGHT_PINKY,
    }
)

INPUT_DEVICE_KEY_MAPPING = {
    InputDevice.KEYBOARD: {
        "participant_mapping": PARTICIPANT_MAPPING,
        "default": KEYBOARD_KEY_MAPPING,
    },
    InputDevice.PYKA: {
        "participant_mapping": {},  # Empty for now
        "default": USB_PYKA_KEY_MAPPING,
    },
}
