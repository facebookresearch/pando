#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

from typing import Any, Optional

import labgraph as df
import numpy as np
import pyqtgraph as pg
from ..messages import TableStatsMessage

from ..common.constants import COLUMN_TYPES


class CRTableState(df.State):
    """
    State for a CRTable
    plot: The pyqt plot that we are updating
    """

    plot: Optional[Any] = None
    data: np.ndarray = np.array([])
    first_msg: bool = False


class CRTable(df.Node):
    """
    PyQTGraph line plot
    Subscribe to a topic and visualize the data in a line plot
    """

    TABLE_STATS = df.Topic(TableStatsMessage)

    state: CRTableState

    @df.subscriber(TABLE_STATS)
    def table_stats(self, message: TableStatsMessage) -> None:
        """
        Receive table stats data
        """
        self.state.data = message.data
        if not self.state.first_msg:
            self.state.first_msg = True

    def update(self):
        """
        Refresh the table with the latest data available
        """
        if self.state.first_msg:
            self.state.plot.setData(self.state.data)

    def build(self) -> Any:
        """
        Creates, stores, and returns a new TableWidget
        """
        self.state.plot = pg.TableWidget()
        self.state.plot.verticalHeader().setVisible(False)

        self.data = np.array(
            [
                ("0", 0, 0, 0, 0, 0, 0),
                ("1", 0, 0, 0, 0, 0, 0),
                ("2", 0, 0, 0, 0, 0, 0),
                ("3", 0, 0, 0, 0, 0, 0),
                ("4", 0, 0, 0, 0, 0, 0),
                ("5", 0, 0, 0, 0, 0, 0),
                ("6", 0, 0, 0, 0, 0, 0),
                ("7", 0, 0, 0, 0, 0, 0),
                ("TOTAL", 0, 0, 0, 0, 0, 0),
                ("STD", 0, 0, 0, 0, 0, 0),
            ],
            dtype=COLUMN_TYPES,
        )
        self.state.plot.setData(self.data)
        return self.state.plot

    def stop(self) -> None:
        self.state.plot.close()
