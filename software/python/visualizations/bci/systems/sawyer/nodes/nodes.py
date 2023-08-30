#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import asyncio
import labgraph as df
import numpy as np
from typing import Optional

from ..messages import (
    Command,
    CountRateMessage,
    CRIMessage,
    DataMessage,
    DTOFMessage,
    DTOFVizMessage,
    G2IMessage,
    G2Message,
    HistogramMessage,
    TableStatsMessage,
    UpdateMessage,
)
from ..common.constants import COLUMN_TYPES


SCALE_FACTOR = 1e-3  # kcps
TAU_START = 9e-5
TAU_END = 2e-4


class CountRateEnsembleNode(df.Node):
    """
    Sum count rates from all channels
    Scale data to correct units
    Divide by number of pixels
    Convert to relative timebase
    """

    INPUT = df.Topic(CountRateMessage)
    OUTPUT = df.Topic(DataMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def count_rate_ensemble(self, message: CountRateMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, DataMessage(
            data=np.sum(message.count_rates) * SCALE_FACTOR / len(message.count_rates),
            timestamp=float(message.pando_timestamp) * 1e-9,
            command=message.command,
        )


class G2TauZeroEnsembleNode(df.Node):
    """
    Get the mean value of g2(tau_0) - 1
    Convert to relative timebase
    """

    INPUT = df.Topic(G2Message)
    OUTPUT = df.Topic(DataMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def g2_tau_zero_ensemble(self, message: G2Message) -> df.AsyncPublisher:
        yield self.OUTPUT, DataMessage(
            data=np.mean(message.g2[:, 0]),
            timestamp=float(message.pando_timestamp) * 1e-9,
            command=message.command,
        )


class G2ModulationEnsembleConfig(df.Config):
    """
    Configuration for a G2ModulationEnsembleNode
    tau_start: Start tau value for g2 modulation in ns
    tau_end: End tau value for g2 modulation in ns
    """

    tau_start: float = TAU_START
    tau_end: float = TAU_END


class G2ModulationEnsembleNode(df.Node):
    """
    Sum of g2_ensemble(tau_start:tau_end) - 1
    Convert to relative timebase
    """

    INPUT = df.Topic(G2Message)
    OUTPUT = df.Topic(DataMessage)

    config: G2ModulationEnsembleConfig

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def g2_modulation(self, message: G2Message) -> df.AsyncPublisher:
        start_idx = np.searchsorted(message.tau, self.config.tau_start)
        end_idx = np.searchsorted(message.tau, self.config.tau_end)

        # Get the relevant g2 columns between tau_start and tau_end
        # Take the mean accross channels for each value of tau (ensemble)
        # Sum those values

        yield self.OUTPUT, DataMessage(
            data=np.sum(np.mean(message.g2[:, start_idx:end_idx], axis=0)),
            timestamp=float(message.pando_timestamp) * 1e-9,
            command=message.command,
        )


class G2EnsembleNode(df.Node):
    """
    Publish G2 Ensemble Mean
    For each tau value take the mean of all g2 channels at that tau
    """

    INPUT = df.Topic(G2Message)
    G2_ENSEMBLE_MEAN = df.Topic(G2Message)

    @df.subscriber(INPUT)
    @df.publisher(G2_ENSEMBLE_MEAN)
    async def g2_ensemble(self, message: G2Message) -> df.AsyncPublisher:
        yield self.G2_ENSEMBLE_MEAN, G2Message(
            timestamp=message.timestamp,
            tau=message.tau,
            g2=np.mean(message.g2, axis=0),
            channel_map=message.channel_map,
            experiment_id=message.experiment_id,
            sequence_number=message.sequence_number,
            pando_timestamp=message.pando_timestamp,
            command=message.command,
        )


class G2StatsConfig(df.Config):
    """
    Configuration for a G2StatsNode
    N: Calculate g2 stats over N Tints
    """

    N: int = 1


class G2StatsState(df.State):
    """
    State for a G2StatsNode
    tau: The tau for the g2s
    g2s: The g2s
    """

    tau = None
    g2s = None


class G2StatsNode(df.Node):
    """
    Publish G2 Stats
    Mean, SNR of G2
    """

    G2 = df.Topic(G2Message)
    G2_STD = df.Topic(UpdateMessage)
    G2_SNR = df.Topic(UpdateMessage)

    config: G2StatsConfig
    state: G2StatsState

    @df.subscriber(G2)
    @df.publisher(G2_STD)
    @df.publisher(G2_SNR)
    async def g2_stats(self, message: G2Message) -> df.AsyncPublisher:
        if self.state.g2s is None or message.command == Command.CLEAR:
            self.state.g2s = np.array([message.g2])
            self.state.tau = None
            return
        else:
            self.state.g2s = np.append(self.state.g2s, [message.g2], axis=0)[
                -(self.config.N) :
            ]

        if self.state.tau is None:
            self.state.tau = message.tau

        mean = np.mean(self.state.g2s, axis=0)
        std = np.std(self.state.g2s, axis=0)

        with np.errstate(divide="ignore", invalid="ignore"):
            snr = np.nan_to_num(np.divide(mean, std))

        yield self.G2_STD, UpdateMessage(x=self.state.tau, y=std)
        yield self.G2_SNR, UpdateMessage(x=self.state.tau, y=snr)


class CRHistogramConfig(df.Config):
    """
    Configuration for a CRHistogramNode
    bins: The number of bins to histogram
    """

    bins: int


class CRHistogramNode(df.Node):
    """
    Create a histogram for count rates accross channels
    """

    INPUT = df.Topic(CountRateMessage)
    CR_HISTOGRAM = df.Topic(HistogramMessage)
    config: CRHistogramConfig

    @df.subscriber(INPUT)
    @df.publisher(CR_HISTOGRAM)
    async def cr_histogram(self, message: CountRateMessage) -> df.AsyncPublisher:
        y, x = np.histogram(message.count_rates, self.config.bins)
        yield self.CR_HISTOGRAM, HistogramMessage(x=x, y=y)


class DTOFVizState(df.State):
    """
    State for a DTOFVizNode
    range_min: past range_min for DTOF
    range_max: past range_max for DTOF
    resolution: past resolution for DTOF
    x: numpy array for domain values
    """

    range_min: Optional[int] = None
    range_max: Optional[int] = None
    resolution: Optional[int] = None
    x: Optional[np.ndarray] = None


class DTOFVizNode(df.Node):
    """
    Node to format DTOF data to be visualized
    """

    INPUT = df.Topic(DTOFMessage)
    OUTPUT = df.Topic(DTOFVizMessage)
    state: DTOFVizState

    n: int = 10

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def dtof_viz(self, message: DTOFMessage) -> df.AsyncPublisher:
        # Publish every 10 messages
        if self.n < 10:
            self.n += 1
            return
        self.n = 1

        if (
            message.range_min != self.state.range_min
            or message.range_max != self.state.range_max
            or message.resolution != self.state.resolution
        ):
            self.state.range_min = message.range_min
            self.state.range_max = message.range_max
            self.state.resolution = message.resolution
            self.state.x = np.arange(
                message.range_min, message.range_max + 1, message.resolution, np.int32,
            )
        yield self.OUTPUT, DTOFVizMessage(x=self.state.x, y=message.counts)


class TableStatsState(df.State):
    """
    State for TableStatsNode
    """

    channel_map: np.ndarray = np.array([])
    count_rates: np.ndarray = np.array([])
    cris: np.ndarray = np.array([])
    dtofs: np.ndarray = np.array([])
    data: np.ndarray = np.array([])

    range_min: Optional[int] = None
    range_max: Optional[int] = None
    resolution: Optional[int] = None
    x: Optional[np.ndarray] = None


class TableStatsNode(df.Node):
    """
    Calculate stats for the volga stats table
    """

    COUNT_RATES = df.Topic(CountRateMessage)
    CRI = df.Topic(CRIMessage)
    DTOF = df.Topic(DTOFMessage)

    TABLE_STATS = df.Topic(TableStatsMessage)

    state: TableStatsState

    def format_num(self, num) -> str:
        if num < 1000:
            return str(num)
        if num < 1000000:
            return str(round(num / 1000, 3)) + "k"
        return str(round(num / 1000000, 3)) + "M"

    @df.subscriber(COUNT_RATES)
    def count_rate(self, message: CountRateMessage) -> None:
        """
        Receive count rate data
        """
        self.state.channel_map = message.channel_map
        self.state.count_rates = message.count_rates

    @df.subscriber(CRI)
    def cri(self, message: CRIMessage) -> None:
        """
        Receive cri data
        """
        self.state.cris = message.count_rates

    @df.subscriber(DTOF)
    def dtof(self, message: DTOFMessage) -> None:
        """
        Receive dtof data
        """
        # Counts
        self.state.dtofs = message.counts
        # Generate x values
        if (
            message.range_min != self.state.range_min
            or message.range_max != self.state.range_max
            or message.resolution != self.state.resolution
        ):
            self.state.range_min = message.range_min
            self.state.range_max = message.range_max
            self.state.resolution = message.resolution
            self.state.x = np.arange(
                message.range_min, message.range_max + 1, message.resolution, np.int32,
            )

    @df.publisher(TABLE_STATS)
    async def background(self) -> df.AsyncPublisher:
        """
        Refresh the table with the latest data available
        """
        while True:
            if (
                len(self.state.count_rates) > 0
                and len(self.state.cris) > 0
                and len(self.state.dtofs) > 0
            ):
                # Generate Time of Flight for max value for each channel
                peak_idx = np.argmax(self.state.dtofs, axis=1)
                peak_tof = np.array([self.state.x[i] for i in peak_idx])

                # Get Time of Flight for 10% of max value for each channel
                ten_percent_peak = np.max(self.state.dtofs, axis=1) * 0.1
                ten_percent_peak_idx = np.array(
                    [
                        np.abs(self.state.dtofs[i] - p).argmin()
                        for i, p in enumerate(ten_percent_peak)
                    ]
                )
                ten_percent_tof = np.array(
                    [self.state.x[i] for i in ten_percent_peak_idx]
                )

                # Get Time of Flights for all values > 10% of max
                tof_filter = []
                for i, row in enumerate(self.state.dtofs):
                    t = []
                    for j, r in enumerate(row):
                        if r > ten_percent_peak[i]:
                            t.append(self.state.x[j])
                    tof_filter.append(np.array(t))

                # Mean and STD of TOF > 10% of max
                tof_mean = np.array(
                    [row.mean() if len(row) > 0 else 0 for row in tof_filter]
                )
                tof_std = np.array(
                    [row.std() if len(row) > 0 else 0 for row in tof_filter]
                )

                count_rates = np.append(
                    self.state.count_rates,
                    [np.sum(self.state.count_rates), np.std(self.state.count_rates),],
                )
                count_rates_formatted = np.array(
                    [self.format_num(count) for count in count_rates]
                )

                cris = np.append(
                    self.state.cris, [np.sum(self.state.cris), np.std(self.state.cris)],
                )
                cris_formatted = np.array([self.format_num(count) for count in cris])

                data = np.array(
                    list(
                        zip(
                            np.append(
                                self.state.channel_map.astype(str), ["Total", "Std"]
                            ),
                            count_rates_formatted,
                            cris_formatted,
                            np.append(tof_mean, [np.sum(tof_mean), np.std(tof_mean)]),
                            np.append(tof_std, [np.sum(tof_std), np.std(tof_std)]),
                            np.append(peak_tof, [np.sum(peak_tof), np.std(peak_tof)]),
                            np.append(
                                ten_percent_tof,
                                [np.sum(ten_percent_tof), np.std(ten_percent_tof)],
                            ),
                        )
                    ),
                    dtype=COLUMN_TYPES,
                )

                yield self.TABLE_STATS, TableStatsMessage(data=data)

            await asyncio.sleep(0.5)


class G2IToG2Node(df.Node):
    """
    Transform a G2IMessage to a G2Message
    """

    INPUT = df.Topic(G2IMessage)
    OUTPUT = df.Topic(G2Message)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def g2i_to_g2(self, message: G2IMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, G2Message(
            timestamp=message.timestamp,
            tau=message.tau,
            g2=message.g2,
            channel_map=message.channel_map,
            experiment_id=message.experiment_id,
            sequence_number=message.sequence_number,
            pando_timestamp=message.pando_timestamp,
        )


class TaucorrStatsConfig(df.Config):
    """
    Configuration for a TaucorrStatsNode
    N: Calculate taucorr stats over N Tints
    """

    N: int = 1


class TaucorrStatsState(df.State):
    """
    State for a TaucorrStatsNode
    plot: The pyqt plot that we are updating
    """

    first_timestamp = None
    taucorrs = None


class TaucorrStatsNode(df.Node):
    """
    Publish Taucorr Stats
    Mean, SNR of G2
    """

    TAUCORR = df.Topic(DataMessage)
    TAUCORR_SNR = df.Topic(DataMessage)

    config: TaucorrStatsConfig
    state: TaucorrStatsState

    @df.subscriber(TAUCORR)
    @df.publisher(TAUCORR_SNR)
    async def taucorr_stats(self, message: DataMessage) -> df.AsyncPublisher:
        if self.state.taucorrs is None:
            self.state.taucorrs = np.array([message.data])
            self.state.first_timestamp = message.timestamp
            return

        self.state.taucorrs = np.append(self.state.taucorrs, message.data)[
            -(self.config.N) :
        ]

        mean = np.mean(self.state.taucorrs)
        std = np.std(self.state.taucorrs)

        with np.errstate(divide="ignore", invalid="ignore"):
            snr = np.nan_to_num(np.divide(mean, std))

        yield self.TAUCORR_SNR, DataMessage(
            timestamp=message.timestamp - self.state.first_timestamp, data=snr
        )


class DownsampleConfig(df.Config):
    """
    Configuration for a DownsampleNode
    N: Publish every n samples
    """

    N: int = 1


class DownsampleState(df.State):
    """
    State for a DownsampleNode
    k: Current sample number
    """

    k: int = 0


class DownsampleNode(df.Node):
    """
    Publish every N samples
    """

    INPUT = df.Topic(CountRateMessage)
    OUTPUT = df.Topic(CountRateMessage)

    config: DownsampleConfig
    state: DownsampleState

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def taucorr_stats(self, message: CountRateMessage) -> df.AsyncPublisher:
        self.state.k += 1
        if self.state.k == self.config.N:
            self.state.k = 0
            yield self.OUTPUT, message
        return
