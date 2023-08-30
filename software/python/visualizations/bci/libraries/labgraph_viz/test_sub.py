import zmq
import time

ctx = zmq.Context()
sock = ctx.socket(zmq.SUB)
sock.connect("tcp://127.0.0.1:5060")
sock.subscribe("")  # Subscribe to all topics

print("Starting receiver loop ...")
while True:
    topic, data = sock.recv_multipart()
    print("topic: %s ..." % topic)

sock.close()
ctx.term()
