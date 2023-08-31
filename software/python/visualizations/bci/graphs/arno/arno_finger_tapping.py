#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
from textwrap import dedent
from typing import Dict, List, Tuple

from bci.graphs.common import (
    add_data_collection_arguments,
    add_hardware_config_arguments,
    add_testing_arguments,
    get_argument_parser,
    run_graph,
    setup_recording_directory,
)
from bci.graphs.constants import (  # Experiment Data; Optical Data; Sync Box
    BEHAVIORAL_FEEDBACK_HDF5_NAME,
    COUNT_RATES_HDF5_NAME,
    DIGITAL_IN_HDF5_NAME,
    EXPERIMENT_INFO_HDF5_NAME,
    FINGER_TAP_EVENTS_HDF5_NAME,
    G2_HDF5_NAME,
    GENERATED_EVENTS_HDF5_NAME,
    HRM_HDF5_NAME,
    IMU_HDF5_NAME,
    KEYBOARD_INPUT_HDF5_NAME,
    POWER_METER_HDF5_NAME,
    PULSE_OX_HDF5_NAME,
    RESP_BELT_HDF5_NAME,
    VIEW_CHANGE_HDF5_NAME,
)
from bci.groups.variable_duration_finger_tapping import (
    VariableDurationFingerTappingConfig,
    VariableDurationFingerTappingGroup,
)
from bci.stimulus.psychopy.config import InputDevice
from bci.stimulus.psychopy.protocols.finger_tapping_duration import (
    RANDOMIZED_VARIABLE_DURATION_FINGER_TAPPING_PHOTODIODE_PROTOCOL,
)
from bci.systems.pandoboxd import PandoboxdGroup
from bci.systems.pandoboxd.viz import HeartRateVizGroup
from bci.systems.colorado import (
    CountRateWindow,
    G2EnsembleWindow,
    LabTimeWindow,
    LabTimeWindowConfig,
)
from bci.systems.colorado.common import add_colorado_arguments
from bci.systems.pf32 import Pf32Group
from bci.systems.sawyer.nodes import (
    FittingType,
    G2FitConfig,
    G2FitNode,
    LossType,
)


class GraphConfig(df.Config):
    fitting: FittingType
    loss: LossType
    tau_range: List[int]
    read_addr: str


class ArnoFingerTappingConfig(df.Config):
    yaml_filename: str
    stimulus_config: VariableDurationFingerTappingConfig
    colorado_config: GraphConfig


class Graph(df.Graph):
    CLEMENTINE: PandoboxdGroup
    PF32: Pf32Group
    G2_FIT: G2FitNode
    COUNT_RATE_WINDOW: CountRateWindow
    G2_ENSEMBLE_WINDOW: G2EnsembleWindow
    LAB_TIME_WINDOW: LabTimeWindow
    HEART_RATE_VIZ: HeartRateVizGroup
    STIMULUS_GROUP: VariableDurationFingerTappingGroup

    config: ArnoFingerTappingConfig

    def setup(self) -> None:
        self.PF32.configure(Pf32Config(read_addr=self.config.colorado_config.read_addr))
        self.CLEMENTINE.configure(
            PandoboxdConfig(read_addr=self.config.colorado_config.read_addr)
        )
        self.STIMULUS_GROUP.configure(self.config.stimulus_config)
        self.G2_FIT.configure(
            G2FitConfig(
                fitting_type=self.config.colorado_config.fitting,
                loss=self.config.colorado_config.loss,
            )
        )
        self.LAB_TIME_WINDOW.configure(
            LabTimeWindowConfig(
                tau_start=self.config.colorado_config.tau_range[0],
                tau_end=self.config.colorado_config.tau_range[1],
            )
        )

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (
            self.CLEMENTINE,
            self.PF32,
            self.G2_FIT,
            self.COUNT_RATE_WINDOW,
            self.G2_ENSEMBLE_WINDOW,
            self.LAB_TIME_WINDOW,
            self.HEART_RATE_VIZ,
            self.STIMULUS_GROUP,
        )

    def connections(self) -> df.Connections:
        return (
            # Data
            (self.PF32.G2, self.G2_FIT.INPUT),
            # Count Rate Window
            (self.PF32.COUNT_RATES, self.COUNT_RATE_WINDOW.COUNT_RATES),
            # G2 Ensemble Window
            (self.PF32.G2, self.G2_ENSEMBLE_WINDOW.G2),
            (self.G2_FIT.FIT_ERROR, self.G2_ENSEMBLE_WINDOW.FIT_ERROR),
            (self.G2_FIT.FITTED_CURVE, self.G2_ENSEMBLE_WINDOW.FITTED_CURVE),
            # Lab Time Window
            (self.PF32.COUNT_RATES, self.LAB_TIME_WINDOW.COUNT_RATES),
            (self.PF32.G2, self.LAB_TIME_WINDOW.G2),
            (self.G2_FIT.TAUCORR, self.LAB_TIME_WINDOW.TAUCORR),
            # Pandoboxd
            (self.CLEMENTINE.HRM, self.HEART_RATE_VIZ.HRM),
            (self.CLEMENTINE.PULSE_OX, self.HEART_RATE_VIZ.PULSE_OX),
            (self.CLEMENTINE.RESP_BELT, self.HEART_RATE_VIZ.RESP_BELT),
        )

    def logging(self) -> Dict[str, df.Topic]:
        return {
            # Experiment Data
            EXPERIMENT_INFO_HDF5_NAME: self.STIMULUS_GROUP.DISPLAY.EXPERIMENT_INFO_TOPIC,
            FINGER_TAP_EVENTS_HDF5_NAME: self.STIMULUS_GROUP.KEY_MAPPING.FINGER_TAP_TOPIC,
            KEYBOARD_INPUT_HDF5_NAME: self.STIMULUS_GROUP.DISPLAY.KEYBOARD_INPUT_TOPIC,
            GENERATED_EVENTS_HDF5_NAME: self.STIMULUS_GROUP.EVENTS.TRIAL_MODE_TOPIC,
            VIEW_CHANGE_HDF5_NAME: self.STIMULUS_GROUP.DISPLAY.VIEW_CHANGE_TOPIC,
            BEHAVIORAL_FEEDBACK_HDF5_NAME: self.STIMULUS_GROUP.BEHAVIORAL_FEEDBACK.BEHAVIORAL_FEEDBACK_TOPIC,
            # Optical Data
            COUNT_RATES_HDF5_NAME: self.PF32.COUNT_RATES,
            G2_HDF5_NAME: self.PF32.G2,
            # Sync Box Data
            DIGITAL_IN_HDF5_NAME: self.CLEMENTINE.DIGITAL_IN,
            HRM_HDF5_NAME: self.CLEMENTINE.HRM,
            IMU_HDF5_NAME: self.CLEMENTINE.IMU,
            PULSE_OX_HDF5_NAME: self.CLEMENTINE.PULSE_OX,
            RESP_BELT_HDF5_NAME: self.CLEMENTINE.RESP_BELT,
            POWER_METER_HDF5_NAME: self.CLEMENTINE.POWER_METER,
        }


if __name__ == "__main__":
    usage = dedent(
        """
        Example command:
          python -m bci.graphs.arno.arno_finger_tapping --
          --output-dir /Users/$USERNAME/Documents/arno_output_data
          --participant-id ODP663
          --system-config arno-placeholder.yaml
          --blocks 8
          --fitting sqrt
          --loss least_square
    """
    )
    parser = get_argument_parser(usage=usage)
    add_hardware_config_arguments(parser, system=True)
    add_data_collection_arguments(parser, blocks=8)
    add_testing_arguments(parser)
    add_colorado_arguments(parser)
    args = parser.parse_args()

    if args.input_device:
        args.input_device = InputDevice[args.input_device.upper()]

    graph = Graph()
    colorado_config = GraphConfig(
        fitting=args.fitting,
        loss=args.loss,
        tau_range=[int(t) for t in args.tau_range],
        read_addr=args.read_addr,
    )
    stimulus_config = VariableDurationFingerTappingConfig(
        development=args.dev,
        simulate_input=args.simulate_input,
        blocks=args.blocks,
        fullscreen=not args.windowed,
        protocol_spec=RANDOMIZED_VARIABLE_DURATION_FINGER_TAPPING_PHOTODIODE_PROTOCOL,
        participant_id=args.participant_id.lower(),
        no_behavior_feedback=args.no_behavior_feedback,
        input_device=args.input_device,
    )
    graph.configure(
        ArnoFingerTappingConfig(
            yaml_filename=args.system_config,
            stimulus_config=stimulus_config,
            colorado_config=colorado_config,
        )
    )
    recording_dir = setup_recording_directory(
        args.output_dir,
        "arnofingertapping",
        args.participant_id.lower(),
        system_config=("bci.systems.colorado2.configuration", args.system_config),
    )

    run_graph(graph, recording_dir)
