import logging
import zmq
import sys
import heapq

from arvr import pandoboxd_rpc_pb2 as clem_pb
from arvr import pando_pb2 as jul_pb

DEFAULT_TIMEOUT_MS = 1000

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("controller")


class Sample:
    """Wrapper object for samples.  Topic should be a string describing
    the event (e.g. 'pandoboxd.imu').  Sample should be a protobuf sample

    """

    def __init__(self, topic, sample):
        self.topic = topic
        self.sample = sample

    def __lt__(self, other):
        return self.sample.header.timestamp < other.sample.header.timestamp


class LimoneneController:
    """Our swiss army knife for pandoboxd control. Encapsulates zmq
    communication and PB serialization / deserialization.

    """

    PUBSUB_PORT = "5064"
    REQRES_PORT = "5063"

    def __init__(
        self, monotonize_samples, sort_buffer_size=16,
    ):
        self._primed = False
        self._monotonize = monotonize_samples

        # Sorting heap management
        self._heap_size = sort_buffer_size
        self._heap = []

        heapq.heapify(self._heap)

    def start(self, experiment_id=0):
        """Starts an experiment with a given experiment ID

        To be implemented by child class
        """
        raise NotImplementedError

    def ping(self):
        """ Checks to make sure pandoboxd is alive.

        To be implemented by child class
        """
        raise NotImplementedError

    def _read_sample(self):
        """Reads a single sample from the some producer.  Returns a
        Sample object.

        To be implemented by child class
        """
        raise NotImplementedError

    def _prime(self):
        """Pre-populates the sorting heap with values

        """
        count = 0
        while count < self._heap_size:
            smp = self._read_sample()
            heapq.heappush(self._heap, smp)
            count += 1
        self._primed = True

    def read_sample(self):
        """Returns a single sample in the form (topic, sample).  Samples are
        guaranteed to have monotonic timestamps.

        """
        if self._monotonize and not self._primed:
            self._prime()

        smp = self._read_sample()
        # If a controller implementation returns nothing, it means
        # we're out of data to process
        if smp is None:
            return (None, None)

        if not self._monotonize:
            return smp.topic, smp.sample

        heapq.heappush(self._heap, smp)

        oldest_sample = heapq.heappop(self._heap)
        return oldest_sample.topic, oldest_sample.sample

    def stop(self):
        """Ends the current experiment run

        """
        raise NotImplementedError
