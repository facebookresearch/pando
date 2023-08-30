#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import abc
import logging
import time
from typing import NamedTuple, Optional, Type, Any, Dict, Union

import labgraph as df
import numpy as np
from bci.messages.generic_signal_sample import SignalSampleMessage
from bci.nodes.lts import LabeledTimeSeriesMessage
from bci.stimulus.psychopy.constants import (
    TrialMode,
    ViewChangeType,
)
from bci.stimulus.psychopy.messages import ViewChangeMessage
from bci.data.types.time_series import LabeledTimeSeries


class SignalViewMessage(df.TimestampedMessage):
    block_number: int
    trial_number: int
    samples_since_stimulus_onset: Optional[int] = None


class ArraySignalViewMessage(SignalViewMessage):
    sample: df.NumpyDynamicType()
    sample_timestamp: float = np.nan


class LabeledSignalViewMessage(SignalViewMessage):
    sample: LabeledTimeSeries


class SignalViewJoinerState(df.State):
    samples_since_stimulus_onset: Optional[int] = None
    block_number: Optional[int] = None
    trial_number: Optional[int] = None


class SignalViewJoiner(NamedTuple):
    input_signal_type: Type[df.TimestampedMessage]
    output_signal_type: Type[SignalViewMessage]
    node: df.Node


class BaseSignalViewJoinerNode(df.Node):
    """
    SignalViewJoinerNode starts outputting message upon
    seeing the first stimulus onset.

    Every new stimulus_onset resets the `samples_after_stimulus_onset`
    state variable.

    If a stimulus_onset event has not happened, `samples_after_stimulus_onset`
    value will be `None` for the streamed SignalViewMessage

    This node is useful for streaming with respect to stimulus onset.
    Implementations (subclasses) of this node deals with different node
    input message types. Important class functions to implement:

    - `join_signal()`: This function determines how a input message
        is converted into an appropriate SignalViewMessage
    """

    VIEW_CHANGE = df.Topic(ViewChangeMessage)
    state: SignalViewJoinerState

    @abc.abstractmethod
    async def join_signal(self, message: df.TimestampedMessage) -> df.AsyncPublisher:
        return

    @df.subscriber(VIEW_CHANGE)
    async def view_change(self, message: ViewChangeMessage) -> None:
        if (
            message.change_type == ViewChangeType.STIMULUS
            and message.trial_mode == TrialMode.PERFORMANCE
        ):
            logging.info("SignalViewJoiner: stimulus_onset view change")
            if (
                self.state.block_number is not None
                and self.state.trial_number is not None
                and (message.trial - self.state.trial_number) > 1
                and self.state.block_number == message.block
            ):
                logging.warning(
                    f"Trial incremented from {self.state.trial_number} "
                    f"to {message.trial} without performance view_change!"
                )
            self.state.block_number = message.block
            self.state.trial_number = message.trial
            self.state.samples_since_stimulus_onset = 0

    async def _join_signal(
        self,
        sample: Union[np.ndarray, LabeledSignalViewMessage],
        message_cls: Type[df.Message],
        message_kwargs: Dict[str, Any],
    ) -> df.AsyncPublisher:
        block_number = self.state.block_number
        trial_number = self.state.trial_number
        if trial_number is None or block_number is None:
            block_number = -1
            trial_number = -1
        elif self.state.samples_since_stimulus_onset is not None:
            self.state.samples_since_stimulus_onset += 1
        message = message_cls(
            block_number=block_number,
            trial_number=trial_number,
            timestamp=time.time(),
            sample=sample,
            samples_since_stimulus_onset=self.state.samples_since_stimulus_onset,
            **message_kwargs,
        )
        yield self.SIGNAL_OUTPUT, message


class ArraySignalViewJoinerNode(BaseSignalViewJoinerNode, df.Node):
    """
    This node is useful for streaming SignalSampleMessage
    inputs and labeling them with respect to stimulus onset
    """

    SIGNAL_INPUT = df.Topic(SignalSampleMessage)
    SIGNAL_OUTPUT = df.Topic(ArraySignalViewMessage)

    @df.subscriber(SIGNAL_INPUT)
    @df.publisher(SIGNAL_OUTPUT)
    async def join_signal(self, message: SignalSampleMessage) -> df.AsyncPublisher:
        async for retval in self._join_signal(
            message.sample,
            ArraySignalViewMessage,
            message_kwargs={"sample_timestamp": message.sample_timestamp},
        ):
            yield retval


class LabeledSignalViewJoinerNode(BaseSignalViewJoinerNode, df.Node):
    """
    This node is useful for streaming LabeledTimeSeriesMessage
    inputs and labeling them with respect to stimulus onset
    """

    SIGNAL_INPUT = df.Topic(LabeledTimeSeriesMessage)
    SIGNAL_OUTPUT = df.Topic(LabeledSignalViewMessage)

    @df.subscriber(SIGNAL_INPUT)
    @df.publisher(SIGNAL_OUTPUT)
    async def join_signal(self, message: LabeledTimeSeriesMessage) -> df.AsyncPublisher:
        async for retval in self._join_signal(
            message.sample, LabeledSignalViewMessage, {}
        ):
            yield retval


SIGNAL_VIEW_JOINERS = {
    SignalSampleMessage: SignalViewJoiner(
        SignalSampleMessage, ArraySignalViewMessage, ArraySignalViewJoinerNode
    ),
    LabeledTimeSeriesMessage: SignalViewJoiner(
        LabeledTimeSeriesMessage, LabeledSignalViewMessage, LabeledSignalViewJoinerNode
    ),
}
