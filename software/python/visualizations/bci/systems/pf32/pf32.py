#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
from typing import Dict, Tuple

from bci.systems.sawyer.messages import CountRateMessage, G2Message
from bci.systems.sawyer.serializers import CountRateSerializer, G2Serializer
from labgraph.zmq_node import ZMQPollerConfig, ZMQPollerNode


ZMQ_TOPIC_COUNTS = "pando.counts"
ZMQ_TOPIC_G2 = "pando.g2"


class Pf32Config(df.Config):
    """
    Config for Pf32 groups
    read_addr: ZMQ read port address
    """

    read_addr: str = "tcp://127.0.0.1:5060"


class Pf32Group(df.Group):
    COUNT_RATES = df.Topic(CountRateMessage)
    G2 = df.Topic(G2Message)

    JULIET_G2: ZMQPollerNode
    JULIET_COUNTS: ZMQPollerNode
    COUNT_RATE_SERIALIZER: CountRateSerializer
    G2_SERIALIZER: G2Serializer

    config: Pf32Config

    def setup(self) -> None:
        self.JULIET_COUNTS.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_COUNTS)
        )
        self.JULIET_G2.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_G2)
        )

    def connections(self) -> df.Connections:
        return (
            (self.JULIET_COUNTS.topic, self.COUNT_RATE_SERIALIZER.INPUT),
            (self.JULIET_G2.topic, self.G2_SERIALIZER.INPUT),
            (self.COUNT_RATE_SERIALIZER.OUTPUT, self.COUNT_RATES),
            (self.G2_SERIALIZER.OUTPUT, self.G2),
        )

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (
            self.COUNT_RATE_SERIALIZER,
            self.G2_SERIALIZER,
            self.JULIET_G2,
            self.JULIET_COUNTS,
        )
