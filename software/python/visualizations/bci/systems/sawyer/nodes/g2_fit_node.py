#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import logging
import math
import numpy as np
import labgraph as df
from enum import Enum
from scipy.optimize import minimize
from timeit import default_timer as timer
from typing import Callable, Dict

from ..messages import (
    DataMessage,
    G2FitMessage,
    G2Message,
)


class LossType(str, Enum):
    L1 = "least_absolute"
    L2 = "least_square"


class FittingType(str, Enum):
    INTRALIPID = "intralipid"
    ROTATING_PLATE = "rotating_plate"
    SQRT = "sqrt"
    DIFFUSIVE_LARGETAU = "diffusive_largetau"
    BALLISTIC_LARGETAU = "ballistic_largetau"


class G2FitConfig(df.Config):
    """
    Configuration for a G2FitNode
    fitting_type: The fitting function to use for the curve fit
    """

    fitting_type: FittingType = FittingType.SQRT
    loss: LossType = LossType.L2


def intralipid_func(self, tau: float, gamma: float, beta: float):
    return 1 + beta * np.exp(-2 * gamma * tau)


def roting_plate_func(self, tau: float, gamma: float, beta: float):
    return 1 + beta * np.exp(-2 * gamma ** 2 * tau ** 2)


def sqrt_func(self, tau: float, gamma: float, beta: float):
    return 1 + beta * np.exp(-2 * math.sqrt(gamma * tau))


def diffusive_largetau(self, tau: float, gamma: float, beta: float):
    return 1 + beta * np.exp(-2 * math.sqrt(gamma * tau)) * (
        1 + 2 * math.sqrt(gamma * tau)
    )


def ballistic_largetau(self, tau: float, gamma: float, beta: float):
    return 1 + beta * np.exp(-2 * gamma * tau) * (1 + 2 * gamma * tau)


FITTING_FUNCTION_MAP: Dict[FittingType, Callable] = {
    FittingType.INTRALIPID: intralipid_func,
    FittingType.ROTATING_PLATE: roting_plate_func,
    FittingType.SQRT: sqrt_func,
    FittingType.DIFFUSIVE_LARGETAU: diffusive_largetau,
    FittingType.BALLISTIC_LARGETAU: ballistic_largetau,
}


class G2FitNode(df.Node):
    """
    Fit g2 using a specified fitting function
    """

    INPUT = df.Topic(G2Message)
    TAUCORR = df.Topic(DataMessage)
    FIT_ERROR = df.Topic(DataMessage)
    BETA = df.Topic(DataMessage)
    FITTED_CURVE = df.Topic(G2FitMessage)
    PROFILE_FITTING = df.Topic(DataMessage)

    config: G2FitConfig

    @df.subscriber(INPUT)
    @df.publisher(TAUCORR)
    @df.publisher(FIT_ERROR)
    @df.publisher(BETA)
    @df.publisher(FITTED_CURVE)
    @df.publisher(PROFILE_FITTING)
    async def g2_fit(self, message: G2Message) -> df.AsyncPublisher:
        lab_timestamp = float(message.pando_timestamp) * 1e-9
        g2_ensemble = np.mean(message.g2, axis=0)

        try:
            fitted_gamma, fitted_beta, fit_error, time_elapsed = self.fit_curve(
                message.tau, g2_ensemble
            )
        except ValueError:
            logging.warn("Failed to fit g2")
            return

        fitted_curve = G2FitMessage(
            timestamp=message.timestamp,
            channel_map=message.channel_map,
            tau=message.tau,
            g2_mean=np.mean(message.g2, axis=0),
            g2_fit=(
                np.array(
                    [
                        FITTING_FUNCTION_MAP[self.config.fitting_type](
                            self.config, t, fitted_gamma, fitted_beta
                        )
                        for t in message.tau
                    ]
                )
                - 1
            ),
            experiment_id=message.experiment_id,
            sequence_number=message.sequence_number,
            pando_timestamp=message.pando_timestamp,
        )

        yield self.TAUCORR, DataMessage(
            data=((1 / (2 * fitted_gamma)) * 1e6),
            timestamp=lab_timestamp,  # scale to us
            command=message.command,
        )
        yield self.FIT_ERROR, DataMessage(
            data=fit_error, timestamp=lab_timestamp, command=message.command
        )
        yield self.BETA, DataMessage(data=fitted_beta, timestamp=lab_timestamp)
        yield self.FITTED_CURVE, fitted_curve
        yield self.PROFILE_FITTING, DataMessage(
            data=time_elapsed, timestamp=lab_timestamp
        )

    def loss(self, params, tau: np.ndarray, g2: np.ndarray):
        """
        Error function to minimize for curve fit
        """
        fitting_function = FITTING_FUNCTION_MAP[self.config.fitting_type]
        gamma, beta = params

        diff = abs(
            (1 + g2)
            - np.array([fitting_function(self.config, t, gamma, beta) for t in tau])
        )

        if self.config.loss == LossType.L1:
            return np.array(diff.sum())
        elif self.config.loss == LossType.L2:
            return np.array((diff ** 2).sum())
        else:
            raise Exception("Invalid loss type")

    def rmse(self, tau: np.ndarray, g2: np.ndarray, gamma: float, beta: float):
        fitting_function = FITTING_FUNCTION_MAP[self.config.fitting_type]

        return np.sqrt(
            (
                (
                    (1 + g2)
                    - np.array(
                        [fitting_function(self.config, t, gamma, beta) for t in tau]
                    )
                )
                ** 2
            ).mean()
        )

    def fit_curve(self, tau: np.ndarray, g2: np.ndarray):
        """
        Fit the selected g2 function
        """
        start = timer()
        results = minimize(self.loss, [0.1, 0.5], args=(tau, g2))
        end = timer()

        fitted_gamma = float(results["x"][0])
        fitted_beta = float(results["x"][1])

        # You can get the fit error from the specified loss function like this
        # fit_error = results["fun"]

        # Calculate the fit error using root mean squared error
        # regardless of what loss function we used to fit the curve
        fit_error = self.rmse(tau, g2, fitted_gamma, fitted_beta)

        return fitted_gamma, fitted_beta, float(fit_error), (end - start)
