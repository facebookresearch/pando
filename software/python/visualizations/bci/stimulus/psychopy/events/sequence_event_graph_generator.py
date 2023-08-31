#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import random
import statistics
from typing import List, Type

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


class SequenceEventGraphGenerator(BaseEventGenerator):
    """
    EventGraphGenerator for when there is block-level or experiment-level
    structure.

    `trial_spec_list` contains a list of ordered TrialSpecs for each block.

    """

    WAIT_TIMEOUT = 3600.0

    def __init__(
        self,
        trial_spec_list: List[List[TrialSpec]],
        stimulus_conditions: Type[Condition],
        stimulus_groups: StimulusGroupsType,
        calibration_blocks: int,
        timing_params: TimingParams,
        simulate_input: bool,
        decoding: bool,
        pad_block_duration: bool,
    ) -> None:
        super().__init__()
        self.trial_spec_list = trial_spec_list
        self.stimulus_conditions = stimulus_conditions
        self.stimulus_groups = stimulus_groups
        self.calibration_blocks = calibration_blocks
        self.timing_params = timing_params
        self.wait_for_input = not simulate_input
        self.decoding = decoding
        self.pad_block_duration = pad_block_duration

        self._topics_set: bool = False
        self.max_block_duration = self._get_max_block_duration()
        self.condition_set = self._get_condition_set()

    def _get_condition_set(self) -> str:
        return self.trial_spec_list[0][0].condition_type_name

    def _get_max_block_duration(self) -> float:
        return max(self._get_block_duration(block) for block in self.trial_spec_list)

    def _get_block_duration(self, trial_list: List[TrialSpec]) -> float:

        if len(trial_list) == 1:
            duration = (
                self.timing_params.initial_rest_duration
                + trial_list[0].performance_duration_range[1]
                + self.timing_params.final_rest_duration
            )
        else:
            duration = sum(trial_spec.max_duration for trial_spec in trial_list[1:-1])
            duration += (
                self.timing_params.initial_rest_duration
                + trial_list[0].post_stimulus_duration_range[1]
                + trial_list[0].performance_duration_range[1]
            )
            duration += (
                trial_list[-1].pre_stimulus_duration_range[1]
                + trial_list[-1].performance_duration_range[1]
                + self.timing_params.final_rest_duration
            )

        duration += self.timing_params.block_number_duration
        return duration

    def _expected_experiment_duration(self) -> float:
        # Return average experiment duration,
        # we do not count the block_number_duration in calculation
        return sum(
            # Total avg duration of each block 2nd to 2nd to last trial
            sum(
                statistics.mean(trial.pre_stimulus_duration_range)
                + statistics.mean(trial.performance_duration_range)
                + statistics.mean(trial.post_stimulus_duration_range)
                for trial in block[1:-1]
            )
            # Avg duration of each block's first trial
            + (
                self.timing_params.initial_rest_duration
                + statistics.mean(block[0].performance_duration_range)
                + statistics.mean(block[0].post_stimulus_duration_range)
            )
            # Avg duration of each block's last trial
            + (
                statistics.mean(block[-1].pre_stimulus_duration_range)
                + statistics.mean(block[-1].performance_duration_range)
                + self.timing_params.final_rest_duration
            )
            if len(block) > 1
            # if only one trial in block, then calculate it differently
            else (
                self.timing_params.initial_rest_duration
                + statistics.mean(block[0].performance_duration_range)
                + self.timing_params.final_rest_duration
            )
            # Do this for all blocks
            for block in self.trial_spec_list
        )

    # TODO: Factor this out?
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

    # TODO: Factor this out?
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
                    total_blocks=len(self.trial_spec_list),
                    trial_spec_sets_per_block=-1,  # don't care about this..
                    expected_experiment_duration=self._expected_experiment_duration(),
                    condition_set=self.condition_set,
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

        for i in range(len(self.trial_spec_list)):
            self._create_and_add_block_events(i, events)

        return events.heap

    def _create_and_add_block_events(self, block_idx: int, events: EventGraph) -> None:
        # Block start
        events.add_event_at_end(
            Event(
                DeferredMessage(BlockMessage, block=block_idx),
                self.block_topic,
                0.0,
                self.timing_params.block_number_duration,
            ),
            events.last_event_added,
        )

        # Add trials for block
        self._create_and_add_block_trial_events(block_idx, events)

        if self.decoding and block_idx == self.calibration_blocks - 1:
            # TODO: Add training/feedback events
            pass
        else:
            events.add_event_at_end(
                Event(
                    DeferredMessage(
                        TrialModeMessage,
                        trial_mode=TrialMode.BLOCK_DONE,
                        block=block_idx,
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
        self, block_idx: int, events: EventGraph
    ) -> float:
        trials = self.trial_spec_list[block_idx]
        elapsed = 0.0
        for count, trial in enumerate(trials):
            pre_stimulus_duration = self._get_trial_pre_stimulus_duration(count, trial)
            elapsed += pre_stimulus_duration
            performance_duration = self._get_trial_performance_duration(trial)
            elapsed += performance_duration
            post_stimulus_duration = self._get_trial_post_stimulus_duration(
                count, trial, block_idx, elapsed
            )
            elapsed += post_stimulus_duration
            self._create_and_add_trial_events(
                block_idx,
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
        self, count: int, trial_spec: TrialSpec, block_idx: int, elapsed: float
    ) -> float:
        if count == len(self.trial_spec_list[block_idx]) - 1:
            if self.pad_block_duration:
                return self.max_block_duration - elapsed
            else:
                return self.timing_params.final_rest_duration
        else:
            return random.uniform(*trial_spec.post_stimulus_duration_range)

    def _create_and_add_trial_events(
        self,
        block_idx: int,
        trial_idx: int,
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
                    block=block_idx,
                    trial=trial_idx,
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
                block=block_idx,
                trial=trial_idx,
                condition=trial_spec.condition,
            ),
            self.trial_mode_topic,
            0.0,
            performance_duration,
        )
        events.add_event_at_end(performance_event, events.last_event_added)

        if self.decoding and block_idx >= self.calibration_blocks:
            # TODO: Add training/feedback events
            pass

        # Post stimulus
        post_stimulus_event = Event(
            DeferredMessage(
                TrialModeMessage,
                trial_mode=TrialMode.POST_STIMULUS,
                block=block_idx,
                trial=trial_idx,
                condition=trial_spec.condition,
            ),
            self.trial_mode_topic,
            0.0,
            post_stimulus_duration,
        )
        events.add_event_at_end(post_stimulus_event, performance_event)
