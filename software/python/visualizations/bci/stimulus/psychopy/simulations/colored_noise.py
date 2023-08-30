#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

# Copied from fbcode/frl/bci/synthetic_data/dummy_generator.py

import numpy as np
from numpy import sqrt, newaxis
from numpy import sum as npsum
from numpy.fft import irfft, rfftfreq
from numpy.random import normal


def colored_noise(
    exponent: float, duration: float, sample_rate: float, fmin: int = 0, seed: int = 0
) -> np.ndarray:
    np.random.seed(seed)

    samples = np.arange(0, duration, 1 / sample_rate).shape[0]
    size = [samples]

    # Calculate Frequencies
    # Use fft functions for real output (-> hermitian spectrum)
    f = rfftfreq(samples, d=1.0 / sample_rate)

    # Build scaling factors for all frequencies
    fmin = max(fmin, 1.0 / samples)  # Low frequency cutoff
    idx = npsum(f < fmin)  # Index of the cutoff
    if idx and idx < len(f):
        f[:idx] = f[idx]
    f = f ** (-exponent / 2.0)

    # Calculate theoretical output standard deviation from scaling
    w = f[1:].copy()
    w[-1] *= (1 + (samples % 2)) / 2.0  # correct f = +-0.5
    sigma = 2 * sqrt(npsum(w ** 2)) / samples

    # Adjust size to generate one Fourier component per frequency
    size[-1] = len(f)

    # Add empty dimension(s) to broadcast s_scale along last
    # dimension of generated random power + phase (below)
    dims_to_add = len(size) - 1
    f = f[(newaxis,) * dims_to_add + (Ellipsis,)]

    # Generate scaled random power + phase
    sr = normal(scale=f, size=size)
    si = normal(scale=f, size=size)

    # If the signal length is even, frequencies +/- 0.5 are equal
    # so the coefficient must be real.
    if not (samples % 2):
        si[..., -1] = 0

    # Regardless of signal length, the DC component must be real
    si[..., 0] = 0

    # Combine power + corrected phase to Fourier components
    s = sr + 1j * si

    # Transform to real time series & scale to unit variance
    generated_noise = irfft(s, n=samples, axis=-1) / sigma

    return generated_noise
