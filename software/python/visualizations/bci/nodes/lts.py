# Copyright 2004-present Facebook. All Rights Reserved.

import time
from dataclasses import field
from typing import List

import labgraph as df
import numpy as np
from bci.messages.generic_signal_sample import SignalSampleMessage
from bci.systems.configuration.system_configuration import SystemConfiguration
from bci.data.types.channel_info import DOTChannelInfo
from bci.data.types.time_series import LabeledTimeSeries
from bci.transform.channel_info import system_configuration_to_channel_info


class LabeledTimeSeriesMessage(df.TimestampedMessage):
    # timestamp field here is consistent with the meaning
    # of other TimestampedMessage -- time at which the
    # message is generated/sent.
    sample: LabeledTimeSeries


class LTStoSignalSampleNodeConfig(df.Config):
    channel_info: DOTChannelInfo = None


class LTStoSignalSampleNode(df.Node):
    INPUT = df.Topic(LabeledTimeSeriesMessage)
    OUTPUT = df.Topic(SignalSampleMessage)
    config: LTStoSignalSampleNodeConfig

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def convert(self, lts_message: LabeledTimeSeriesMessage) -> df.AsyncPublisher:
        if self.config.channel_info != lts_message.sample.channel_info:
            raise RuntimeError("LabeledTimeSeriesMessage channel_info has changed!")
        yield self.OUTPUT, SignalSampleMessage(
            timestamp=time.time(),
            sample=lts_message.sample.channel_data.flatten(),
            sample_timestamp=lts_message.timestamp,
        )


class LabeledTimeSeriesState(df.State):
    messages: List[SignalSampleMessage] = field(default_factory=list)


class LabeledTimeSeriesConfig(df.Config):
    length: int  # unit: Number of samples
    stride: int  # unit: Number of samples
    sysconfig: SystemConfiguration  # Needed to convert the data into a LTS

    def __post_init__(self):
        super().__post_init__()

        if self.length < 1:
            raise ValueError("The window length must be >= 1.")

        if self.stride < 1:
            raise ValueError("The stride length must be >= 1.")


class LabeledTimeSeriesNode(df.Node):
    """
    Converts n messages from a SignalSampleMessage source (Simmons,
    Lumo) into a LabeledTimeSeries.

    This node is parameterized by two parameters:

    - length: how many SimmonsAdcCanonicalMessage's to combine into a
      LabeledTimeSeries.
    - stride: how many samples to skip before taking another length of samples.

    For example, to make 64 messages into a LabeledTimeSeries with an overlap
    of 50% between LabeledTimeSeriesMessage's, one would use a length of 64 and
    a stride of 32.

    Graphically, a length of 9 with a stride of 4 would look like

        |---2---||---4---||---6---|      LabeledTimeSeriesMessage (each bar is one
    |---1---||---3---||---5---||---7---| message, covering the bar's length of data)
    .................................... SimmonsAdcCanonicalMessage (one per dot)

    -> time
    """

    INPUT = df.Topic(SignalSampleMessage)
    OUTPUT = df.Topic(LabeledTimeSeriesMessage)

    state: LabeledTimeSeriesState
    config: LabeledTimeSeriesConfig

    def setup(self):
        self._channel_info = system_configuration_to_channel_info(self.config.sysconfig)

    def convert_to_lts(self, samples: List[SignalSampleMessage]) -> LabeledTimeSeries:
        if not np.isnan(samples[0].sample_timestamp):
            timestamps = np.array([s.sample_timestamp for s in samples])
        else:
            timestamps = np.array([s.timestamp for s in samples])
        channel_data = np.stack([s.sample for s in samples])

        lts = LabeledTimeSeries(timestamps, channel_data, self._channel_info)
        return lts

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def window(self, message: SignalSampleMessage) -> df.AsyncPublisher:
        self.state.messages.append(message)
        if len(self.state.messages) == self.config.length:
            window_lts = self.convert_to_lts(self.state.messages)
            timestamp = time.time()
            msg = LabeledTimeSeriesMessage(timestamp=timestamp, sample=window_lts)

            yield self.OUTPUT, msg

            self.state.messages = self.state.messages[self.config.stride :]
