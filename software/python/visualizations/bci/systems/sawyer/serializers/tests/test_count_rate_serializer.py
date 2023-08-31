#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest
import numpy as np

from arvr import pando_pb2
from ..serializers import CountRateSerializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    counts: np.ndarray, period: int, exp_id: int, seq_num: int, timestamp: int
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    for i in range(0, len(counts)):
        channel = packet.payload.counts.channels[i]
        channel.count = counts[i]
    packet.payload.counts.meta.integration_period_ns = period
    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp
    return packet


class CountRateSerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        counts = np.random.randint(10000, size=1024, dtype=np.int64)
        integration_period_ns = 100000
        exp_id = 1
        seq_num = 10
        pando_timestamp = 1234
        message = create_message(
            counts, integration_period_ns, exp_id, seq_num, pando_timestamp
        )
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = CountRateSerializer()
        count_rate_msg = serializer.serialize_message(zmq_msg)

        self.assertTrue(np.array_equal(count_rate_msg.channel_map, np.arange(0, 1024)))
        self.assertTrue(
            np.array_equal(
                count_rate_msg.count_rates, counts / (integration_period_ns * 1e-9)
            )
        )
        self.assertEqual(count_rate_msg.experiment_id, exp_id)
        self.assertEqual(count_rate_msg.sequence_number, seq_num)
        self.assertEqual(count_rate_msg.pando_timestamp, pando_timestamp)


if __name__ == "__main__":
    unittest.main()
