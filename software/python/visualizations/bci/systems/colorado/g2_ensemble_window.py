#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import labgraph as df
from bci.systems.common import PyQtApplication, PyQtApplicationConfig
from bci.systems.sawyer.messages import (
    DataMessage,
    G2FitMessage,
    G2Message,
    UpdateMessage,
)
from bci.systems.sawyer.nodes import G2EnsembleNode, G2StatsNode, G2StatsConfig
from bci.systems.sawyer.viz import G2Plot, G2PlotConfig
from bci.libraries.labgraph_viz.plots import (
    CurveStyle,
    LinePlot,
    LinePlotConfig,
    Mode,
)


class G2EnsembleWindow(df.Group):
    # Messages
    G2 = df.Topic(G2Message)
    FIT_ERROR = df.Topic(DataMessage)
    FITTED_CURVE = df.Topic(G2FitMessage)

    # Nodes
    G2_ENSEMBLE: G2EnsembleNode
    G2_STATS: G2StatsNode

    # Application
    application: PyQtApplication

    # Plots
    fit_error_plot: LinePlot
    g2_ensemble_mean_plot: G2Plot
    g2_ensemble_std_plot: LinePlot
    g2_ensemble_snr_plot: LinePlot

    def setup(self) -> None:
        self.G2_STATS.configure(G2StatsConfig(N=10))
        self.fit_error_plot.configure(
            LinePlotConfig(
                x_field="timestamp",
                y_field="data",
                mode=Mode.APPEND,
                window_size=200,
                style={
                    "setLabels": {
                        "title": "Fit Error",
                        "left": "GMSE",
                        "bottom": "Lab Time (seconds)",
                    }
                },
                external_timer=True,
            )
        )
        self.g2_ensemble_mean_plot.configure(
            G2PlotConfig(
                tau_field="tau",
                g2_mean_field="g2_mean",
                g2_fit_field="g2_fit",
                mode=Mode.UPDATE,
                style={
                    "setLabels": {
                        "title": "G2-1 Ensemble Mean",
                        "left": "G2-1",
                        "bottom": "Delay (seconds)",
                    },
                    "setLogMode": {"x": True},
                },
                external_timer=True,
            )
        )
        self.g2_ensemble_std_plot.configure(
            LinePlotConfig(
                x_field="x",
                y_field="y",
                mode=Mode.UPDATE,
                style={
                    "setLabels": {
                        "title": "STD of G2-1 Ensemble Mean over 10 Tints",
                        "left": "STD",
                        "bottom": "Delay (seconds)",
                    },
                    "setLogMode": {"x": True},
                },
                curve_style=CurveStyle(pen=(255, 0, 0)),
                external_timer=True,
            )
        )
        self.g2_ensemble_snr_plot.configure(
            LinePlotConfig(
                x_field="x",
                y_field="y",
                mode=Mode.UPDATE,
                style={
                    "setLabels": {
                        "title": "SNR of G2-1 Ensemble Mean over 10 Tints",
                        "left": "SNR",
                        "bottom": "Delay (seconds)",
                    },
                    "setLogMode": {"x": True},
                },
                curve_style=CurveStyle(pen=(255, 0, 255)),
                external_timer=True,
            )
        )

        plots = [
            self.g2_ensemble_mean_plot,
            self.fit_error_plot,
            self.g2_ensemble_std_plot,
            self.g2_ensemble_snr_plot,
        ]

        # Setup app
        self.application.configure(
            PyQtApplicationConfig(
                title="G2 Ensemble Stats",
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
            (self.G2, self.G2_ENSEMBLE.INPUT),
            (self.G2_ENSEMBLE.G2_ENSEMBLE_MEAN, self.G2_STATS.G2),
            # Viz
            (self.FIT_ERROR, self.fit_error_plot.INPUT),
            (self.FITTED_CURVE, self.g2_ensemble_mean_plot.INPUT),
            (self.G2_STATS.G2_STD, self.g2_ensemble_std_plot.INPUT),
            (self.G2_STATS.G2_SNR, self.g2_ensemble_snr_plot.INPUT),
        )
