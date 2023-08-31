import labgraph as df
import asyncio
import zmq
import zmq.asyncio
import time
import logging
import os
import sys
import contrast_calcs
import numpy as np

from zmq.utils.monitor import parse_monitor_message

if os.environ["CAM_MODE"] == "basler":
    from bci.systems.sawyer.messages import BaslerImageMessage as CameraImageMessage
else:
    from bci.systems.sawyer.messages import HamamatsuImageMessage as CameraImageMessage

from arvr import pando_pb2
from ..plots import byte_stream_deserializer

POLL_TIME = 0.1


class PollerSerializerConfig(df.Config):
    read_addr: str
    zmq_topic: str
    poll_time: float = POLL_TIME


class PollerSerializer(df.Node):
    topic = df.Topic(CameraImageMessage)
    config: PollerSerializerConfig

    dark_image = None

    def setup(self) -> None:
        self.context = zmq.asyncio.Context()
        self.socket = self.context.socket(zmq.SUB)
        self.monitor = self.socket.get_monitor_socket()
        self.socket.connect(self.config.read_addr)
        self.socket.subscribe(self.config.zmq_topic)

        self.poller = zmq.Poller()
        self.poller.register(self.socket, zmq.POLLIN)

        self.socket_open = False

    def cleanup(self) -> None:
        self.socket.close()

    @df.background
    async def socket_monitor(self) -> None:
        while True:
            monitor_result = await self.monitor.poll(100, zmq.POLLIN)
            if monitor_result:
                data = await self.monitor.recv_multipart()
                evt = parse_monitor_message(data)

                event = evt["event"]

                if event == zmq.EVENT_CONNECTED:
                    self.socket_open = True
                elif event == zmq.EVENT_CLOSED:
                    was_open = self.socket_open
                    self.socket_open = False
                    # ZMQ seems to be sending spurious CLOSED event when we
                    # try to connect before the source is running. Only give up
                    # if we were previously connected. If we give up now, we
                    # will never unblock zmq_publisher.
                    if was_open:
                        break
                elif event in (zmq.EVENT_DISCONNECTED, zmq.EVENT_MONITOR_STOPPED,):
                    self.socket_open = False
                    break

    @df.publisher(topic)
    async def zmq_publisher(self) -> df.AsyncPublisher:
        # Wait for socket connection
        while not self.socket_open:
            await asyncio.sleep(POLL_TIME)

        while self.socket_open:
            poll_result = await self.socket.poll(
                self.config.poll_time * 1000, zmq.POLLIN
            )
            if poll_result:
                _, data = await self.socket.recv_multipart()
                yield self.topic, self._format_message(data)

    # overwrite _format_message function so as to return CameraImageMessages
    # instead of ZMQMessages
    def _format_message(self, data: bytes) -> CameraImageMessage:
        packet_pb = pando_pb2.Packet()
        packet_pb.ParseFromString(data)
        if not packet_pb.payload.HasField("camera_image"):
            raise df.LabgraphError("packet_pb missing camera_image field")
        camera_image_pb = packet_pb.payload.camera_image

        header = packet_pb.header

        idata = byte_stream_deserializer(
            camera_image_pb.width,
            camera_image_pb.height,
            camera_image_pb.bytes_per_pixel,
            camera_image_pb.image_data,
        )

        if (
            isinstance(self.dark_image, type(None))
            and os.environ["CAM_MODE"] == "hamamatsu"
        ):
            dark_image_filepath = os.path.join(
                os.getcwd(), "bci\libraries\labgraph_viz\dark_image_average.npy"
            )
            assert os.path.exists(
                dark_image_filepath
            ), "dark_image_average filepath does not exist"
            self.dark_image = np.load(dark_image_filepath)

        if os.environ["CAM_MODE"] == "hamamatsu":
            contrast_calcs.dark_image(self.dark_image, idata)

        return CameraImageMessage(
            timestamp=time.time(),
            device_id=camera_image_pb.device_id,
            width=camera_image_pb.width,
            height=camera_image_pb.height,
            bytes_per_pixel=camera_image_pb.bytes_per_pixel,
            image_data=idata,
            experiment_id=header.experiment_id,
            sequence_number=header.sequence_number,
            pando_timestamp=header.timestamp,
        )
