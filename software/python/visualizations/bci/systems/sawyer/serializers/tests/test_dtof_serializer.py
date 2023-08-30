#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import numpy as np
import unittest
from typing import List

from arvr import pando_pb2
from ..serializers import DTOFSerializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    channels: List[int],
    resolution: int,
    range_min: int,
    range_max: int,
    integration_period_ns: float,
    counts: np.ndarray,
    exp_id: int,
    seq_num: int,
    timestamp: int,
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    for i, channel_label in enumerate(channels):
        channel = packet.payload.dtof.channels[channel_label]
        channel.counts.extend(counts[i])
    packet.payload.dtof.meta.resolution = resolution
    packet.payload.dtof.meta.range_min = range_min
    packet.payload.dtof.meta.range_max = range_max
    packet.payload.dtof.meta.integration_period_ns = integration_period_ns
    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp
    return packet


class DTOFSerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        channels = np.array([0, 1, 2, 3, 4, 5, 6, 7])
        resolution = 20000
        range_min = 0
        range_max = 20000
        integration_period_ns = 100 * 1e9  # 100ms
        counts = np.random.randint(10, size=(8, 20000), dtype=np.int32)
        exp_id = 1
        seq_num = 10
        pando_timestamp = 1234
        message = create_message(
            channels,
            resolution,
            range_min,
            range_max,
            integration_period_ns,
            counts,
            exp_id,
            seq_num,
            pando_timestamp,
        )
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = DTOFSerializer()
        dtof_msg = serializer.serialize_message(zmq_msg)

        self.assertTrue(np.array_equal(dtof_msg.channel_map, channels))
        self.assertEqual(dtof_msg.resolution, resolution)
        self.assertEqual(dtof_msg.range_min, range_min)
        self.assertEqual(dtof_msg.range_max, range_max)
        self.assertEqual(dtof_msg.integration_period_ns, integration_period_ns)
        self.assertTrue(np.array_equal(dtof_msg.counts, counts))
        self.assertEqual(dtof_msg.experiment_id, exp_id)
        self.assertEqual(dtof_msg.sequence_number, seq_num)
        self.assertEqual(dtof_msg.pando_timestamp, pando_timestamp)


if __name__ == "__main__":
    unittest.main()
