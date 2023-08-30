#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest
import numpy as np

from arvr import pando_pb2
from ..serializers import IMUSerializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    device_id: int,
    gyro_x: int,
    gyro_y: int,
    gyro_z: int,
    accel_x: int,
    accel_y: int,
    accel_z: int,
    fsync: int,
    exp_id: int,
    seq_num: int,
    timestamp: int,
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    packet.payload.imu.device_id = device_id
    packet.payload.imu.gyro_x = gyro_x
    packet.payload.imu.gyro_y = gyro_y
    packet.payload.imu.gyro_z = gyro_z
    packet.payload.imu.accel_x = accel_x
    packet.payload.imu.accel_y = accel_y
    packet.payload.imu.accel_z = accel_z
    packet.payload.imu.fsync = fsync

    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp

    return packet


class IMUSerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        device_id = 1
        gyro_x = 1
        gyro_y = 2
        gyro_z = 3
        accel_x = 4
        accel_y = 5
        accel_z = 6
        fsync = 1
        exp_id = 1
        seq_num = 10
        timestamp = 1024

        message = create_message(
            device_id=device_id,
            gyro_x=gyro_x,
            gyro_y=gyro_y,
            gyro_z=gyro_z,
            accel_x=accel_x,
            accel_y=accel_y,
            accel_z=accel_z,
            fsync=fsync,
            exp_id=exp_id,
            seq_num=seq_num,
            timestamp=timestamp,
        )
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = IMUSerializer()
        imu_msg = serializer.serialize_message(zmq_msg)

        self.assertEqual(imu_msg.device_id, device_id)
        self.assertEqual(imu_msg.gyro_x, gyro_x)
        self.assertEqual(imu_msg.gyro_y, gyro_y)
        self.assertEqual(imu_msg.gyro_z, gyro_z)
        self.assertEqual(imu_msg.accel_x, accel_x)
        self.assertEqual(imu_msg.accel_y, accel_y)
        self.assertEqual(imu_msg.accel_z, accel_z)
        self.assertEqual(imu_msg.fsync, fsync)
        self.assertEqual(imu_msg.experiment_id, exp_id)
        self.assertEqual(imu_msg.sequence_number, seq_num)
        self.assertEqual(imu_msg.pando_timestamp, timestamp)


if __name__ == "__main__":
    unittest.main()
