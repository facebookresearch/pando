#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
import math
import random
import statistics
from collections import defaultdict
from typing import Dict, List, Optional, Type

from bci.stimulus.psychopy.config import TimingParams, TrialSpec
from bci.stimulus.psychopy.constants import (
    Condition,
    Instructions,
    StimulusGroupsType,
    TrialMode,
)
from bci.stimulus.psychopy.messages import (
    BlockMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    TrialModeMessage,
)
from labgraph.events import (
    BaseEventGenerator,
    DeferredMessage,
    Event,
    EventGraph,
    EventPublishingHeap,
    WaitBeginMessage,
)
from labgraph.graphs.topic import Topic
from labgraph.util.error import LabgraphError


class RandomizedEventGraphGenerator(BaseEventGenerator):
    """
    Adaptation of ViewSequenceGenerator for major version 1 of Labgraph.  Generates
    a graph of events describing the rendering of the experiment.

    The trial ordering is not enforced (other than no repeating same condition in
    a row), and is randomized.
    """

    WAIT_TIMEOUT = 3600.0

    def __init__(
        self,
        blocks: int,
        stimulus_conditions: Type[Condition],
        stimulus_groups: StimulusGroupsType,
        trial_spec_set: List[TrialSpec],
        trial_spec_sets_per_block: int,
        calibration_blocks: int,
        timing_params: TimingParams,
        simulate_input: bool,
        decoding: bool,
        pad_block_duration: bool,
        disallow_consecutive: Optional[List[int]] = None,
    ) -> None:
        super().__init__()
        self.blocks = blocks
        self.stimulus_conditions = stimulus_conditions
        self.stimulus_groups = stimulus_groups
        self.trial_spec_set = trial_spec_set
        self.trial_spec_sets_per_block = trial_spec_sets_per_block
        self.trials = len(trial_spec_set) * trial_spec_sets_per_block
        self.calibration_blocks = calibration_blocks
        self.timing_params = timing_params
        self.wait_for_input = not simulate_input
        self.decoding = decoding
        self.pad_block_duration = pad_block_duration

        self._topics_set: bool = False
        self.disallow_consecutive = disallow_consecutive
        if self.disallow_consecutive is not None:
            self._validate_trial_spec_set()
        self.max_block_duration = self._get_max_block_duration()

    def _validate_trial_spec_set(self) -> None:
        buckets = defaultdict(int)
        for trial_spec in self.trial_spec_set:
            buckets[self.stimulus_groups[trial_spec.condition]] += 1
        max_count = math.ceil(sum(buckets.values(), 2) / 2)
        for group, count in buckets.items():
            if (group in self.disallow_consecutive) and (count >= max_count):
                raise LabgraphError(
                    "Cannot arrange the trial_spec_set without adjacent duplicates"
                )

    def _get_max_block_duration(self) -> float:
        max_pre_stimulus_duration = 0.0
        max_performance_duration = 0.0
        max_post_stimulus_duration = 0.0
        for trial_spec in self.trial_spec_set:
            max_pre_stimulus_duration = max(
                max_pre_stimulus_duration, trial_spec.pre_stimulus_duration_range[1]
            )
            max_performance_duration = max(
                max_performance_duration, trial_spec.performance_duration_range[1]
            )
            max_post_stimulus_duration = max(
                max_post_stimulus_duration, trial_spec.post_stimulus_duration_range[1]
            )
        return (
            self.timing_params.block_number_duration
            + self.timing_params.initial_rest_duration
            + (self.trials - 1) * max_pre_stimulus_duration
            + self.trials * max_performance_duration
            + (self.trials - 1) * max_post_stimulus_duration
            + self.timing_params.final_rest_duration
        )

    def _expected_experiment_duration(self) -> float:
        trial_spec_nonperformance_duration = statistics.mean(
            [
                statistics.mean(trial_spec.pre_stimulus_duration_range)
                + statistics.mean(trial_spec.post_stimulus_duration_range)
                for trial_spec in self.trial_spec_set
            ]
        )
        trial_spec_set_performance_duration = sum(
            statistics.mean(trial_spec.performance_duration_range)
            for trial_spec in self.trial_spec_set
        )
        trial_specs_per_block = self.trial_spec_sets_per_block * len(
            self.trial_spec_set
        )
        return float(
            self.blocks
            * self.trial_spec_sets_per_block
            * trial_spec_set_performance_duration
            + self.blocks * self.timing_params.initial_rest_duration
            + self.blocks * self.timing_params.final_rest_duration
            + self.blocks * self.timing_params.block_number_duration
            + self.blocks
            * (trial_specs_per_block - 1)
            * trial_spec_nonperformance_duration
        )

    def _add_wait_for_input_message(self, events: EventGraph) -> None:
        if self.wait_for_input:
            events.add_event_at_end(
                Event(
                    DeferredMessage(WaitBeginMessage, timeout=self.WAIT_TIMEOUT),
                    self.wait_for_input_topic,
                    0.0,
                ),
                events.last_event_added,
            )

    def set_topics(
        self,
        experiment_info_topic: Topic,
        instructions_topic: Topic,
        trial_mode_topic: Topic,
        block_topic: Topic,
        wait_for_input_topic: Topic,
        show_decoder_feedback_topic: Topic,
    ) -> None:
        """
        Sets the topics that any message produced by this generator may be
        published on.  This topic must be called before event generation is
        attempted.
        """
        self.experiment_info_topic = experiment_info_topic
        self.instructions_topic = instructions_topic
        self.trial_mode_topic = trial_mode_topic
        self.block_topic = block_topic
        self.wait_for_input_topic = wait_for_input_topic
        self.show_decoder_feedback_topic = show_decoder_feedback_topic
        self._topics_set = True

    def generate_events(self) -> EventPublishingHeap:
        """
        Generates and returns a heap of experiment events.
        """

        if not self._topics_set:
            raise LabgraphError("Cannot generate messages before topics are set!")

        # Publish experiment info
        events = EventGraph(
            Event(
                DeferredMessage(
                    ExperimentInfoMessage,
                    total_blocks=self.blocks,
                    trial_spec_sets_per_block=self.trial_spec_sets_per_block,
                    expected_experiment_duration=self._expected_experiment_duration(),
                    condition_set=self.trial_spec_set[0].condition_type_name,
                ),
                self.experiment_info_topic,
                0.0,
            )
        )
        self._add_wait_for_input_message(events)

        # key mapping instructions
        events.add_event_at_end(
            Event(
                DeferredMessage(
                    InstructionsMessage, instructions=Instructions.KEY_MAPPING
                ),
                self.instructions_topic,
                0.0,
            ),
            events.last_event_added,
        )
        self._add_wait_for_input_message(events)

        # protocol instructions
        events.add_event_at_end(
            Event(
                DeferredMessage(
                    InstructionsMessage, instructions=Instructions.PROTOCOL
                ),
                self.instructions_topic,
                0.0,
            ),
            events.last_event_added,
        )
        self._add_wait_for_input_message(events)

        for i in range(self.blocks):
            self._create_and_add_block_events(i, events)

        return events.heap

    def _create_and_add_block_events(self, block: int, events: EventGraph) -> None:
        # Block start
        events.add_event_at_end(
            Event(
                DeferredMessage(BlockMessage, block=block),
                self.block_topic,
                0.0,
                self.timing_params.block_number_duration,
            ),
            events.last_event_added,
        )

        # Add trials for block
        self._create_and_add_block_trial_events(block, events)

        if self.decoding and block == self.calibration_blocks - 1:
            # TODO: Add training/feedback events
            pass
        else:
            events.add_event_at_end(
                Event(
                    DeferredMessage(
                        TrialModeMessage,
                        trial_mode=TrialMode.BLOCK_DONE,
                        block=block,
                        trial=-1,
                        condition=self.stimulus_conditions.UNDEFINED,
                    ),
                    self.trial_mode_topic,
                    0.0,
                ),
                events.last_event_added,
            )
            self._add_wait_for_input_message(events)

    def _create_and_add_block_trial_events(
        self, block: int, events: EventGraph
    ) -> float:
        trials = self.trial_spec_set * self.trial_spec_sets_per_block
        # Make sure that consecutive trials are of different types.
        while True:
            # Take a balanced sample
            trials = random.sample(trials, len(trials))
            if self.disallow_consecutive is not None:
                # Check stimulus groups have correct order
                groups = [self.stimulus_groups[trial.condition] for trial in trials]
                if all(
                    g1 != g2
                    for g1, g2 in zip(groups[:-1], groups[1:])
                    if g1 in self.disallow_consecutive
                ):
                    break
            else:
                break

        logging.info(f"Block {block}")
        for (j, trial) in enumerate(trials):
            logging.info(f"  trial {j}: {trial.condition.name}")

        elapsed = 0.0
        for count, trial in enumerate(trials):
            pre_stimulus_duration = self._get_trial_pre_stimulus_duration(count, trial)
            elapsed += pre_stimulus_duration
            performance_duration = self._get_trial_performance_duration(trial)
            elapsed += performance_duration
            post_stimulus_duration = self._get_trial_post_stimulus_duration(
                count, trial, elapsed
            )
            elapsed += post_stimulus_duration
            self._create_and_add_trial_events(
                block,
                count,
                trial,
                pre_stimulus_duration,
                performance_duration,
                post_stimulus_duration,
                events,
            )

    def _get_trial_pre_stimulus_duration(
        self, count: int, trial_spec: TrialSpec
    ) -> float:
        if count == 0:
            return self.timing_params.initial_rest_duration
        else:
            return random.uniform(*trial_spec.pre_stimulus_duration_range)

    def _get_trial_performance_duration(self, trial_spec: TrialSpec) -> float:
        return random.uniform(*trial_spec.performance_duration_range)

    def _get_trial_post_stimulus_duration(
        self, count: int, trial_spec: TrialSpec, elapsed: float
    ) -> float:
        if count == self.trials - 1:
            if self.pad_block_duration:
                return self.max_block_duration - elapsed
            else:
                return self.timing_params.final_rest_duration
        else:
            return random.uniform(*trial_spec.post_stimulus_duration_range)

    def _create_and_add_trial_events(
        self,
        block: int,
        trial: int,
        trial_spec: TrialSpec,
        pre_stimulus_duration: float,
        performance_duration: float,
        post_stimulus_duration: float,
        events: EventGraph,
    ) -> None:

        # Trial start

        # Pre-stimulus
        events.add_event_at_end(
            Event(
                DeferredMessage(
                    TrialModeMessage,
                    trial_mode=TrialMode.PRE_STIMULUS,
                    block=block,
                    trial=trial,
                    condition=trial_spec.condition,
                ),
                self.trial_mode_topic,
                0.0,
                pre_stimulus_duration,
            ),
            events.last_event_added,
        )

        # Performance start
        performance_event = Event(
            DeferredMessage(
                TrialModeMessage,
                trial_mode=TrialMode.PERFORMANCE,
                block=block,
                trial=trial,
                condition=trial_spec.condition,
            ),
            self.trial_mode_topic,
            0.0,
            performance_duration,
        )
        events.add_event_at_end(performance_event, events.last_event_added)

        if self.decoding and block >= self.calibration_blocks:
            # TODO: Add training/feedback events
            pass

        # Post stimulus
        post_stimulus_event = Event(
            DeferredMessage(
                TrialModeMessage,
                trial_mode=TrialMode.POST_STIMULUS,
                block=block,
                trial=trial,
                condition=trial_spec.condition,
            ),
            self.trial_mode_topic,
            0.0,
            post_stimulus_duration,
        )
        events.add_event_at_end(post_stimulus_event, performance_event)
