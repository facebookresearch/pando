#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import argparse
import labgraph as df
import os
import pkg_resources
import shutil
from datetime import datetime
from typing import Optional, Tuple

from bci.libraries.labgraph_viz.plots.common import AutoRange, AxisRange
from .constants import (
    LABGRAPH_RECORDING_NAME,
    SYSTEM_CONFIG_FILENAME,
    USER_CONFIG_FILENAME,
)


logger = df.util.logger.get_logger(__name__)


def _axis_range(arg: str) -> AxisRange:
    range_error = argparse.ArgumentTypeError(
        "range must be either 'auto' or a two-tuple of floats"
    )
    if arg.lower() == "auto":
        return AutoRange()
    try:
        range_ = tuple(float(i) for i in arg.split(","))
    except ValueError:
        raise range_error
    if len(range_) != 2:
        raise range_error
    return range_


def get_argument_parser(usage=None) -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(usage=usage)
    parser.add_argument(
        "--output-dir",
        required=True,
        type=str,
        help="Output directory for serialized topic data",
    )
    parser.add_argument(
        "--participant-id",
        required=True,
        type=str,
        help="Gaia-assigned anonymous participant ID",
    )
    return parser


def add_hardware_config_arguments(
    parser: argparse.ArgumentParser, system: bool = False, user: bool = False
) -> None:
    parser.add_argument(
        "--system-config",
        required=system,
        type=str,
        help="Filename for YAML system configuration for this hardware",
    )
    parser.add_argument(
        "--user-config",
        required=user,
        type=str,
        help="Filename for YAML user configuration for this hardware",
    )
    parser.add_argument(
        "--input-device",
        type=str,
        default=None,
        help="Specify an input device (keyboard, pyka)",
    )


def add_data_collection_arguments(parser: argparse.ArgumentParser, blocks: int) -> None:
    parser.add_argument(
        "--blocks",
        type=int,
        default=blocks,
        help="Number of blocks to run in experiment.",
    )
    parser.add_argument(
        "--short-block",
        action="store_true",
        help="Use short block (<=5min). Number of blocks requested should be doubled",
    )
    parser.add_argument(
        "--no-behavior-feedback",
        action="store_true",
        help="Disable Real-time behavioral feedback.",
    )
    parser.add_argument(
        "--disable-dynamic-phantom",
        action="store_true",
        help="Disable triggers to the dynamic phantom.",
    )
    parser.add_argument(
        "--rest-block",
        action="store_true",
        help="Include a 3 minutes rest block before the performance blocks",
    )
    parser.add_argument(
        "--left-hand",
        action="store_true",
        help="Run experiment with only left hand conditions. Applies to some experiments",
    )
    parser.add_argument(
        "--no-signal-processing",
        action="store_true",
        help="Include this command to disable real-time causal signal processing",
    )


def add_testing_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--mock", action="store_true", help="Mock the hardware system with noise"
    )
    parser.add_argument(
        "--dev",
        action="store_true",
        help="simulate input False by default. Use for development testing",
    )
    parser.add_argument(
        "--windowed",
        action="store_true",
        help="Experiment window fullscreen by default. Flag to make it windowed",
    )
    parser.add_argument(
        "--simulate_input",
        action="store_true",
        help="Input is simulated to advance the experiment",
    )


def add_visualization_arguments(
    parser: argparse.ArgumentParser, y_range: AxisRange
) -> None:
    parser.add_argument(
        "--y-range",
        type=_axis_range,
        default=y_range,
        help="Range of the Y-axis for this plot",
    )


def setup_recording_directory(
    output_dir: str,
    name: str,
    participant_id: str,
    system_config: Optional[Tuple[str, str]] = None,
    user_config: Optional[Tuple[str, str]] = None,
) -> str:
    """
    Creates the recording directory, copies the configs to the
    new directory, and returns the path to the newly set up directory.
    """
    recording_directory = os.path.join(
        output_dir,
        f"{name}_{participant_id}_{datetime.now().strftime('%m-%d-%Y_%Hh%Mm')}",
    )
    os.makedirs(recording_directory, exist_ok=True)
    if not os.path.isdir(recording_directory):
        raise Exception("Unable to create recording dir {}".format(recording_directory))

    assert system_config[1] is not None
    package = system_config[0]
    filename = system_config[1]
    system_config_path = pkg_resources.resource_filename(package, filename)

    shutil.copyfile(
        system_config_path, os.path.join(recording_directory, SYSTEM_CONFIG_FILENAME)
    )

    if user_config is not None:
        package = user_config[0]
        filename = user_config[1]
        user_config_path = pkg_resources.resource_filename(package, filename)
        shutil.copyfile(
            user_config_path, os.path.join(recording_directory, USER_CONFIG_FILENAME)
        )

    logger.info(f"Setting output directory to {recording_directory}")
    return recording_directory


def run_graph(
    graph: df.Graph, recording_directory: str, aligner: Optional[df.Aligner] = None
) -> None:
    options = df.RunnerOptions(
        aligner=aligner,
        logger_config=df.LoggerConfig(
            output_directory=recording_directory,
            recording_name=LABGRAPH_RECORDING_NAME,
        ),
    )
    runner = df.ParallelRunner(graph=graph, options=options)
    runner.run()
