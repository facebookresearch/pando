#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df

from bci.nodes.profiler_node import ProfileNode


class FilterConfig(df.Config):
    filter_ratio: int = 1


class Filter(ProfileNode):
    INPUT = df.Topic(df.Message)
    OUTPUT = df.Topic(df.Message)

    config: FilterConfig
    message_counter: int = 0

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def filter(self, message: df.Message) -> df.Message:
        self.increment_message_processed()
        if self.message_counter % self.config.filter_ratio == 0:
            yield self.OUTPUT, message
        self.message_counter += 1
