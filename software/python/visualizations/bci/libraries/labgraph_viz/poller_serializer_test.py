import logging
import os

# downstream nodes need this declared
os.environ["CAM_MODE"] = "basler"

from .viz_nodes import PollerSerializer
from typing import Tuple
from argparse import ArgumentParser
from labgraph.zmq_node import ZMQPollerNode, ZMQPollerConfig, ZMQMessage
from bci.nodes.profiler_node import ProfileNode
from bci.systems.sawyer.messages import BaslerImageMessage as CameraImageMessage

import labgraph as df
import time


class Consumer(ProfileNode):
    INPUT = df.Topic(CameraImageMessage)
    st = time.time()

    @df.subscriber(INPUT)
    async def consumer(self, message: CameraImageMessage) -> None:
        if self.messages_processed == 0:
            self.st = time.time()
        self.increment_message_processed()
        logging.info("message received %s", self.messages_processed)
        time_elapsed = time.time() - self.st
        if self.messages_processed % 100 == 50:
            logging.info("fps %s", self.messages_processed / time_elapsed)


class PollerSerializerTestConfig(df.Config):
    read_addr: str = "tcp://localhost:5060"


class PollerSerializerTest(df.Graph):
    POLLER: PollerSerializer
    C: Consumer

    def setup(self) -> None:
        self.POLLER.configure(
            ZMQPollerConfig(
                read_addr=self._config.read_addr, zmq_topic="pando.camera_image"
            )
        )

    def connections(self) -> df.Connections:
        return ((self.POLLER.topic, self.C.INPUT),)

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (self.POLLER, self.C)


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument(
        "--read-addr",
        help="The ZMQ address to read data from",
        default="tcp://127.0.0.1:5060",
    )
    args = parser.parse_args()
    graph = PollerSerializerTest()
    graph.configure(PollerSerializerTestConfig(read_addr=args.read_addr))
    runner = df.ParallelRunner(graph)
    runner.run()
