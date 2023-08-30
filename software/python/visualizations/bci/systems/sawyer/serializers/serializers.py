#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.
import logging
import time
import sys
import os
import labgraph as df
import numpy as np

from arvr import pando_pb2
from ..messages import (
    CountRateMessage,
    CRIMessage,
    DigitalInMessage,
    DTOFMessage,
    G2IMessage,
    G2Message,
    HRMMessage,
    IMUMessage,
    PowerMeterMessage,
    PulseOxMessage,
    RespBeltMessage,
)

if "CAM_MODE" in os.environ and os.environ["CAM_MODE"] == "basler":
    logging.info("[Serializer] importing BaslerImageMessage as CameraImageMessage")
    from ..messages import BaslerImageMessage as CameraImageMessage
else:
    logging.info("[Serializer] importing HamamatsuImageMessage as CameraImageMessage")
    from ..messages import HamamatsuImageMessage as CameraImageMessage

from labgraph.zmq_node import ZMQMessage
from bci.systems.sawyer.messages.messages import Command
from bci.nodes.profiler_node import ProfileNode


ZMQ_PORT = 5060
ZMQ_ADDR = "tcp://127.0.0.1:%d" % (ZMQ_PORT,)


class PandoSerializerState(df.State):
    """
    State for a Pando Serializer
    experiment_id: The current experiment id
    """

    experiment_id = None


class PandoSerializer(ProfileNode):
    state: PandoSerializerState

    def generate_command(self, experiment_id: int) -> bool:
        if self.state.experiment_id is None:
            self.state.experiment_id = experiment_id
            return Command.NONE
        if self.state.experiment_id != experiment_id:
            self.state.experiment_id = experiment_id
            return Command.CLEAR
        return Command.NONE


class CountRateSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(CountRateMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def counts(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> CountRateMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        if not packet_pb.payload.HasField("counts"):
            raise df.LabgraphError("packet_pb missing counts field")
        counts_pb = packet_pb.payload.counts
        num_channels = len(counts_pb.channels)
        channel_map = np.zeros(num_channels, dtype=np.int64)
        count_rates = np.zeros(num_channels, dtype=np.float64)
        channels = list(counts_pb.channels)
        channels.sort()

        # period given in nanoseconds, we want counts/second
        for i, channel in enumerate(channels):
            channel_map[i] = channel
            count_rates[i] = counts_pb.channels[channel].count / (
                counts_pb.meta.integration_period_ns * 1e-9  # ns to s
            )
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return CountRateMessage(
            timestamp=time.time(),
            channel_map=channel_map,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            count_rates=count_rates,
            command=command,
        )


class CRISerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(CRIMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def cri(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> CRIMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("cri")
        cri_pb = packet_pb.payload.cri

        num_channels = len(cri_pb.channels)
        header = packet_pb.header

        channel_map = np.zeros(num_channels, dtype=np.int64)
        count_rates = np.zeros(num_channels)

        channels = list(cri_pb.channels)
        channels.sort()
        first_channel = channels[0]

        # There are configured together in Pando and will always
        # be the same
        utime_from_ps = cri_pb.channels[first_channel].utime_from_ps
        utime_until_ps = cri_pb.channels[first_channel].utime_until_ps

        for i, channel in enumerate(channels):
            channel_map[i] = channel
            count_rates[i] = cri_pb.channels[channel].count / (
                cri_pb.meta.integration_period_ns * 1e-9  # ns to s
            )

        command = self.generate_command(header.experiment_id)

        return CRIMessage(
            timestamp=time.time(),
            channel_map=channel_map,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            utime_from_ps=utime_from_ps,
            utime_until_ps=utime_until_ps,
            count_rates=count_rates,
            command=command,
        )


class DigitalInSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(DigitalInMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def digital_in(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> DigitalInMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("digital_in")
        digital_in_pb = packet_pb.payload.digital_in
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return DigitalInMessage(
            timestamp=time.time(),
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            device_id=digital_in_pb.device_id,
            edge=digital_in_pb.edge,
            command=command,
        )


class DTOFSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(DTOFMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def dtof(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> DTOFMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        if not packet_pb.payload.HasField("dtof"):
            raise df.LabgraphError("packet_pb missing dtof field")
        dtof_pb = packet_pb.payload.dtof

        header = packet_pb.header

        channels = list(dtof_pb.channels)
        channels.sort()
        first_channel = channels[0]

        resolution = dtof_pb.meta.resolution
        range_min = dtof_pb.meta.range_min
        range_max = dtof_pb.meta.range_max
        integration_period_ns = dtof_pb.meta.integration_period_ns

        channel_map = np.array(channels, dtype=np.int64)
        counts = np.array(
            [dtof_pb.channels[channel].counts for channel in channels], dtype=np.int32
        )

        command = self.generate_command(header.experiment_id)

        return DTOFMessage(
            timestamp=time.time(),
            channel_map=channel_map,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            resolution=resolution,
            range_min=range_min,
            range_max=range_max,
            integration_period_ns=integration_period_ns,
            counts=counts,
            command=command,
        )


class CameraImageSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(CameraImageMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def camera_image(self, message: ZMQMessage) -> df.AsyncPublisher:
        self.increment_message_processed()
        yield self.OUTPUT, self.serialize_message(message)
        self.time_since_last_update()

    def serialize_message(self, message: ZMQMessage) -> CameraImageMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        if not packet_pb.payload.HasField("camera_image"):
            raise df.LabgraphError("packet_pb missing camera_image field")
        camera_image_pb = packet_pb.payload.camera_image

        header = packet_pb.header

        return CameraImageMessage(
            timestamp=time.time(),
            device_id=camera_image_pb.device_id,
            width=camera_image_pb.width,
            height=camera_image_pb.height,
            bytes_per_pixel=camera_image_pb.bytes_per_pixel,
            image_data=camera_image_pb.image_data,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
        )


class G2Serializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(G2Message)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def g2(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> G2Message:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        if not packet_pb.payload.HasField("g2"):
            raise df.LabgraphError("packet_pb missing g2 field")
        g2_pb = packet_pb.payload.g2

        channel_map = np.zeros(len(g2_pb.channels), dtype=np.int64)

        channels = list(g2_pb.channels)
        channels.sort()
        first_channel = channels[0]

        # All channels should have the same tau values
        tau = g2_pb.meta.dt * np.array(g2_pb.meta.k)[1:]

        # Note we exclude the first value of g2 and tau
        # to reduce effects of photon pileup
        data_size = len(g2_pb.channels[first_channel].g2) - 1
        header = packet_pb.header

        g2_data = np.zeros((len(channels), data_size))
        for i, channel in enumerate(channels):
            channel_map[i] = channel
            g2_data[i] = g2_pb.channels[channel].g2[1:]

        command = self.generate_command(header.experiment_id)

        return G2Message(
            timestamp=time.time(),
            channel_map=channel_map,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            tau=tau,
            g2=g2_data - 1,  # Note we always work with g2 - 1
            command=command,
        )


class G2ISerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(G2IMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def g2i(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> G2IMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        if not packet_pb.payload.HasField("g2i"):
            raise df.LabgraphError("packet_pb missing g2i field")
        g2_pb = packet_pb.payload.g2i

        channel_map = np.zeros(len(g2_pb.channels), dtype=np.int64)

        channels = list(g2_pb.channels)
        channels.sort()
        first_channel = channels[0]

        # All channels should have the same tau values
        tau = g2_pb.meta.dt * np.array(g2_pb.meta.k)

        # Note we do not exclude the first value of g2i like we do for g2
        data_size = len(g2_pb.channels[first_channel].g2i)
        header = packet_pb.header

        g2_data = np.zeros((len(channels), data_size))
        for i, channel in enumerate(channels):
            channel_map[i] = channel
            g2_data[i] = g2_pb.channels[channel].g2i

        utime_from_ps = g2_pb.channels[first_channel].utime_from_ps
        utime_until_ps = g2_pb.channels[first_channel].utime_until_ps

        command = self.generate_command(header.experiment_id)

        return G2IMessage(
            timestamp=time.time(),
            channel_map=channel_map,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            tau=tau,
            g2=g2_data - 1,  # Note we always work with g2 - 1
            utime_from_ps=utime_from_ps,
            utime_until_ps=utime_until_ps,
            command=command,
        )


class IMUSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(IMUMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def imu(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> IMUMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("imu")
        imu_pb = packet_pb.payload.imu
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return IMUMessage(
            timestamp=time.time(),
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            device_id=imu_pb.device_id,
            gyro_x=imu_pb.gyro_x,
            gyro_y=imu_pb.gyro_y,
            gyro_z=imu_pb.gyro_z,
            accel_x=imu_pb.accel_x,
            accel_y=imu_pb.accel_y,
            accel_z=imu_pb.accel_z,
            fsync=imu_pb.fsync,
            command=command,
        )


class RespBeltSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(RespBeltMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def resp_belt(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> RespBeltMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("resp_belt")
        resp_belt_pb = packet_pb.payload.resp_belt
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return RespBeltMessage(
            timestamp=time.time(),
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            device_id=resp_belt_pb.device_id,
            sample=resp_belt_pb.sample,
            command=command,
        )


class PulseOxSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(PulseOxMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def pulse_ox(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> PulseOxMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("pulse_ox")
        pulse_ox_pb = packet_pb.payload.pulse_ox
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return PulseOxMessage(
            timestamp=time.time(),
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            device_id=pulse_ox_pb.device_id,
            sample=pulse_ox_pb.sample,
            command=command,
        )


class HRMSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(HRMMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def pulse_ox(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> HRMMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("hrm")
        hrm_pb = packet_pb.payload.hrm
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return HRMMessage(
            timestamp=time.time(),
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            device_id=hrm_pb.device_id,
            sample=hrm_pb.sample,
            command=command,
        )


class PowerMeterSerializer(PandoSerializer):
    INPUT = df.Topic(ZMQMessage)
    OUTPUT = df.Topic(PowerMeterMessage)

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def power_meter(self, message: ZMQMessage) -> df.AsyncPublisher:
        yield self.OUTPUT, self.serialize_message(message)

    def serialize_message(self, message: ZMQMessage) -> PowerMeterMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(message.data)
        assert packet_pb.payload.HasField("analog_power_meter")
        power_meter_pb = packet_pb.payload.analog_power_meter
        header = packet_pb.header

        command = self.generate_command(header.experiment_id)

        return PowerMeterMessage(
            timestamp=time.time(),
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
            device_id=power_meter_pb.device_id,
            sample=power_meter_pb.sample,
            command=command,
        )
