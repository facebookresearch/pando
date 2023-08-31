# Copyright 2004-present Facebook. All Rights Reserved.

import operator as op
from functools import reduce

import labgraph as df
import numpy as np
import pytest
from bci.nodes.heart_beat import HeartBeatMetricConfig, HeartBeatMetricNode
from bci.nodes.lts import LabeledTimeSeriesMessage
from bci.systems.configuration.system_configuration import SystemConfiguration
from xplat.frl.bci.data.types.time_series import LabeledTimeSeries
from xplat.frl.bci.heart_beat import heart_beat_metric
from xplat.frl.bci.transform.channel_info import system_configuration_to_channel_info


class TestHeartBeatMetricConfig:
    """Test the __post_init__ conditions inside HeartBeatMetricConfig"""

    def test_less_zero_confidence(self) -> None:
        with pytest.raises(ValueError):
            HeartBeatMetricConfig(confidence=0.0)

        with pytest.raises(ValueError):
            HeartBeatMetricConfig(confidence=-1.0)


class TestHeartBeatMetricNode:
    """We want to test that the output from the function matches calling the
    function bare."""

    # Equality assertions on node output message #############################

    def assert_equal_heart_beat_metric(self, metric1, metric2):
        """Assert that each subfield of the metric output matches"""

        def _get_nested_attr(obj, attr_str):
            attr_list = attr_str.split(".")
            return reduce(getattr, attr_list, obj)

        attrs = (
            "fft.bins",
            "fft.spectrum",
            "classification.peak_positions",
            "classification.peak_heights",
            "classification.good_channels",
            "classification.noise_mean",
            "classification.noise_std",
        )

        for field in (lambda obj: _get_nested_attr(obj, attr) for attr in attrs):
            np.testing.assert_allclose(field(metric1), field(metric2))

    def assert_equal_logs(self, log1, log2):
        """Assert that the log statements are the same. The timestamp in the
        log will differ"""
        for l1, l2 in zip(log1, log2):
            assert l1.message == l2.message

    # The main deal ##########################################################

    def test_node(self) -> None:

        # Values you may want to change
        confidence = 3.0
        window_length = 128  # samples

        # Fake system, used to create a `channel_info` object to make a LTS.
        # Channels > 60.0 mm are considered to be noise; we set up some
        # channels below and above this limit to trigger the signal/noise
        # separation function in `heart_beat_metric`
        source = [0.0, 0.0, 0.0]
        near_detector = [10.0, 0.0, 0.0]
        far_detector = [70.0, 0.0, 0.0]
        system_dict = {
            "ConfigVersion": "1.1.0",
            "DeviceName": "simmons",
            "Canisters": [
                {
                    "Name": "Test",
                    "Coordinates": [
                        source,
                        source,
                        near_detector,
                        near_detector,
                        far_detector,
                        far_detector,
                    ],
                    "Sources": {
                        "Wavelengths": {"Red": 680.0, "Infrared": 850.0,},
                        "Red": [{"coordinate": 0}],
                        "Infrared": [{"coordinate": 1}],
                    },
                    "Detectors": [
                        {"coordinate": 2},
                        {"coordinate": 3},
                        {"coordinate": 4},
                        {"coordinate": 5},
                    ],
                }
            ],
        }
        sysconfig = SystemConfiguration(system_dict)
        channel_info = system_configuration_to_channel_info(sysconfig)
        n_channels = len(channel_info)

        # Testing state
        rs = np.random.RandomState()

        # Properties of the time series
        f = 10.0  # Hz, matches Lumo system
        t = np.arange(window_length) / f

        # Pulse properties
        pulse_f = 1.0  # Hz
        pulse_amplitude = 10.0

        # Generate channel_data
        noise = rs.normal(loc=0, scale=1.0, size=(window_length, n_channels))
        pulse = pulse_amplitude * np.sin(2 * np.pi * pulse_f * t)
        signal = reduce(
            op.mul,
            np.meshgrid(pulse, (channel_info.distance < 60.0).astype(np.float64)),
        ).T

        channel_data = signal + noise

        # Generate LTS for the simulated system
        lts = LabeledTimeSeries(t, channel_data, channel_info)
        expected_metric, expected_log = heart_beat_metric(lts, confidence=confidence)

        # Set up the node
        heart_beat_config = HeartBeatMetricConfig(confidence=confidence)
        test_harness = df.NodeTestHarness(HeartBeatMetricNode)

        # Test the node by calling it with repeated streamed inputs, verifying
        # the outputs match the predicted state.
        with test_harness.get_node(config=heart_beat_config) as node:
            msg = LabeledTimeSeriesMessage(timestamp=0.0, sample=lts)
            node_return_msg = df.run_async(node.run_heart_beat_analysis, args=[msg])[0][
                1
            ]
            self.assert_equal_heart_beat_metric(expected_metric, node_return_msg.sample)
            self.assert_equal_logs(expected_log, node_return_msg.log)
