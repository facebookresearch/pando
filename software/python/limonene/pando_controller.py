import logging
import zmq
import sys
import heapq

from limonene.controller_base import LimoneneController, Sample

from arvr import pandoboxd_rpc_pb2 as clem_pb
from arvr import pando_pb2 as jul_pb

DEFAULT_TIMEOUT_MS = 1000

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("controller")


class PandoController(LimoneneController):
    """Connects to juliet software and streams pub/sub messages

    """

    PUBSUB_PORT = "5060"

    def __init__(
        self,
        receive_timeout_ms=DEFAULT_TIMEOUT_MS,
        addr="localhost",
        filters=None,
        sort_buffer_size=16,
    ):
        super().__init__(True, sort_buffer_size)
        self._ctx = zmq.Context()

        # PUB/SUB socket, used for data sampling
        self._sub_sock = self._ctx.socket(zmq.SUB)
        self._sub_sock.connect("tcp://{}:{}".format(addr, self.PUBSUB_PORT))

        if filters is None:
            log.info("No filters")
            self._sub_sock.setsockopt_string(zmq.SUBSCRIBE, "")
        else:
            for filt in filters:
                self._sub_sock.setsockopt_string(zmq.SUBSCRIBE, filt)

        self._sub_sock.linger = 100

    def start(self, experiment_id=0):
        """Starts an experiment with a given experiment ID

        """
        return True

    def ping(self):
        """ Checks to make sure pandoboxd is alive.
        """
        return True

    def _read_sample(self):
        """Reads a single sample from the PUB/SUB connection.  Returns a
        Sample object

        """
        while True:
            topic, data = self._sub_sock.recv_multipart()
            # the negative index removes a trailing zero
            topic = topic.decode("utf-8")[:-1]
            sample = jul_pb.Packet()

            # We want to disregard these as they are simply text based
            # logs
            if topic.startswith("pando.report"):
                continue
            try:
                sample.ParseFromString(data)
            except:
                print(topic, data)

            if topic == "pando.event":
                if sample.payload.event.event == jul_pb.EventType.EVENT_STOP:
                    return None

            smp = Sample(topic, sample)
            return smp

    def stop(self):
        """Ends the current experiment run

        """
        return True
