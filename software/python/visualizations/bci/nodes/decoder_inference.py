#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import time
from typing import List, Optional, Tuple

import labgraph as df
import joblib
import numpy as np
from bci.nodes.stimulus_windowing import ArrayStimulusWindowMessage
from bci.nodes.window_trigger import make_window_trigger_message
from xplat.frl.bci.ml.hrf_regression import HRFRegressionDecoder, HRFTemplateMatching


class ConditionInferenceMessage(df.TimestampedMessage):
    # Display node need to add checks to make sure
    # this message corresponds to the correct event
    block_number: int
    trial_number: int
    # TODO: change this to the actual stimulus enum used
    conditions: List[str]
    probabilities: List[float]


class DecoderConfig(df.Config):
    model_path: str


class DecoderState(df.State):
    model: Optional[HRFRegressionDecoder] = None
    window_length: Optional[int] = None
    post_onset_length: Optional[int] = None


def get_stimulus_window_params(
    model_path: str,
) -> Tuple[HRFRegressionDecoder, int, int]:
    model = joblib.load(model_path)
    window_length = len(model.regression_timestamps)
    post_onset_length = int((np.array(model.regression_timestamps) > 0).sum())
    return (model, window_length, post_onset_length)


class DecoderInferenceNode(df.Node):
    """
    This is a specific HRF-regression decoder
    inference node, taking in ArrayStimulusWindowMessage.

    If the input to the decoder is a different message type,
    then there needs to be created a new node (possibly
    functionally taking given the input type as a
    parameter)
    """

    INPUT = df.Topic(ArrayStimulusWindowMessage)
    OUTPUT = df.Topic(ConditionInferenceMessage)
    TERMINATION_TOPIC = df.Topic(df.TerminationMessage)

    config: DecoderConfig
    state: DecoderState

    def setup(self) -> None:
        (
            self.state.model,
            self.state.window_length,
            self.state.post_onset_length,
        ) = get_stimulus_window_params(self.config.model_path)
        if self.state.model.template_matching != HRFTemplateMatching.DOT_PRODUCT.value:
            raise ValueError(
                "Unsupported template matching: currently only supporting "
                f"HRFTemplateMatching.DOT_PRODUCT, got {self.model.template_matching}"
            )
        self._construct_hrf_templates()

    @df.subscriber(TERMINATION_TOPIC)
    def terminate(self, message: df.TerminationMessage) -> None:
        raise df.NormalTermination()

    @df.subscriber(INPUT)
    @df.publisher(OUTPUT)
    async def infer(self, window: ArrayStimulusWindowMessage) -> df.AsyncPublisher:
        X = np.stack([message.sample for message in window.sample])
        predicted_hrfs = self.state.model.hrf_regressor.predict(
            X[:, self.state.model.channel_idx_list]
        )
        # columnwise dot product
        features = np.multiply(
            predicted_hrfs.reshape(
                len(X), self.state.model.hrf_regressor.coef_.shape[0],
            ),
            self.hrf_template,
        ).sum(axis=0, keepdims=True)
        condition_probabilities = self.state.model.hrf_classifier.predict_proba(
            features
        )

        """
        Stimulus Window always end at or at some point AFTER
        the last stimulus event, so the last message in the
        stimulus window will always be of the same block and trial
        as the stimulus event
        """
        block_number = window.sample[-1].block_number
        trial_number = window.sample[-1].trial_number
        yield self.OUTPUT, ConditionInferenceMessage(
            time.time(),
            block_number=block_number,
            trial_number=trial_number,
            conditions=self.state.model.condition_list,
            probabilities=condition_probabilities[0],
        )

    def _construct_hrf_templates(self) -> None:
        # placeholder
        template = np.full(self.state.window_length, -1, dtype=np.float64)
        template[len(template) // 2 :] = 1
        self.hrf_template = np.expand_dims(template, -1)
