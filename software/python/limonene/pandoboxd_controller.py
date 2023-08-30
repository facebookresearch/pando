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


class PandoboxdController(LimoneneController):
    """Our swiss army knife for pandoboxd control. Encapsulates zmq
    communication and PB serialization / deserialization.

    """

    PUBSUB_PORT = "5064"
    REQRES_PORT = "5063"

    def __init__(
        self,
        receive_timeout_ms=DEFAULT_TIMEOUT_MS,
        addr="localhost",
        filters=None,
        sort_buffer_size=16,
    ):
        super().__init__(True, sort_buffer_size)
        self._ctx = zmq.Context()

        # Request socket, used for command/control
        self._req_sock = self._ctx.socket(zmq.REQ)
        self._req_sock.connect("tcp://{}:{}".format(addr, self.REQRES_PORT))
        self._req_sock.setsockopt(zmq.RCVTIMEO, receive_timeout_ms)
        # Note: Setting linger is necessary to gracefully quit if,
        # say, pandoboxd isn't alive when we try to connect
        self._req_sock.linger = 100

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
        txn = clem_pb.PandoboxdTransaction()
        txn.start.req.SetInParent()
        txn.start.req.experiment_id = experiment_id
        self._req_sock.send(txn.SerializeToString())

        try:
            resp = self._req_sock.recv()
        except zmq.error.Again:
            return None

        self._prime()
        txn.ParseFromString(resp)
        return txn

    def ping(self):
        """ Checks to make sure pandoboxd is alive.
        """
        txn = clem_pb.PandoboxdTransaction()
        txn.ping.req.SetInParent()
        self._req_sock.send(txn.SerializeToString())
        try:
            resp = self._req_sock.recv()
        except zmq.error.Again:
            return None

        txn.ParseFromString(resp)
        return txn

    def _read_sample(self):
        """Reads a single sample from the PUB/SUB connection.  Returns a
        Sample object

        """
        topic, data = self._sub_sock.recv_multipart()
        # the negative index removes a trailing zero
        topic = topic.decode("utf-8")[:-1]
        sample = jul_pb.Packet()
        sample.ParseFromString(data)

        smp = Sample(topic, sample)
        return smp

    def stop(self):
        """Ends the current experiment run

        """
        txn = clem_pb.PandoboxdTransaction()
        txn.stop.req.SetInParent()
        self._req_sock.send(txn.SerializeToString())
        try:
            resp = self._req_sock.recv()
        except zmq.error.Again:
            return None
        txn.ParseFromString(resp)
        return txn
