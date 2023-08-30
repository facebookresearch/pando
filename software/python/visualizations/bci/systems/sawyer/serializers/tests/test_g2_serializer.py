#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest
import numpy as np

from arvr import pando_pb2
from ..serializers import G2Serializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    k: np.ndarray, g2: np.ndarray, dt: float, exp_id: int, seq_num: int, timestamp: int
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    for i in range(0, len(g2)):
        channel = packet.payload.g2.channels[i]
        channel.g2.extend(g2[i])
    packet.payload.g2.meta.k.extend(k)
    packet.payload.g2.meta.dt = dt
    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp
    return packet


class G2SerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        dt = 0.000000001
        k = np.arange(1000, 37000, 1000)
        g2 = np.random.rand(1024, 36).astype(np.float64)
        exp_id = 1
        seq_num = 10
        pando_timestamp = 1234
        message = create_message(k, g2, dt, exp_id, seq_num, pando_timestamp)
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = G2Serializer()
        g2_msg = serializer.serialize_message(zmq_msg)

        # The serializer ignores the first value for tau and g2 for each channel
        tau_expected = np.array([(k * dt)[1:]] * 1024)[0]
        g2_expected = g2[:, 1:] - 1

        self.assertTrue(np.array_equal(g2_msg.channel_map, np.arange(0, 1024)))
        self.assertTrue(np.array_equal(g2_msg.g2, g2_expected))
        self.assertTrue(np.array_equal(g2_msg.tau, tau_expected))
        self.assertEqual(g2_msg.experiment_id, exp_id)
        self.assertEqual(g2_msg.sequence_number, seq_num)
        self.assertEqual(g2_msg.pando_timestamp, pando_timestamp)


if __name__ == "__main__":
    unittest.main()
