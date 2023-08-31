from threading import Thread
from queue import Queue
import logging
from itertools import repeat
import sys

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)


def queue_get(queue):
    try:
        ("Trying q get")
        v = queue.get()
        if v == None:
            raise StopIteration
        return v
    except:
        raise StopIteration


def _wrapper(out_pipe, name, filename, fn, **kwargs):
    log = logging.getLogger(name)
    forever = repeat(None)
    iterator = (queue_get(out_pipe) for _ in forever)
    log.info(f"Spinning up module: limonene.{name}")
    fn(filename, iterator, **kwargs)
    log.info(f"Shutting down module: limonene.{name}")


class LimoneneModule:
    def __init__(self, name, mod_fn):
        self.name = name
        self._fn = mod_fn

    def launch(self, filename, **kwargs):
        q = Queue()
        thread = Thread(
            target=_wrapper,
            args=(q, self.name, filename, self._fn),
            kwargs=kwargs,
            name=self.name,
        )
        thread.start()
        return thread, q
