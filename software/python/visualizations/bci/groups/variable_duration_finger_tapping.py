#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.


import logging
from labgraph import Config
from labgraph.graphs.group import Connections, Group
from typing import Optional

from bci.stimulus.psychopy.config import get_participant_key_mapping, InputDevice
from bci.stimulus.psychopy.config.protocol_spec import RandomTrialOrderProtocolSpec
from bci.stimulus.psychopy.events import RandomizedEventGraphGenerator
from bci.stimulus.psychopy.nodes import (
    DerivedEventGeneratorNode,
    DisplayNode,
    FingerFeedbackNode,
    FingerFeedbackNodeConfig,
    KeyMappingNode,
)


class VariableDurationFingerTappingConfig(Config):
    development: bool
    simulate_input: bool
    blocks: int
    fullscreen: bool
    protocol_spec: RandomTrialOrderProtocolSpec
    participant_id: str
    no_behavior_feedback: bool
    input_device: Optional[InputDevice] = None


class VariableDurationFingerTappingGroup(Group):
    EVENTS: DerivedEventGeneratorNode
    DISPLAY: DisplayNode
    KEY_MAPPING: KeyMappingNode
    BEHAVIORAL_FEEDBACK: FingerFeedbackNode

    config: VariableDurationFingerTappingConfig

    def setup(self) -> None:

        blocks = self.config.blocks
        simulate_input = self.config.simulate_input
        fullscreen = self.config.fullscreen
        protocol = self.config.protocol_spec
        trial_spec_sets_per_block = protocol.trial_spec_sets_per_block
        participant_id = self.config.participant_id

        if self.config.development:
            trial_spec_sets_per_block = 1
            fullscreen = False

        generator = RandomizedEventGraphGenerator(
            blocks=blocks,
            stimulus_groups=protocol.stimulus_groups,
            trial_spec_set=protocol.trial_spec_set,
            trial_spec_sets_per_block=trial_spec_sets_per_block,
            calibration_blocks=trial_spec_sets_per_block,
            timing_params=protocol.timing_params,
            stimulus_conditions=type(protocol.trial_spec_set[0].condition),
            simulate_input=simulate_input,
            decoding=False,
            pad_block_duration=False,
            disallow_consecutive=protocol.disallow_consecutive,
        )
        protocol.display_config.fullscreen = fullscreen
        self.EVENTS.set_generator(generator)
        protocol.display_config.no_behavior_feedback = self.config.no_behavior_feedback
        self.DISPLAY.set_config(protocol.display_config)

        self.BEHAVIORAL_FEEDBACK.configure(
            FingerFeedbackNodeConfig(protocol.correct_condition_events)
        )

        key_mapping = get_participant_key_mapping(
            participant_id, self.config.input_device
        )
        protocol.display_config.key_mapping_instructions = str(key_mapping)
        self.KEY_MAPPING.set_config(key_mapping)
        logging.info(f"Key mapping: {key_mapping}")

    def connections(self) -> Connections:
        return (
            (self.DISPLAY.KEYBOARD_INPUT_TOPIC, self.KEY_MAPPING.KEYBOARD_INPUT_TOPIC),
            (self.DISPLAY.WAIT_END_TOPIC, self.EVENTS.WAIT_END_TOPIC),
            (self.EVENTS.BLOCK_TOPIC, self.DISPLAY.BLOCK_TOPIC),
            (self.EVENTS.EXPERIMENT_INFO_TOPIC, self.DISPLAY.EXPERIMENT_INFO_TOPIC),
            (self.EVENTS.INSTRUCTIONS_TOPIC, self.DISPLAY.INSTRUCTIONS_TOPIC),
            (self.EVENTS.TRIAL_MODE_TOPIC, self.DISPLAY.TRIAL_MODE_TOPIC),
            (self.EVENTS.WAIT_BEGIN_TOPIC, self.DISPLAY.WAIT_BEGIN_TOPIC),
            (self.EVENTS.TERMINATION_TOPIC, self.DISPLAY.TERMINATION_TOPIC),
            (
                self.BEHAVIORAL_FEEDBACK.BEHAVIORAL_FEEDBACK_TOPIC,
                self.DISPLAY.BEHAVIORAL_FEEDBACK_TOPIC,
            ),
            (
                self.KEY_MAPPING.FINGER_TAP_TOPIC,
                self.BEHAVIORAL_FEEDBACK.FINGER_TAP_TOPIC,
            ),
            (
                self.EVENTS.INSTRUCTIONS_TOPIC,
                self.BEHAVIORAL_FEEDBACK.INSTRUCTIONS_TOPIC,
            ),
            (
                self.EVENTS.EXPERIMENT_INFO_TOPIC,
                self.BEHAVIORAL_FEEDBACK.EXPERIMENT_INFO_TOPIC,
            ),
            (self.EVENTS.BLOCK_TOPIC, self.BEHAVIORAL_FEEDBACK.BLOCK_TOPIC,),
            (self.EVENTS.TRIAL_MODE_TOPIC, self.BEHAVIORAL_FEEDBACK.TRIAL_MODE_TOPIC),
            (self.EVENTS.TERMINATION_TOPIC, self.BEHAVIORAL_FEEDBACK.TERMINATION_TOPIC),
        )
