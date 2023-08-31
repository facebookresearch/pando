#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui

from bci.systems.common import PyQtApplication, PyQtApplicationConfig
from bci.systems.sawyer.messages import CountRateMessage, DataMessage, G2Message
from bci.systems.sawyer.nodes import (
    CountRateEnsembleNode,
    G2ModulationEnsembleNode,
    G2ModulationEnsembleConfig,
    G2TauZeroEnsembleNode,
)
from bci.libraries.labgraph_viz.plots import (
    CurveStyle,
    LinePlot,
    LinePlotConfig,
    Mode,
)


class LabTimeWindowConfig(df.Config):
    """
    Config for LabTimeWindow
    tau_start: Start tau value for g2 modulation
    tau_end: End tau value for g2 modulation
    """

    tau_start: int = 90
    tau_end: int = 200


class LabTimeWindow(df.Group):
    # Messages
    COUNT_RATES = df.Topic(CountRateMessage)
    G2 = df.Topic(G2Message)
    TAUCORR = df.Topic(DataMessage)

    # Nodes
    COUNT_RATE_ENSEMBLE: CountRateEnsembleNode
    G2_MODULATION_ENSEMBLE: G2ModulationEnsembleNode
    G2_TAU_ZERO_ENSEMBLE: G2TauZeroEnsembleNode

    # Application
    application: PyQtApplication

    # Lab Time Viz
    cr_ensemble_plot: LinePlot
    decay_time_plot: LinePlot
    g2_modulation_plot: LinePlot
    g2_tau_zero_plot: LinePlot

    config: LabTimeWindowConfig

    def setup(self) -> None:
        self.G2_MODULATION_ENSEMBLE.configure(
            G2ModulationEnsembleConfig(
                tau_start=self.config.tau_start * 1e-6,
                tau_end=self.config.tau_end * 1e-6,
            )
        )
        self.decay_time_plot.configure(
            LinePlotConfig(
                x_field="timestamp",
                y_field="data",
                mode=Mode.APPEND,
                window_size=200,
                style={
                    "setLabels": {
                        "title": "Decay Time",
                        "left": "Taucorr (us)",
                        "bottom": "Lab Time (seconds)",
                    }
                },
                curve_style=CurveStyle(pen=(38, 247, 253)),
                external_timer=True,
            )
        )
        self.cr_ensemble_plot.configure(
            LinePlotConfig(
                x_field="timestamp",
                y_field="data",
                mode=Mode.APPEND,
                window_size=200,
                style={
                    "setLabels": {
                        "title": "Ensemble Count Rate",
                        "left": "CR per pixel (kcps)",
                        "bottom": "Lab Time (seconds)",
                    }
                },
                curve_style=CurveStyle(pen=(255, 165, 0)),
                external_timer=True,
            )
        )
        self.g2_tau_zero_plot.configure(
            LinePlotConfig(
                x_field="timestamp",
                y_field="data",
                mode=Mode.APPEND,
                window_size=200,
                style={
                    "setLabels": {
                        "title": "Ensemble G2(1)-1",
                        "left": "G2(1)-1",
                        "bottom": "Lab Time (seconds)",
                    }
                },
                external_timer=True,
            )
        )
        self.g2_modulation_plot.configure(
            LinePlotConfig(
                x_field="timestamp",
                y_field="data",
                mode=Mode.APPEND,
                window_size=200,
                style={
                    "setLabels": {
                        "title": "Sum of G2-1({}us to {}us)".format(
                            self.config.tau_start, self.config.tau_end
                        ),
                        "left": "G2 Slope",
                        "bottom": "Lab Time (seconds)",
                    }
                },
                external_timer=True,
            )
        )

        plots = [
            self.cr_ensemble_plot,
            self.decay_time_plot,
            self.g2_tau_zero_plot,
            self.g2_modulation_plot,
        ]

        # Setup app
        self.application.configure(
            PyQtApplicationConfig(
                title="Lab time visualizations",
                width=400,
                height=640,
                external_timer=True,
                external_timer_interval=100,
            )
        )
        self.application.plots = plots

    def connections(self) -> df.Connections:
        return (
            # Data
            (self.COUNT_RATES, self.COUNT_RATE_ENSEMBLE.INPUT),
            (self.G2, self.G2_MODULATION_ENSEMBLE.INPUT),
            (self.G2, self.G2_TAU_ZERO_ENSEMBLE.INPUT),
            # Viz
            (self.COUNT_RATE_ENSEMBLE.OUTPUT, self.cr_ensemble_plot.INPUT),
            (self.G2_MODULATION_ENSEMBLE.OUTPUT, self.g2_modulation_plot.INPUT),
            (self.G2_TAU_ZERO_ENSEMBLE.OUTPUT, self.g2_tau_zero_plot.INPUT),
            (self.TAUCORR, self.decay_time_plot.INPUT),
        )
