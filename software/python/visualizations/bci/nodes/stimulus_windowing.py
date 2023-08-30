#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import abc
import logging
import time
from collections import deque
from enum import IntEnum
from functools import partial
from typing import List, Optional, Type, Tuple, NamedTuple

import labgraph as df
from bci.nodes.signal_view_joiner import (
    ArraySignalViewMessage,
    LabeledSignalViewMessage,
    SignalViewMessage,
)
from bci.nodes.window_trigger import make_window_trigger_node


"""
StimulusWindowNode take the SignalViewMessage (e.g. output by
the corresponding SignalWindowJoinerNode) and output
an entire window (list of SignalViewMessage) of a given length
post stimulus onset.
"""


class TriggerCommand(IntEnum):
    START_TRIGGER = 0
    STOP_TRIGGER = 1


class TriggerMessage(df.TimestampedMessage):
    trigger_command: TriggerCommand
    window_length: Optional[int] = None
    samples_since_stimulus_onset: Optional[int] = None

    def __post_init__(self):
        if self.trigger_command == TriggerCommand.START_TRIGGER:
            if self.window_length is None:
                raise ValueError("window_length cannot be None for start trigger")
            if self.samples_since_stimulus_onset is None:
                raise ValueError(
                    "samples_since_stimulus_onset cannot be None for start trigger"
                )


class StimulusWindowTriggerState(df.State):
    window_length: Optional[int] = None
    samples_since_stimulus_onset: Optional[int] = None
    window: Optional[deque] = None


class StimulusWindowTriggerConfig(df.Config):
    window_length: Optional[int] = None
    samples_since_stimulus_onset: Optional[int] = None


class ArrayStimulusWindowMessage(df.TimestampedMessage):
    sample: List[ArraySignalViewMessage]


class LabeledStimulusWindowMessage(df.TimestampedMessage):
    sample: List[LabeledSignalViewMessage]


class BaseStimulusWindowTriggerNode(df.Node):
    """
    By default StimulusWindowTriggerNode doesn't do anything.
    It monitors for TriggerMessage to start buffering
    input messages and return a list of successive messages
    (of length window_length), whose last message ends at
    samples_since_stimulus_onset w.r.t stimulus onset event

    TriggerMessage with STOP_TRIGGER command resets
    the node to zero output production.
    """

    TRIGGER_START = df.Topic(TriggerMessage)
    TERMINATION_TOPIC = df.Topic(df.TerminationMessage)

    config: StimulusWindowTriggerConfig
    state: StimulusWindowTriggerState

    def setup(self) -> None:
        """
        Non-default config will start the window trigger node
        running without needing to wait for trigger message
        """
        if (
            self.config.window_length is not None
            and self.config.samples_since_stimulus_onset is not None
        ):
            self._reset_stimulus_window(
                self.config.window_length, self.config.samples_since_stimulus_onset
            )

    def _reset_stimulus_window(
        self, window_length: int, samples_since_stimulus_onset: int
    ) -> None:
        if window_length <= 0 or samples_since_stimulus_onset <= 0:
            logging.warn(
                f"Received TriggerMessage with window_length={window_length}, "
                f"samples_since_stimulus_onset={samples_since_stimulus_onset}"
            )
        else:
            self.state.window_length = window_length
            self.state.samples_since_stimulus_onset = samples_since_stimulus_onset
            self.state.window = deque(maxlen=self.state.window_length)

    @abc.abstractmethod
    async def run(self, message: SignalViewMessage) -> df.AsyncPublisher:
        # call _run
        return

    @df.subscriber(TERMINATION_TOPIC)
    async def terminate(self, message: df.TerminationMessage) -> None:
        raise df.NormalTermination()

    @df.subscriber(TRIGGER_START)
    async def trigger_control(self, message: TriggerMessage) -> None:
        if message.trigger_command == TriggerCommand.STOP_TRIGGER:
            logging.info("Stopping stimulus window triggering, emptying current window")
            self.state.window_length = None
            self.state.samples_since_stimulus_onset = None
            self.state.window = None
        elif message.trigger_command == TriggerCommand.START_TRIGGER:
            logging.info("Starting stimulus window trigger")
            self._reset_stimulus_window(
                message.window_length, message.samples_since_stimulus_onset
            )

    def __window_is_full(self):
        if self.state.window is not None and self.state.window_length is not None:
            return len(self.state.window) == self.state.window_length

    async def _run(self, message_cls: Type[df.Message]):
        if self.__window_is_full():
            if (
                self.state.window[-1].samples_since_stimulus_onset
                == self.state.samples_since_stimulus_onset
            ):
                yield self.OUTPUT, message_cls(
                    timestamp=time.time(), sample=list(self.state.window)
                )


class ArrayStimulusWindowTriggerNode(BaseStimulusWindowTriggerNode, df.Node):
    INPUT = df.Topic(ArraySignalViewMessage)
    OUTPUT = df.Topic(ArrayStimulusWindowMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def run(self, message: ArraySignalViewMessage) -> df.AsyncPublisher:
        if self.state.window is not None:
            self.state.window.append(message)
            async for retval in self._run(ArrayStimulusWindowMessage):
                yield retval


class LabeledStimulusWindowTriggerNode(BaseStimulusWindowTriggerNode, df.Node):
    INPUT = df.Topic(LabeledSignalViewMessage)
    OUTPUT = df.Topic(LabeledStimulusWindowMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def run(self, message: LabeledSignalViewMessage) -> df.AsyncPublisher:
        if self.state.window is not None:
            self.state.window.append(message)
            async for retval in self._run(LabeledStimulusWindowMessage):
                yield retval
