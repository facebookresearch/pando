#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import argparse

from bci.systems.sawyer.nodes import FittingType, LossType


def add_colorado_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--fitting",
        type=FittingType,
        help="Fitting function used to fit g2 (intralipid, rotating_plate, sqrt, diffusive_largetau, ballistic_largetau)",
        default="sqrt",
    )
    parser.add_argument(
        "--loss",
        type=LossType,
        help="Loss function used to fit g2 (least_absolute, least_square)",
        default="least_square",
    )
    parser.add_argument(
        "--tau-range",
        nargs=2,
        help="The tau range to use for tau modulation visualization",
        default=[90, 200],
    )
    parser.add_argument(
        "--read-addr",
        help="The ZMQ address to read data from",
        default="tcp://127.0.0.1:5060",
    )
