# Copyright 2004-present Facebook. All Rights Reserved.

from typing import List, Optional

import labgraph as df
from bci.nodes.window_trigger import (
    make_window_trigger_node,
    WindowTriggerConfig,
)


###############################################################################
#                           Types used across tests                           #
###############################################################################


class SampleMessage(df.TimestampedMessage):
    sample: int

    def __eq__(self, other: "SampleMessage"):
        return (self.timestamp == other.timestamp) and (self.sample == other.sample)


###############################################################################
#                         Parameters used across tests                        #
###############################################################################


def trigger_on_index(index: int) -> bool:
    return index % 2 == 0


def trigger(messages: List[SampleMessage]) -> bool:
    return trigger_on_index(messages[0].sample)


###############################################################################
#           Test returned window node matches expected state machine          #
###############################################################################


class TestMakeWindowTriggerNode:

    StreamState = Optional[List[df.TimestampedMessage]]

    length = 2
    messages_to_send = 10

    def predict_state(self, timestamp: float, index: int) -> StreamState:
        if trigger_on_index(index - self.length + 1):
            return [
                SampleMessage(timestamp=timestamp - k, sample=index - k)
                for k in range(self.length)
            ][::-1]

        return None

    def convert_run_async_message_to_stream_state(self, window_message) -> StreamState:
        if len(window_message) == 0:
            return None

        return window_message[0][1].sample

    def test_function_returns_same_after_lift(self) -> None:
        WindowTriggerMessage, WindowTriggerNode = make_window_trigger_node(
            SampleMessage, trigger
        )

        test_harness = df.NodeTestHarness(WindowTriggerNode)
        config = WindowTriggerConfig(self.length)

        with test_harness.get_node(config=config) as node:
            for k in range(self.messages_to_send):
                msg = SampleMessage(timestamp=float(k), sample=k)

                predicted_state = self.predict_state(msg.timestamp, k)
                state = self.convert_run_async_message_to_stream_state(
                    df.run_async(node.run, args=[msg])
                )

                assert predicted_state == state


###############################################################################
#                                Run in a graph                               #
###############################################################################

# # Needs to be at a top level in order to be run through an actual graph
length = 2
messages_to_send = 10

WindowTriggerMessage, WindowTriggerNode = make_window_trigger_node(
    SampleMessage, trigger
)


class Generator(df.Node):
    OUTPUT = df.Topic(SampleMessage)

    counter: int

    def setup(self):
        self.counter = 0

    @df.publisher(OUTPUT)
    async def run(self) -> df.AsyncPublisher:
        while self.counter < messages_to_send:
            self.counter += 1
            yield self.OUTPUT, SampleMessage(
                timestamp=float(self.counter), sample=self.counter
            )
        raise df.NormalTermination()


class Consumer(df.Node):
    INPUT = df.Topic(WindowTriggerMessage)

    @df.subscriber(INPUT)
    async def run(self, messages: WindowTriggerMessage):
        assert trigger(messages)


class Graph(df.Graph):
    GENERATOR: Generator
    WINDOW_TRIGGER: WindowTriggerNode
    CONSUMER: Consumer

    config: WindowTriggerConfig

    def setup(self) -> None:
        self.WINDOW_TRIGGER.configure(self.config)

    def connections(self) -> df.Connections:
        return (
            (self.GENERATOR.OUTPUT, self.WINDOW_TRIGGER.INPUT),
            (self.WINDOW_TRIGGER.OUTPUT, self.CONSUMER.INPUT),
        )


def test_in_graph() -> None:
    g = Graph()
    g.configure(WindowTriggerConfig(2))
    runner = df.ParallelRunner(graph=g)
    runner.run()
