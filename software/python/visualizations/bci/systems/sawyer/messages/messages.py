#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import numpy as np
from enum import Enum


class Command(str, Enum):
    NONE = "none"
    CLEAR = "clear"


class DataMessage(df.Message):
    timestamp: float
    data: float
    command: Command = Command.NONE


class UpdateMessage(df.Message):
    x: np.ndarray
    y: np.ndarray
    command: Command = Command.NONE


class DTOFVizMessage(df.Message):
    x: df.NumpyDynamicType()
    y: df.NumpyDynamicType()


class HistogramMessage(df.Message):
    x: df.NumpyType((33,), np.float64)
    y: df.NumpyType((32,), np.int64)


class PandoMessage(df.Message):
    timestamp: float
    experiment_id: int
    sequence_number: int
    pando_timestamp: df.IntType(c_type=df.CIntType.LONG_LONG)
    command: Command = Command.NONE


class CountRateMessage(PandoMessage):
    channel_map: df.NumpyDynamicType()
    count_rates: np.ndarray

    def __str__(self):
        return (
            "<CountRateMessage ts: {} cr: {} cm: {}" "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.count_rates.shape,
            self.channel_map.shape,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class CRIMessage(PandoMessage):
    channel_map: df.NumpyDynamicType()
    count_rates: np.ndarray
    utime_from_ps: int
    utime_until_ps: int

    def __str__(self):
        return (
            "<CRIMessage ts: {} cr: {} cm: {}" "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.count_rates.shape,
            self.channel_map.shape,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class G2Message(PandoMessage):
    channel_map: df.NumpyDynamicType()
    tau: np.ndarray
    g2: np.ndarray

    def __str__(self):
        return (
            "<G2Message ts: {} tau: {} g2: {} cm: {}" "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.tau.shape,
            self.g2.shape,
            self.channel_map.shape,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class G2IMessage(PandoMessage):
    channel_map: df.NumpyDynamicType()
    tau: np.ndarray
    g2: np.ndarray
    utime_from_ps: int
    utime_until_ps: int

    def __str__(self):
        return (
            "<G2IMessage ts: {} tau: {} g2: {} cm: {} from: {} until: {}"
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.tau.shape,
            self.g2.shape,
            self.channel_map.shape,
            self.utime_from_ps,
            self.utime_until_ps,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class G2FitMessage(PandoMessage):
    channel_map: df.NumpyDynamicType()
    tau: np.ndarray
    g2_mean: np.ndarray
    g2_fit: np.ndarray

    def __str__(self):
        return (
            "<G2Message ts: {} tau: {} g2_mean: {} g2_fit: {} cm: {}"
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.tau.shape,
            self.g2_mean.shape,
            self.g2_fit.shape,
            self.channel_map.shape,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class DTOFMessage(PandoMessage):
    channel_map: df.NumpyDynamicType()
    resolution: int
    range_min: int
    range_max: int
    integration_period_ns: float
    counts: df.NumpyDynamicType()

    def __str__(self):
        return (
            "<DTOFMessage ts: {} resolution: {} range_min: {} range_max: {}"
            "integration period: {} counts: {} cm: {} pando: [exp: {} seq: {}"
            "ts: {}]>"
        ).format(
            self.timestamp,
            self.resolution,
            self.range_min,
            self.range_max,
            self.integration_period_ns,
            self.counts.shape,
            self.channel_map.shape,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class HamamatsuImageMessage(PandoMessage):
    device_id: np.int32
    width: np.int32
    height: np.int32
    bytes_per_pixel: np.int32
    image_data: df.NumpyType(shape=(2304, 2304), dtype=np.uint8)

    def __str__(self):
        return (
            "<BasslerMessage ts: {} device_id: {} width: {} height: {}"
            "bytes_per_pixel: {} image_data: {}"
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.width,
            self.height,
            self.bytes_per_pixel,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class BaslerImageMessage(PandoMessage):
    device_id: np.int32
    width: np.int32
    height: np.int32
    bytes_per_pixel: np.int32
    image_data: df.NumpyType(shape=(1200, 1920), dtype=np.uint8)

    def __str__(self):
        return (
            "<BasslerMessage ts: {} device_id: {} width: {} height: {}"
            "bytes_per_pixel: {} image_data: {}"
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.width,
            self.height,
            self.bytes_per_pixel,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class IMUMessage(PandoMessage):
    device_id: int
    gyro_x: int
    gyro_y: int
    gyro_z: int
    accel_x: int
    accel_y: int
    accel_z: int
    fsync: int

    def __str__(self):
        return (
            "<IMUMessage ts: {} device_id: {} gyro_x: {} gyro_y: {} "
            "gyro_z: {} accel_x: {} accel_y: {} accel_z: {} fsync: {} "
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.gyro_x,
            self.gyro_y,
            self.gyro_z,
            self.accel_x,
            self.accel_y,
            self.accel_z,
            self.fsync,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class RespBeltMessage(PandoMessage):
    device_id: int
    sample: int

    def __str__(self):
        return (
            "<RespBeltMessage ts: {} device_id: {} sample: {} "
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.sample,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class PulseOxMessage(PandoMessage):
    device_id: int
    sample: int

    def __str__(self):
        return (
            "<PulseOxMessage ts: {} device_id: {} sample: {} "
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.sample,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class HRMMessage(PandoMessage):
    device_id: int
    sample: int

    def __str__(self):
        return (
            "<HRMMessage ts: {} device_id: {} sample: {} "
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.sample,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class PowerMeterMessage(PandoMessage):
    device_id: int
    sample: int

    def __str__(self):
        return (
            "<PowerMeterMessage ts: {} device_id: {} sample: {} "
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.sample,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class DigitalInMessage(PandoMessage):
    device_id: int
    edge: int

    def __str__(self):
        return (
            "<DigitalInMessage ts: {} device_id: {} edge: {} "
            "pando: [exp: {} seq: {} ts: {}]>"
        ).format(
            self.timestamp,
            self.device_id,
            self.edge,
            self.experiment_id,
            self.sequence_number,
            self.pando_timestamp,
        )


class TableStatsMessage(df.Message):
    data: df.NumpyDynamicType()
