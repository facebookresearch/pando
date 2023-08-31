# Copyright 2004-present Facebook. All Rights Reserved.

from numbers import Number
from typing import List, Tuple, Union

import labgraph as df
import numpy as np
import pytest
from bci.messages.generic_signal_sample import SignalSampleMessage
from bci.nodes.lts import (
    LabeledTimeSeriesConfig,
    LabeledTimeSeriesNode,
)
from bci.systems.configuration.system_configuration import SystemConfiguration
from xplat.frl.bci.data.types.time_series import LabeledTimeSeries
from xplat.frl.bci.transform.channel_info import system_configuration_to_channel_info


State = Union[List[None], LabeledTimeSeries]
NodeReturnState = List[Tuple[df.Topic, LabeledTimeSeries]]


class TestLabeledTimeSeriesConfig:
    """Test the __post_init__ conditions inside LabeledTimeSeriesConfig"""

    def test_less_one_length(self) -> None:
        with pytest.raises(ValueError):
            LabeledTimeSeriesConfig(length=0, stride=1, sysconfig=None)

        with pytest.raises(ValueError):
            LabeledTimeSeriesConfig(length=-1, stride=1, sysconfig=None)

    def test_less_one_stride(self) -> None:
        with pytest.raises(ValueError):
            LabeledTimeSeriesConfig(length=1, stride=0, sysconfig=None)

        with pytest.raises(ValueError):
            LabeledTimeSeriesConfig(length=1, stride=-1, sysconfig=None)


# Parameters derived from a fake, 2 channel system
TEST_SYSTEM_DICT = {
    "ConfigVersion": "1.1.0",
    "DeviceName": "simmons",
    "Canisters": [
        {
            "Name": "Test",
            "Coordinates": [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [2.0, 0.0, 0.0]],
            "Sources": {
                "Wavelengths": {"Red": 680.0, "Infrared": 850.0,},
                "Red": [{"coordinate": 0}],
                "Infrared": [{"coordinate": 1}],
            },
            "Detectors": [{"coordinate": 2}],
        }
    ],
}
TEST_SYSCONFIG = SystemConfiguration(TEST_SYSTEM_DICT)
TEST_CHANNEL_INFO = system_configuration_to_channel_info(TEST_SYSCONFIG)


class TestLabeledTimeSeriesNode:
    """We want to test for two things:

    1. The yields happen after the correct number of messages have been
       supplied, as determined by the window length and stride.
    2. When a message is emitted, it contains the expected data.
    """

    # Equality assertions on node state ######################################

    def assert_equal_lts(
        self, predicted_lts: LabeledTimeSeries, node_lts: LabeledTimeSeries
    ) -> None:
        """Check if two LabeledTimeSeries objects contain the same data. We do
        not test for channel_info, as we don't have a good equality
        definition for that dictionary"""
        attrs = ("timestamps", "channel_data")

        for attr in attrs:
            predicted_attr = getattr(predicted_lts, attr)
            node_attr = getattr(node_lts, attr)

            np.testing.assert_allclose(
                predicted_attr,
                node_attr,
                err_msg=f"{attr} does not match between predicted state and node state",
            )

    def assert_equal_state(self, predicted_state: State, node_state: State) -> None:
        """Test that the predicted and node state are the same. The state can either be

        1. An empty list
        2. A LabeledTimeSeries
        """
        err_msg = (
            f"States are not of the same type: \n"
            f"Predicted state type: {type(predicted_state)}\n"
            f"Node state: {type(node_state)}"
        )

        # Do we have some unknown state from the node?
        if not isinstance(node_state, (list, LabeledTimeSeries)):
            raise ValueError(err_msg)

        if type(predicted_state) != type(node_state):
            raise ValueError(err_msg)

        if isinstance(predicted_state, LabeledTimeSeries):
            self.assert_equal_lts(predicted_state, node_state)

        if isinstance(node_state, list):
            if node_state != []:
                raise ValueError("Node state must be the empty list.")

    # Simulated state progression ############################################

    def in_emit_state(self, length: int, stride: int, t: int) -> bool:
        """Is the current timestep when a message would be emitted?"""
        # If t < length - 1, then no strides have yet been taken.
        if t < length - 1:
            return False

        # We have started striding
        if (t - length) % stride:
            return True
        else:
            return False

    def predict_state(self, length, stride, t, channel_value_offset, channel_info):
        """If we are in an emit state, return a LabeledTimeSeries. Otherwise
        return []"""
        if self.in_emit_state(length, stride, t):
            n_channels = len(channel_info)
            timestamps = np.arange(t - length + 1, t + 1)
            data = (np.tile(timestamps, [n_channels, 1])).T + channel_value_offset
            return LabeledTimeSeries(timestamps, data, channel_info)
        else:
            return []

    # Node input and unpacking ###############################################

    def unpack_node_state(self, state: NodeReturnState) -> State:
        """The results from the NodeTestHarness are either

        1. [] for the case that no yield was performed
        2. A singleton [(Topic, LabeledTimeSeriesMessage)]. We unpack the
        sample contained in the LabeledTimeSeriesMessage to compare the state
        of the sample.
        """
        if len(state) == 0:
            return []
        else:
            assert state[0][1].timestamp not in state[0][1].sample.timestamps
            return state[0][1].sample

    def create_signal_sample_message(
        self, timestamp: int, sample: List[Number]
    ) -> SignalSampleMessage:
        return SignalSampleMessage(
            sample_timestamp=np.nan, timestamp=float(timestamp), sample=np.array(sample)
        )

    def create_filtered_sample_message(
        self, sample_timestamp: int, sample: List[Number]
    ) -> SignalSampleMessage:
        """
        Assume 0.1s for the "filtering" lag for the
        current filtered sample message
        """
        return SignalSampleMessage(
            sample_timestamp=float(sample_timestamp),
            timestamp=float(sample_timestamp) + 0.1,
            sample=np.array(sample),
        )

    # The main deal ##########################################################

    def test_node(self) -> None:

        # Values you may want to change
        length = 3
        stride = 2
        channel_value_offset = 10.0
        windows_to_emit = 10
        n_channels = len(TEST_CHANNEL_INFO)

        # Set up the node
        lts_config = LabeledTimeSeriesConfig(
            length=length, stride=stride, sysconfig=TEST_SYSCONFIG
        )
        test_harness = df.NodeTestHarness(LabeledTimeSeriesNode)

        # Test the node by calling it with repeated streamed inputs, verifying
        # the outputs match the predicted state.
        with test_harness.get_node(config=lts_config) as node:
            for t in range(length + stride * windows_to_emit):
                value = t + channel_value_offset
                msg = self.create_signal_sample_message(t, [value] * n_channels)

                node_state = self.unpack_node_state(
                    df.run_async(node.window, args=[msg])
                )
                predicted_state = self.predict_state(
                    length, stride, t, channel_value_offset, TEST_CHANNEL_INFO
                )
                self.assert_equal_state(predicted_state, node_state)

    def test_node_filtered_sample(self) -> None:

        # Values you may want to change
        length = 3
        stride = 2
        channel_value_offset = 10.0
        windows_to_emit = 10
        n_channels = len(TEST_CHANNEL_INFO)

        # Set up the node
        lts_config = LabeledTimeSeriesConfig(
            length=length, stride=stride, sysconfig=TEST_SYSCONFIG
        )
        test_harness = df.NodeTestHarness(LabeledTimeSeriesNode)

        # Test the node by calling it with repeated streamed inputs, verifying
        # the outputs match the predicted state.
        with test_harness.get_node(config=lts_config) as node:
            for t in range(length + stride * windows_to_emit):
                value = t + channel_value_offset
                msg = self.create_filtered_sample_message(t, [value] * n_channels)

                node_state = self.unpack_node_state(
                    df.run_async(node.window, args=[msg])
                )
                predicted_state = self.predict_state(
                    length, stride, t, channel_value_offset, TEST_CHANNEL_INFO
                )
                self.assert_equal_state(predicted_state, node_state)
