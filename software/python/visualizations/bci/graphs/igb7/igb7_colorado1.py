#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import argparse
import labgraph as df
from typing import Dict, List, Tuple

from bci.systems.colorado import (
    G2EnsembleWindow,
    LabTimeWindow,
    LabTimeWindowConfig,
)
from bci.systems.picoharp import PicoharpDCSGroup, PicoharpConfig
from bci.systems.sawyer.nodes import (
    FittingType,
    G2FitConfig,
    G2FitNode,
    LossType,
)


class GraphConfig(df.Config):
    fitting: FittingType
    loss: LossType
    enable_logging: bool
    tau_range: List[int]
    read_addr: str


class Graph(df.Graph):
    PICOHARP: PicoharpDCSGroup
    G2_FIT: G2FitNode
    G2_ENSEMBLE_WINDOW: G2EnsembleWindow
    LAB_TIME_WINDOW: LabTimeWindow

    config: GraphConfig

    def setup(self) -> None:
        self.PICOHARP.configure(PicoharpConfig(read_addr=self.config.read_addr))
        self.G2_FIT.configure(
            G2FitConfig(fitting_type=self.config.fitting, loss=self.config.loss)
        )
        self.LAB_TIME_WINDOW.configure(
            LabTimeWindowConfig(
                tau_start=self.config.tau_range[0], tau_end=self.config.tau_range[1],
            )
        )

    def process_modules(self) -> Tuple[df.Module, ...]:
        return (
            self.PICOHARP,
            self.G2_FIT,
            self.G2_ENSEMBLE_WINDOW,
            self.LAB_TIME_WINDOW,
        )

    def connections(self) -> df.Connections:
        return (
            # Data
            (self.PICOHARP.G2, self.G2_FIT.INPUT),
            # G2 Ensemble Window
            (self.PICOHARP.G2, self.G2_ENSEMBLE_WINDOW.G2),
            (self.G2_FIT.FIT_ERROR, self.G2_ENSEMBLE_WINDOW.FIT_ERROR),
            (self.G2_FIT.FITTED_CURVE, self.G2_ENSEMBLE_WINDOW.FITTED_CURVE),
            # Lab Time Window
            (self.PICOHARP.COUNT_RATES, self.LAB_TIME_WINDOW.COUNT_RATES),
            (self.PICOHARP.G2, self.LAB_TIME_WINDOW.G2),
            (self.G2_FIT.TAUCORR, self.LAB_TIME_WINDOW.TAUCORR),
        )

    def logging(self) -> Dict[str, df.Topic]:
        if self.config.enable_logging:
            return {
                "gmse": self.G2_FIT.FIT_ERROR,
                "profile_fitting": self.G2_FIT.PROFILE_FITTING,
            }
        return {}


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
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
        "--enable-logging",
        action="store_true",
        help="Log duration of fitting functions and fit error",
        default=False,
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
    args = parser.parse_args()
    graph = Graph()
    graph.configure(
        GraphConfig(
            fitting=args.fitting,
            loss=args.loss,
            enable_logging=args.enable_logging,
            tau_range=[int(t) for t in args.tau_range],
            read_addr=args.read_addr,
        )
    )
    runner = df.ParallelRunner(graph=graph)
    runner.run()
