#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time

import labgraph as df
from bci.messages.dynamic_phantom import DynamicPhantomTriggerMessage
from bci.stimulus.psychopy.constants import (
    StimulusActivationsType,
    TrialMode,
    ViewChangeType,
)
from bci.stimulus.psychopy.messages import ViewChangeMessage


class DynamicPhantomTriggerConfig(df.Config):
    stimulus_activations: StimulusActivationsType


class DynamicPhantomTriggerNode(df.Node):
    TRIGGER_TOPIC = df.Topic(DynamicPhantomTriggerMessage)
    VIEW_CHANGE_TOPIC = df.Topic(ViewChangeMessage)

    config: DynamicPhantomTriggerConfig

    @df.subscriber(VIEW_CHANGE_TOPIC)
    @df.publisher(TRIGGER_TOPIC)
    async def stimulus_to_trigger(
        self, message: ViewChangeMessage
    ) -> df.AsyncPublisher:
        if message.change_type != ViewChangeType.STIMULUS:
            return
        if message.trial_mode != TrialMode.PERFORMANCE:
            return
        if message.condition not in self.config.stimulus_activations:
            return
        side = self.config.stimulus_activations[message.condition]
        yield (
            self.TRIGGER_TOPIC,
            DynamicPhantomTriggerMessage(timestamp=time.time(), side=side),
        )
