# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List

import labgraph as df
from bci.nodes.lts import LabeledTimeSeriesMessage
from xplat.frl.bci.heart_beat import HeartBeatMetric, WallTimeLog, heart_beat_metric


class HeartBeatMetricMessage(df.TimestampedMessage):
    sample: HeartBeatMetric
    log: List[WallTimeLog]


class HeartBeatMetricConfig(df.Config):
    confidence: float = 3.0

    def __post_init__(self):
        super().__post_init__()

        if self.confidence <= 0:
            raise ValueError("Confidence for heart beat metric must be > 0")


class HeartBeatMetricNode(df.Node):
    INPUT = df.Topic(LabeledTimeSeriesMessage)
    OUTPUT = df.Topic(HeartBeatMetricMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def run_heart_beat_analysis(
        self, message: LabeledTimeSeriesMessage
    ) -> df.AsyncPublisher:
        analysis, log = heart_beat_metric(
            message.sample, confidence=self.config.confidence
        )
        yield self.OUTPUT, HeartBeatMetricMessage(
            timestamp=message.timestamp, sample=analysis, log=log
        )
