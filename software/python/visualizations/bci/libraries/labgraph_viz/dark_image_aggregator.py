#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
import os

import labgraph as df
import numpy as np
from argparse import ArgumentParser
from typing import Tuple

from .viz_nodes import PollerSerializer

from bci.systems.sawyer.serializers.serializers import CameraImageSerializer
from bci.systems.sawyer.messages import CameraImageMessage
from bci.nodes.profiler_node import ProfileNode
from labgraph.zmq_node import ZMQPollerConfig


# this script exists solely for the purpose of producing the average dark frame values
class ImageAvg(ProfileNode):
    INPUT = df.Topic(CameraImageMessage)

    running_sum: np.ndarray = np.zeros((2304, 2304), dtype="uint64")

    @df.subscriber(INPUT)
    async def update_avg(self, message: CameraImageMessage) -> None:
        self.increment_message_processed()
        mp = self.messages_processed
        logging.info("msg proc: %s", mp)
        self.running_sum += message.image_data
        if mp == 606:
            logging.info("cwd: %s", os.getcwd())
            running_avg = np.floor_divide(self.running_sum, mp).astype("uint8")
            logging.info("dark image: %s", running_avg)
            np.save("dark_image_average", running_avg)


class DFAggregatorConfig(df.Config):
    read_addr: str = "tcp://localhost:5060"


class DFAggregator(df.Graph):
    POLLER: PollerSerializer
    IA: ImageAvg

    def setup(self) -> None:
        self.POLLER.configure(
            ZMQPollerConfig(
                read_addr=self._config.read_addr, zmq_topic="pando.camera_image"
            )
        )

    def connections(self) -> df.Connections:
        return ((self.POLLER.topic, self.IA.INPUT),)

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (self.POLLER, self.IA)


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument(
        "--read-addr",
        help="The ZMQ address to read data from",
        default="tcp://127.0.0.1:5060",
    )
    args = parser.parse_args()
    graph = DFAggregator()
    graph.configure(DFAggregatorConfig(read_addr=args.read_addr))
    runner = df.ParallelRunner(graph)
    runner.run()
