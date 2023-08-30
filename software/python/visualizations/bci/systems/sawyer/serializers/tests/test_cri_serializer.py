#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest
import numpy as np

from arvr import pando_pb2
from ..serializers import CRISerializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    counts: np.ndarray,
    integration_period_ns: float,
    utimes_from_ps: np.ndarray,
    utimes_until_ps: np.ndarray,
    channels: np.ndarray,
    exp_id: int,
    seq_num: int,
    timestamp: int,
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    for i in range(0, len(counts)):
        channel = packet.payload.cri.channels[i]
        channel.count = counts[i]
        channel.utime_from_ps = utimes_from_ps[i]
        channel.utime_until_ps = utimes_until_ps[i]
    packet.payload.cri.meta.integration_period_ns = integration_period_ns
    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp
    return packet


class CRISerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        channels = np.array([0, 1, 2, 3, 4, 5, 6, 7], dtype=np.int64)
        utimes_from_ps = np.array([20000] * 8)
        utimes_until_ps = np.array([0] * 8)
        counts = np.random.randint(0, 10, size=8, dtype=np.int32)
        integration_period_ns = 100000
        exp_id = 1
        seq_num = 10
        pando_timestamp = 1234
        message = create_message(
            counts,
            integration_period_ns,
            utimes_from_ps,
            utimes_until_ps,
            channels,
            exp_id,
            seq_num,
            pando_timestamp,
        )
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = CRISerializer()
        cri_msg = serializer.serialize_message(zmq_msg)

        self.assertTrue(np.array_equal(cri_msg.channel_map, channels))
        self.assertEqual(cri_msg.utime_from_ps, utimes_from_ps[0])
        self.assertEqual(cri_msg.utime_until_ps, utimes_until_ps[0])
        self.assertTrue(
            np.array_equal(cri_msg.count_rates, counts / (integration_period_ns * 1e-9))
        )
        self.assertEqual(cri_msg.experiment_id, exp_id)
        self.assertEqual(cri_msg.sequence_number, seq_num)
        self.assertEqual(cri_msg.pando_timestamp, pando_timestamp)


if __name__ == "__main__":
    unittest.main()
