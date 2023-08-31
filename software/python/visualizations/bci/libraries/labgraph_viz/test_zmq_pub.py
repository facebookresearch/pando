import logging

import zmq
import time
import os
import sys
import numpy as np

# need to set this here for making this environment variable visible to the serializer
if "basler" in sys.argv:
    os.environ["CAM_MODE"] = "basler"
else:
    os.environ["CAM_MODE"] = "hamamatsu"

from arvr import pando_pb2


def create_message(
    device_id: int,
    width: int,
    height: int,
    bytes_per_pixel: int,
    exp_id: int,
    seq_num: int,
    timestamp: int,
    image_data: np.array,
) -> pando_pb2.Packet:
    packet = pando_pb2.Packet()
    packet.payload.camera_image.device_id = device_id
    packet.payload.camera_image.width = width
    packet.payload.camera_image.height = height
    packet.payload.camera_image.image_data = image_data.tobytes()
    packet.payload.camera_image.bytes_per_pixel = bytes_per_pixel

    packet.header.experiment_id = exp_id
    packet.header.sequence_number = seq_num
    packet.header.timestamp = timestamp

    return packet


ctx = zmq.Context()
sock = ctx.socket(zmq.PUB)
if os.environ["CAM_MODE"] == "basler":
    sock.bind("tcp://127.0.0.1:5060")
else:
    sock.bind("tcp://127.0.0.1:5061")

device_id = 1
width: int
height: int
if os.environ["CAM_MODE"] == "basler":
    width = 1920
    height = 1200
else:
    width = 2304
    height = 2304
bytes_per_pixel = 1
exp_id = 1
seq_num = 10
timestamp = 1024
image_data = np.random.randint(low=0, high=255, size=(width, height), dtype=np.uint8)

message = create_message(
    device_id=device_id,
    width=width,
    height=height,
    bytes_per_pixel=bytes_per_pixel,
    exp_id=exp_id,
    seq_num=seq_num,
    timestamp=timestamp,
    image_data=image_data,
)

topic = "pando.camera_image".encode()
data = message.SerializeToString()

start_time = time.time()
msgs_sent = 0

while True:
    sock.send_multipart([topic, data])
    msgs_sent += 1
    if msgs_sent % 100 == 0:
        logging.info("Publisher FPS: %s", msgs_sent / (time.time() - start_time))
        logging.info("Messages Emitted: %s", msgs_sent)
    time.sleep(0.01)

sock.close()
ctx.term()
