#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import numpy as np
import scipy
import scipy.special
import scipy.stats


# Copied from fbcode/frl/bci/dot/hemodynamics.py
def gamma_fun(t, alpha, beta):
    n = scipy.special.gamma(alpha)
    return t ** (alpha - 1) * beta ** (alpha) * np.exp(-beta * t) / n


def eval_canonical_hrf(t, beta=1):
    return gamma_fun(np.fmax(t, 0), 6 * beta, beta) - 1 / 6.0 * gamma_fun(
        np.fmax(t, 0), 16, 1
    )


def eval_canonical_hrf_and_derivatives(t):
    # Canonical hemodynamic response in SPM.
    # https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3318970/#FD7
    delta = 0.1
    hrfs = [
        eval_canonical_hrf(t),
        (eval_canonical_hrf(t + delta) - eval_canonical_hrf(t - delta)) / 2 / delta,
        (eval_canonical_hrf(t, 1 + delta) - eval_canonical_hrf(t, 1 - delta))
        / 2
        / delta,
    ]

    return np.array(hrfs).T


# Copied from fbcode/frl/bci/synthetic_data/dummy_generator.py
def gamma_shape_rate(x: np.ndarray, shape: float, rate: float) -> np.ndarray:
    """
    scipy.stats.gamma is parameterized by shape and scale=1/rate,
    we flip this for convenience
    """
    scale = 1 / rate
    return scipy.stats.gamma.pdf(x, a=shape, scale=scale)


def double_gamma_hrf(
    t: float,
    A: float,
    alpha1: float = 6.0,
    alpha2: float = 16.0,
    beta1: float = 1,
    beta2: float = 1,
    c: float = 1 / 6,
) -> np.ndarray:
    """
    HRF as linear combination of two gamma distributions
    Default parameters match:
        https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3318970/#FD7
    """
    return A * (
        gamma_shape_rate(t, alpha1, beta1) - c * gamma_shape_rate(t, alpha2, beta2)
    )


def simulated_hemodynamics(
    amplitude: float, duration: float, sample_rate: float, pad_len: int = 0
) -> np.ndarray:
    """
    Generate simulated hemodynamics using double-gamma HRF over
    a particular window and sample rate

    Args:
        - pad_len: How many samples to shift the HRF.  Defaults to 0.
    """
    ts = np.arange(0, duration, 1 / sample_rate)
    # canonical HRF is in seconds
    hrf = double_gamma_hrf(ts, amplitude)
    # shift the initiation point
    # only left padding to shift with constant 0
    hrf = np.pad(hrf, (pad_len, 0), "constant")[: int(duration * sample_rate)]

    return hrf
