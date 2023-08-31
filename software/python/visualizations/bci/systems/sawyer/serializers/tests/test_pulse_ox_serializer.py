#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import unittest

from arvr import pando_pb2
from ..serializers import PulseOxSerializer
from labgraph.zmq_node import ZMQMessage


def create_message(
    device_id: int, sample: int, exp_id: int, seq_num: int, timestamp: int
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    packet.payload.pulse_ox.device_id = device_id
    packet.payload.pulse_ox.sample = sample

    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp

    return packet


class PulseOxSerializerTest(unittest.TestCase):
    def test_serialize_message(self) -> None:
        device_id = 1
        sample = 123
        exp_id = 1
        seq_num = 10
        timestamp = 1024

        message = create_message(
            device_id=device_id,
            sample=sample,
            exp_id=exp_id,
            seq_num=seq_num,
            timestamp=timestamp,
        )
        zmq_msg = ZMQMessage(data=message.SerializeToString())
        serializer = PulseOxSerializer()
        pulse_ox_msg = serializer.serialize_message(zmq_msg)

        self.assertEqual(pulse_ox_msg.device_id, device_id)
        self.assertEqual(pulse_ox_msg.sample, sample)
        self.assertEqual(pulse_ox_msg.experiment_id, exp_id)
        self.assertEqual(pulse_ox_msg.sequence_number, seq_num)
        self.assertEqual(pulse_ox_msg.pando_timestamp, timestamp)


if __name__ == "__main__":
    unittest.main()
