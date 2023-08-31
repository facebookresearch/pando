#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.


import asyncio
import logging
import threading
import time
from collections import deque
from dataclasses import dataclass
from typing import Any, Dict, Optional

from bci.stimulus.psychopy.config import DisplayConfig, ImageProperties
from bci.stimulus.psychopy.constants import (
    MINIMUM_SLEEP_TIME,
    BehavioralFeedbackStatus,
    Instructions,
    TrialMode,
    ViewChangeType,
)
from bci.stimulus.psychopy.messages import (
    BehavioralFeedbackMessage,
    BlockMessage,
    ExperimentInfoMessage,
    InstructionsMessage,
    KeyboardInputMessage,
    TrialModeMessage,
    ViewChangeMessage,
)
from bci.stimulus.psychopy.util import (
    ClockConverter,
    DrawingManager,
    StimInfo,
    StimType,
)
from labgraph import NormalTermination
from labgraph.events import (
    TerminationMessage,
    WaitBeginMessage,
    WaitEndMessage,
)
from labgraph.graphs import (
    AsyncPublisher,
    Node,
    Topic,
    main,
    publisher,
    subscriber,
)
from psychopy import event, logging as psychopy_logging, monitors, visual


BLACK = (-1, -1, -1)
WHITE = (1, 1, 1)


@dataclass
class ViewChangeParameters:
    block: int
    trial: int
    condition: Any
    trial_mode: TrialMode
    change_type: ViewChangeType
    rendered_at: Optional[float] = None


class DisplayNode(Node):
    BEHAVIORAL_FEEDBACK_TOPIC = Topic(BehavioralFeedbackMessage)
    BLOCK_TOPIC = Topic(BlockMessage)
    EXPERIMENT_INFO_TOPIC = Topic(ExperimentInfoMessage)
    INSTRUCTIONS_TOPIC = Topic(InstructionsMessage)
    KEYBOARD_INPUT_TOPIC = Topic(KeyboardInputMessage)
    TERMINATION_TOPIC = Topic(TerminationMessage)
    TRIAL_MODE_TOPIC = Topic(TrialModeMessage)
    VIEW_CHANGE_TOPIC = Topic(ViewChangeMessage)
    WAIT_BEGIN_TOPIC = Topic(WaitBeginMessage)
    WAIT_END_TOPIC = Topic(WaitEndMessage)

    def __init__(self) -> None:
        super().__init__()
        self._clock_converter = None
        self._shutdown = None
        self._received_input = None
        self._config = None
        self._display_size = None
        self._wait_start = None
        self._keys = []
        self._keys_available = None
        self._drawing_manager_lock = None
        self._view_changes = []
        self._view_changes_lock = None
        # Experiment state
        self._total_blocks = None
        # Trial info
        self._block = -1
        self._trial = -1
        self._condition = None
        self._trial_mode = TrialMode.UNDEFINED
        self._drawing_manager = None
        # Photodiode
        self._photodiode_stim = None
        # Decoder feedback
        self._decoder_feedback = None
        self._decoder_feedback_queue = deque()
        # tracks number of feedback for current trial
        self._cur_trial_feedback_num = 0

    # TODO: Subscribe to "motion" topic

    def setup(self) -> None:
        self._clock_converter = ClockConverter()
        psychopy_logging.setDefaultClock(self._clock_converter.clock)
        self._shutdown = asyncio.Event()
        self._received_input = asyncio.Event()
        self._keys_available = asyncio.Event()
        self._drawing_manager_lock = threading.Lock()
        self._view_changes_lock = threading.Lock()
        self._drawing_manager = DrawingManager()

    def set_config(self, config: DisplayConfig) -> None:
        self._config = config
        self._display_size = (
            config.fullscreen_size if config.fullscreen else config.window_size
        )
        self._condition = self._config.stimulus_conditions.UNDEFINED

    def _open_window(self) -> visual.Window:
        monitor = monitors.Monitor(
            "BCIMonitor",
            width=self._config.monitor_width,
            distance=self._config.monitor_distance,
        )
        monitor.setSizePix(self._display_size)
        return visual.Window(
            monitor=monitor,
            size=self._display_size,
            units="pix",
            color=self._config.background_color,
            fullscr=self._config.fullscreen,
        )

    def _setup_stims(
        self, window: visual.Window
    ) -> Dict[StimType, visual.BaseVisualStim]:
        fixation_stim = visual.TextStim(
            window,
            text="+",
            color=self._config.fixation_cross_color,
            pos=(0, 0),
            height=100,
        )
        image_stim = visual.ImageStim(
            window, image=None, units="pix", size=self._config.image_size, pos=(0, 0)
        )
        text_stim = visual.TextStim(
            window, text="", color=self._config.foreground_color, pos=(0, 0)
        )
        incorrect_stim = visual.TextStim(
            window,
            text="X",
            color=self._config.incorrect_behavior_color,
            pos=(0, -100),
            height=100,
        )
        if self._config.photodiode_enabled:
            self._photodiode_stim = visual.Rect(
                window,
                width=40,
                height=40,
                color=BLACK,
                pos=(
                    (self._display_size[0] - 40) / 2,
                    (self._display_size[1] - 40) / 2,
                ),
            )
            self._photodiode_stim.autoDraw = True
        return {
            StimType.FIXATION: fixation_stim,
            StimType.IMAGE: image_stim,
            StimType.TEXT: text_stim,
            StimType.INCORRECT: incorrect_stim,
        }

    def _get_fixation_stim_info(self) -> StimInfo:
        return StimInfo(type_=StimType.FIXATION, attrs={})

    def _get_image_stim_info(self, image_properties: ImageProperties) -> StimInfo:
        return StimInfo(
            type_=StimType.IMAGE,
            attrs={"image": image_properties.image, "pos": image_properties.position},
        )

    def _get_text_stim_info(self, text: str) -> StimInfo:
        return StimInfo(type_=StimType.TEXT, attrs={"text": text})

    def _get_incorrect_stim_info(self) -> StimInfo:
        return StimInfo(type_=StimType.INCORRECT, attrs={})

    @main
    def render(self):
        view_changed = None
        window = self._open_window()
        stims = self._setup_stims(window)
        while not self._shutdown.is_set():
            # TODO: Use KeyboardInputNode for this
            if not self._keys_available.is_set():
                self._keys = event.getKeys(timeStamped=self._clock_converter.clock)
                self._keys_available.set()
            with self._drawing_manager_lock:
                if self._drawing_manager.is_dirty:
                    for stim in stims.values():
                        stim.autoDraw = False
                    for stim_info in self._drawing_manager.current_view:
                        stim = stims[stim_info.type_]
                        for attr, value in stim_info.attrs.items():
                            setattr(stim, attr, value)
                        stim.autoDraw = True
                    view_changed = ViewChangeParameters(
                        block=self._block,
                        trial=self._trial,
                        condition=self._condition,
                        trial_mode=self._trial_mode,
                        change_type=self._drawing_manager.view_change_type,
                    )
            rendered_at = window.flip()
            if view_changed is not None:
                view_changed.rendered_at = self._clock_converter.to_system(rendered_at)
                with self._view_changes_lock:
                    self._view_changes.append(view_changed)
                view_changed = None
        window.close()

    @subscriber(EXPERIMENT_INFO_TOPIC)
    def experiment_info(self, message: ExperimentInfoMessage) -> None:
        self._total_blocks = message.total_blocks
        expected_experiment_duration = message.expected_experiment_duration
        # TODO: Make this less rigid:
        # - Don't `.format()` strings here
        with self._drawing_manager_lock:
            self._drawing_manager.replace_view(
                [
                    self._get_text_stim_info(
                        self._config.experiment_info.format(
                            expected_experiment_duration / 60
                        )
                    )
                ]
            )

    @subscriber(BLOCK_TOPIC)
    def block(self, message: BlockMessage) -> None:
        self._block = message.block
        # TODO: Make this less rigid:
        # - Don't `.format()` strings here
        with self._drawing_manager_lock:
            self._drawing_manager.replace_view(
                [
                    self._get_text_stim_info(
                        self._config.block_information.format(self._block + 1)
                    )
                ]
            )

    @subscriber(INSTRUCTIONS_TOPIC)
    def instructions(self, message: InstructionsMessage) -> None:
        # TODO: Make this less rigid:
        # - Don't `.format()` strings here
        if message.instructions == Instructions.PROTOCOL:
            self._drawing_manager.replace_view(
                [self._get_text_stim_info(self._config.protocol_instructions)]
            )
        elif message.instructions == Instructions.KEY_MAPPING:
            self._drawing_manager.replace_view(
                [self._get_text_stim_info(self._config.key_mapping_instructions)]
            )

    @subscriber(TRIAL_MODE_TOPIC)
    def trial_mode(self, message: TrialModeMessage) -> None:
        if message.block != self._block or message.trial != self._trial:
            # we have a new trial!
            self._cur_trial_feedback_num = 0
            self._decoder_feedback_queue.clear()
            # not clearing _decoder_feedback because feedback from
            # last trial can still be displayed right now
            # responsibility for other nodes to turn off previous feedback!

        self._block = message.block
        self._trial = message.trial
        self._condition = message.condition
        self._trial_mode = message.trial_mode
        # TODO: Make this less rigid:
        # - More flexibility for mode-to-stim_type
        if (
            message.trial_mode == TrialMode.PRE_STIMULUS
            or message.trial_mode == TrialMode.POST_STIMULUS
        ):
            with self._drawing_manager_lock:
                self._drawing_manager.replace_view(
                    [self._get_fixation_stim_info()], ViewChangeType.STIMULUS
                )
                if self._config.photodiode_enabled:
                    self._photodiode_stim.color = BLACK
        if message.trial_mode == TrialMode.PERFORMANCE:
            with self._drawing_manager_lock:
                self._drawing_manager.replace_view(
                    [
                        self._get_image_stim_info(
                            self._config.stimulus_images[self._condition]
                        )
                    ],
                    ViewChangeType.STIMULUS,
                )
                if self._config.photodiode_enabled:
                    self._photodiode_stim.color = WHITE
        if message.trial_mode == TrialMode.BLOCK_DONE:
            if self._block + 1 == self._total_blocks:
                display_instructions = self._config.experiment_done_instructions
            else:
                display_instructions = self._config.block_done_instructions
            self._drawing_manager.replace_view(
                [
                    self._get_text_stim_info(
                        display_instructions.format(self._block + 1, self._total_blocks)
                    )
                ]
            )
            if self._config.photodiode_enabled:
                self._photodiode_stim.color = BLACK

    @subscriber(WAIT_BEGIN_TOPIC)
    def wait_for_input(self, message: WaitBeginMessage) -> None:
        self._wait_start = time.time()

    @subscriber(TERMINATION_TOPIC)
    def terminate(self, message: TerminationMessage) -> None:
        self._terminate()

    def _terminate(self) -> None:
        self._shutdown.set()
        self._wait_start = None
        self._received_input.set()
        raise NormalTermination()

    @publisher(VIEW_CHANGE_TOPIC)
    async def view_change(self) -> AsyncPublisher:
        while not self._shutdown.is_set():
            with self._view_changes_lock:
                for view_change_parameters in self._view_changes:
                    yield (
                        self.VIEW_CHANGE_TOPIC,
                        ViewChangeMessage(
                            block=view_change_parameters.block,
                            trial=view_change_parameters.trial,
                            condition=view_change_parameters.condition,
                            trial_mode=view_change_parameters.trial_mode,
                            rendered_at=view_change_parameters.rendered_at,
                            timestamp=time.time(),
                            change_type=view_change_parameters.change_type,
                        ),
                    )
                self._view_changes.clear()
            await asyncio.sleep(MINIMUM_SLEEP_TIME)

    @publisher(KEYBOARD_INPUT_TOPIC)
    async def keyboard_input(self) -> AsyncPublisher:
        while not self._shutdown.is_set():
            if self._keys_available.is_set():
                for keypress in self._keys:
                    keypress_name = keypress[0]
                    keypress_tDown = keypress[1]
                    if keypress_name.lower() == self._config.exit_key:
                        self._terminate()
                    if (
                        self._wait_start is not None
                        and keypress_name == self._config.continue_key
                    ):
                        self._received_input.set()
                    yield (
                        self.KEYBOARD_INPUT_TOPIC,
                        KeyboardInputMessage(
                            key=keypress_name,
                            timestamp=self._clock_converter.to_system(keypress_tDown),
                        ),
                    )
                self._keys = []
                self._keys_available.clear()
            await asyncio.sleep(MINIMUM_SLEEP_TIME)

    @publisher(WAIT_END_TOPIC)
    async def wait_end(self) -> AsyncPublisher:
        while not self._shutdown.is_set():
            while not self._received_input.is_set():
                await asyncio.sleep(MINIMUM_SLEEP_TIME)
            if self._wait_start is not None:
                now = time.time()
                wait_length = now - self._wait_start
                yield (
                    self.WAIT_END_TOPIC,
                    WaitEndMessage(wait_length=wait_length, timestamp=now),
                )
            self._wait_start = None
            self._received_input.clear()

    @subscriber(BEHAVIORAL_FEEDBACK_TOPIC)
    def behavioral_feedback(self, message: BehavioralFeedbackMessage) -> None:
        if not self._config.no_behavior_feedback:
            if message.status == BehavioralFeedbackStatus.INCORRECT:
                with self._drawing_manager_lock:
                    self._drawing_manager.add_to_view(
                        [self._get_incorrect_stim_info()],
                        ViewChangeType.BEHAVIORAL_FEEDBACK,
                    )
            if (
                message.status == BehavioralFeedbackStatus.RESET
                or message.status == BehavioralFeedbackStatus.CORRECT
            ):
                with self._drawing_manager_lock:
                    self._drawing_manager.remove_from_view(
                        [self._get_incorrect_stim_info()],
                        ViewChangeType.BEHAVIORAL_FEEDBACK,
                    )
