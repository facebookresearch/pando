#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
import cProfile
import os
import time

from labgraph.graphs.node import Node


class ProfileNode(Node):
    """
    Node with setup and cleanup methods that invoke cProfiler
    """

    profiler: cProfile.Profile
    name: str
    messages_processed: int = 0
    start_time: float = 0.0
    last_update_time: float = 0.0
    ping_time: float = 0.0
    total_time: float = 0.0
    time_dict: dict = {}

    def setup(self) -> None:
        # disabling this now that profiling is no longer needed
        # self.profiler = cProfile.Profile()
        # self.profiler.enable()
        self.name = type(self).__name__
        logging.info("[%s] Starting with profiling on PID %d", self.name, os.getpid())

    def cleanup(self) -> None:
        self.total_time = self.last_update_time - self.start_time
        self.print_stats()
        # self.profiler.disable()
        # self.profiler.dump_stats("pstat_%s.pstat" % self.name)

    def frame_rate(self) -> float:
        try:
            return self.messages_processed / (self.last_update_time - self.start_time)
        except ZeroDivisionError:
            return 0.0

    def increment_message_processed(self, print_stats: bool = False) -> None:
        # Set the start_time on the first frame
        if self.start_time < 0.001:
            self.start_time = time.time()

        self.messages_processed += 1
        self.last_update_time = time.time()
        if print_stats:
            self.print_stats()

    def time_since_last_update(self) -> None:
        time_elapsed = time.time() - self.last_update_time
        logging.info("[%s] Time since last update %s", self.name, time_elapsed)

    # requires string argument describing the lines preceding call
    def time_since_last_ping(self, description: str) -> None:
        key = description.replace(" ", "").replace(".", "")
        if self.ping_time < 0.001:
            self.ping_time = time.time()
            time_elapsed = time.time() - self.last_update_time
            self.time_dict[key] = time_elapsed
        else:
            time_elapsed = time.time() - self.ping_time
            self.ping_time = time.time()
            if key not in self.time_dict:
                self.time_dict[key] = time_elapsed
            mp = self.messages_processed
            oa = self.time_dict[key]
            self.time_dict[key] = ((mp - 1) * oa + time_elapsed) / mp
        logging.info("[%s] Time for %s: %s", self.name, description, time_elapsed)

    def print_stats(self) -> None:
        logging.info("[%s] Total frames emitted %d", self.name, self.messages_processed)
        logging.info("[%s] Frame Rate %.2f", self.name, self.frame_rate())
        for pair in self.time_dict.items():
            logging.info(pair)
