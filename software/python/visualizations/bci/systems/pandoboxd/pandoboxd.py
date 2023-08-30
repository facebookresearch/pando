#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Tuple

import labgraph as df
from bci.systems.sawyer.messages import (
    DigitalInMessage,
    HRMMessage,
    IMUMessage,
    PowerMeterMessage,
    PulseOxMessage,
    RespBeltMessage,
)
from bci.systems.sawyer.serializers import (
    DigitalInSerializer,
    HRMSerializer,
    IMUSerializer,
    PowerMeterSerializer,
    PulseOxSerializer,
    RespBeltSerializer,
)
from labgraph.zmq_node import ZMQPollerConfig, ZMQPollerNode


ZMQ_TOPIC_DIGITAL_IN = "pandoboxd.digital_in"
ZMQ_TOPIC_HRM = "pandoboxd.hrm"
ZMQ_TOPIC_IMU = "pandoboxd.imu"
ZMQ_TOPIC_PULSE_OX = "pandoboxd.pulse_ox"
ZMQ_TOPIC_RESP_BELT = "pandoboxd.resp_belt"
ZMQ_TOPIC_POWER_METER = "pandoboxd.power_meter"


class PandoboxdConfig(df.Config):
    """
    Config for Pandoboxd groups
    read_addr: ZMQ read port address
    """

    read_addr: str = "tcp://127.0.0.1:5060"


class PandoboxdGroup(df.Group):
    DIGITAL_IN = df.Topic(DigitalInMessage)
    HRM = df.Topic(HRMMessage)
    IMU = df.Topic(IMUMessage)
    PULSE_OX = df.Topic(PulseOxMessage)
    RESP_BELT = df.Topic(RespBeltMessage)
    POWER_METER = df.Topic(PowerMeterMessage)

    JULIET_DIGITAL_IN: ZMQPollerNode
    JULIET_HRM: ZMQPollerNode
    JULIET_IMU: ZMQPollerNode
    JULIET_PULSE_OX: ZMQPollerNode
    JULIET_RESP_BELT: ZMQPollerNode
    JULIET_POWER_METER: ZMQPollerNode
    DIGITAL_IN_SERIALIZER: DigitalInSerializer
    HRM_SERIALIZER: HRMSerializer
    IMU_SERIALIZER: IMUSerializer
    POWER_METER_SERIALIZER: PowerMeterSerializer
    PULSE_OX_SERIALIZER: PulseOxSerializer
    RESP_BELT_SERIALIZER: RespBeltSerializer

    config: PandoboxdConfig

    def setup(self) -> None:
        self.JULIET_DIGITAL_IN.configure(
            ZMQPollerConfig(
                read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_DIGITAL_IN
            )
        )
        self.JULIET_HRM.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_HRM)
        )
        self.JULIET_IMU.configure(
            ZMQPollerConfig(read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_IMU)
        )
        self.JULIET_PULSE_OX.configure(
            ZMQPollerConfig(
                read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_PULSE_OX
            )
        )
        self.JULIET_RESP_BELT.configure(
            ZMQPollerConfig(
                read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_RESP_BELT
            )
        )
        self.JULIET_POWER_METER.configure(
            ZMQPollerConfig(
                read_addr=self.config.read_addr, zmq_topic=ZMQ_TOPIC_POWER_METER
            )
        )

    def connections(self) -> df.Connections:
        return (
            (self.JULIET_DIGITAL_IN.topic, self.DIGITAL_IN_SERIALIZER.INPUT),
            (self.JULIET_HRM.topic, self.HRM_SERIALIZER.INPUT),
            (self.JULIET_IMU.topic, self.IMU_SERIALIZER.INPUT),
            (self.JULIET_PULSE_OX.topic, self.PULSE_OX_SERIALIZER.INPUT),
            (self.JULIET_RESP_BELT.topic, self.RESP_BELT_SERIALIZER.INPUT),
            (self.JULIET_POWER_METER.topic, self.POWER_METER_SERIALIZER.INPUT),
            (self.DIGITAL_IN_SERIALIZER.OUTPUT, self.DIGITAL_IN),
            (self.HRM_SERIALIZER.OUTPUT, self.HRM),
            (self.IMU_SERIALIZER.OUTPUT, self.IMU),
            (self.PULSE_OX_SERIALIZER.OUTPUT, self.PULSE_OX),
            (self.RESP_BELT_SERIALIZER.OUTPUT, self.RESP_BELT),
            (self.POWER_METER_SERIALIZER.OUTPUT, self.POWER_METER),
        )
