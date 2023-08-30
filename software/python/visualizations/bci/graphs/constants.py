#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

LABGRAPH_RECORDING_NAME = "recording-data"
SYSTEM_CONFIG_FILENAME = "system-config.yaml"
USER_CONFIG_FILENAME = "user-config.yaml"

# need to figure how to make these maximally re-usable / consistent
# across graphs
OPTICAL_RAW_MEASUREMENT_HDF5_NAME = "raw_sample"
OPTICAL_MEASUREMENT_HDF5_NAME = "demod_sample"
EXPERIMENT_INFO_HDF5_NAME = "experiment_info"
FINGER_TAP_EVENTS_HDF5_NAME = "finger_tap_events"
KEYBOARD_INPUT_HDF5_NAME = "keyboard_input"
GENERATED_EVENTS_HDF5_NAME = "generated_events"
VIEW_CHANGE_HDF5_NAME = "view_change"
BEHAVIORAL_FEEDBACK_HDF5_NAME = "behavioral_feedback"
PHANTOM_TRIGGER_HDF5_NAME = "phantom_trigger"

# Pando data
COUNT_RATES_HDF5_NAME = "count_rates"
CRI_HDF5_NAME = "cri"
DTOF_HDF5_NAME = "dtof"
G2_HDF5_NAME = "g2"

# Sync Box data
DIGITAL_IN_HDF5_NAME = "digital_in"
HRM_HDF5_NAME = "hrm"
IMU_HDF5_NAME = "imu"
PULSE_OX_HDF5_NAME = "pulse_ox"
RESP_BELT_HDF5_NAME = "resp_belt"
POWER_METER_HDF5_NAME = "power_meter"

# Lumo data
SAMPLES_NAME = "samples"

# Causal faucet data
OPTICAL_TO_FAUCET_HDF5_NAME = "prefiltered_sample"
FILTERED_SAMPLE_HDF5_NAME = "filtered_sample"
BANDPASS_SAMPLE_HDF5_NAME = "bandpass_sample"
HINF_SAMPLE_HDF5_NAME = "hinf_sample"
