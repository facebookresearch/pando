#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

# Colors
BLACK = (-1, -1, -1)
BLUE = (-1, -1, 1)
GRAY = (-0.25, -0.25, -0.25)
GREEN = (-1, 1, -1)
RED = (1, -1, -1)
WHITE = (1, 1, 1)

# DisplayConfig constants
BLOCK_DONE_INSTRUCTIONS = (
    "Block {} / {} done. \n\n"
    "Let the experimenter know when you are ready to continue."
)
BLOCK_DONE_INSTRUCTIONS_AT_HOME = (
    "Block {} / {} done. \n\n" "Press the spacebar to continue."
)
BLOCK_INFORMATION = "Block {}"
EXPERIMENT_DONE_INSTRUCTIONS = "Experiment complete. \n\n Great job!"
EXPERIMENT_DONE_INSTRUCTIONS_AT_HOME = (
    "Experiment complete. \n\n" "Great job! \n\n" "Press the spacebar to exit."
)
EXPERIMENT_INFO = (
    "Get ready ... \n\n"
    "This experiment will take about {:.2f} min.\n\n"
    "Let the experimenter know when you are ready."
)
EXPERIMENT_INFO_AT_HOME = (
    "Get ready ... \n\n"
    "This experiment will take about {:.2f} min.\n\n"
    "Press the spacebar to proceed."
)
FULLSCREEN_SIZE = (1920, 1080)
IMAGE_SIZE = (300, 300)
MONITOR_DISTANCE = 57  # Distance in cm
MONITOR_WIDTH = 33.1
SPACEBAR_KEY = "space"
PROTOCOL_INSTRUCTIONS = (
    "Reminder: Try your best to stay still within a block.\n\n"
    "You will have a break at the end of each block."
)
PROTOCOL_INSTRUCTIONS_AT_HOME = (
    "Reminders: \n\n"
    "- Try your best to stay still during the block.\n\n"
    "- Tap your fingers in the following order: "
    "thumb, index, middle, ring, pinky. \n\n"
    "- Attend to each finger as you tap it.\n\n"
    "- You will have a break at the end of each block.\n\n"
    "Press the spacebar to begin."
)
WINDOW_SIZE = (1280, 720)

# TrialSpec constants
DEFAULT_PRE_STIMULUS_DURATION_RANGE = (6.0, 12.0)
DEFAULT_PERFORMANCE_DURATION_RANGE = (6.0, 6.5)
DEFAULT_POST_STIMULUS_DURATION_RANGE = (0.0, 0.0)

# TimingParams constants
DEFAULT_BLOCK_NUMBER_DURATION = 2.0
