#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest

from arvr import pando_pb2
from ..serializers import CameraImageSerializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    device_id: int,
    width: int,
    height: int,
    bytes_per_pixel: int,
    exp_id: int,
    seq_num: int,
    timestamp: int,
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    packet.payload.camera_image.device_id = device_id
    packet.payload.camera_image.width = width
    packet.payload.camera_image.height = height
    packet.payload.camera_image.bytes_per_pixel = bytes_per_pixel

    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp

    return packet


# TODO implement test of image_data once format better understood
class CameraImageSerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        device_id = 1
        width = 1920
        height = 1200
        bytes_per_pixel = 2
        exp_id = 1
        seq_num = 10
        timestamp = 1024

        message = create_message(
            device_id=device_id,
            width=width,
            height=height,
            bytes_per_pixel=bytes_per_pixel,
            exp_id=exp_id,
            seq_num=seq_num,
            timestamp=timestamp,
        )
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = CameraImageSerializer()
        camera_image_msg = serializer.serialize_message(zmq_msg)

        self.assertEqual(camera_image_msg.device_id, device_id)
        self.assertEqual(camera_image_msg.width, width)
        self.assertEqual(camera_image_msg.height, height)
        self.assertEqual(camera_image_msg.bytes_per_pixel, bytes_per_pixel)
        self.assertEqual(camera_image_msg.experiment_id, exp_id)
        self.assertEqual(camera_image_msg.sequence_number, seq_num)
        self.assertEqual(camera_image_msg.pando_timestamp, timestamp)


if __name__ == "__main__":
    unittest.main()
