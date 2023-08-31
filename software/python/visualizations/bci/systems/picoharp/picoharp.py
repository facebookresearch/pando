#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
from typing import List
from labgraph.zmq_node import ZMQPollerConfig, ZMQPollerNode

from bci.systems.sawyer.messages import (
    CountRateMessage,
    CRIMessage,
    DTOFMessage,
    G2IMessage,
    G2Message,
)
from bci.systems.sawyer.serializers import (
    CountRateSerializer,
    CRISerializer,
    DTOFSerializer,
    G2ISerializer,
    G2Serializer,
)


ZMQ_TOPIC_COUNTS = "pando.counts"
ZMQ_TOPIC_CRI = "pando.cri"
ZMQ_TOPIC_DTOF = "pando.dtof"
ZMQ_TOPIC_G2 = "pando.g2"
ZMQ_TOPIC_G2I = "pando.g2i"


class PicoharpConfig(df.Config):
    """
    Config for Picoharp groups
    read_addr: ZMQ read port address
    """

    read_addr: str = "tcp://127.0.0.1:5060"


class PicoharpDCSGroup(df.Group):
    COUNT_RATES = df.Topic(CountRateMessage)
    G2 = df.Topic(G2Message)

    JULIET_COUNTS: ZMQPollerNode
    JULIET_G2: ZMQPollerNode

    COUNT_RATE_SERIALIZER: CountRateSerializer
    G2_SERIALIZER: G2Serializer

    config: PicoharpConfig

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


class PicoharpTDDCSGroup(df.Group):
    COUNT_RATES = df.Topic(CountRateMessage)
    CRI = df.Topic(CRIMessage)
    DTOF = df.Topic(DTOFMessage)
    G2I = df.Topic(G2IMessage)

    JULIET_COUNTS: ZMQPollerNode
    JULIET_CRI: ZMQPollerNode
    JULIET_DTOF: ZMQPollerNode
    JULIET_G2I: ZMQPollerNode
    COUNT_RATE_SERIALIZER: CountRateSerializer
    CRI_SERIALIZER: CRISerializer
    DTOF_SERIALIZER: DTOFSerializer
    G2I_SERIALIZER: G2ISerializer

    config: PicoharpConfig

    def setup(self) -> None:
        self.JULIET_COUNTS.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_COUNTS)
        )
        self.JULIET_CRI.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_CRI)
        )
        self.JULIET_DTOF.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_DTOF)
        )
        self.JULIET_G2I.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_G2I)
        )

    def connections(self) -> df.Connections:
        return (
            (self.JULIET_COUNTS.topic, self.COUNT_RATE_SERIALIZER.INPUT),
            (self.JULIET_CRI.topic, self.CRI_SERIALIZER.INPUT),
            (self.JULIET_DTOF.topic, self.DTOF_SERIALIZER.INPUT),
            (self.JULIET_G2I.topic, self.G2I_SERIALIZER.INPUT),
            (self.COUNT_RATE_SERIALIZER.OUTPUT, self.COUNT_RATES),
            (self.CRI_SERIALIZER.OUTPUT, self.CRI),
            (self.DTOF_SERIALIZER.OUTPUT, self.DTOF),
            (self.G2I_SERIALIZER.OUTPUT, self.G2I),
        )
