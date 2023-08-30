from PyQt5 import QtCore, QtWidgets, QtGui

# a helper class for managing pixel grids like for the pf32
class PixelGrid(QtWidgets.QWidget):
    def __init__(self, label, nrows, ncols, runtimeEditable):
        QtWidgets.QWidget.__init__(self)
        self._pixels = []
        self._nrows = nrows
        self._ncols = ncols
        self._horizontal_size_hint = 0
        self._vertical_size_hint = 0
        self._label = label
        self._runtime_editable = runtimeEditable
        self.setLayout(self._create_layout(self._label, self._runtime_editable))
        self._selection_callback = None
        self._pixel_button_group = QtWidgets.QButtonGroup()
        self._pixel_button_group.setExclusive(False)
        self._attach_buttons_and_handlers()

    def _create_layout(self, label, runtimeEditable):
        layout = QtWidgets.QGridLayout()
        layout.setAlignment(QtCore.Qt.AlignCenter)
        layout.setContentsMargins(0, 0, 0, 0)
        containerLabel = QtWidgets.QLabel()
        if runtimeEditable:
            label.setStyleSheet("background-color: yellow")
        self.runtimeEditable = runtimeEditable
        containerLabel.setText(label)
        layout.setHorizontalSpacing(0)
        layout.setVerticalSpacing(0)
        layout.addWidget(containerLabel, 0, 1, 1, self._ncols)
        return layout

    def _clearLayout(self):
        layout = self.layout()
        items = [layout.itemAt(i) for i in range(layout.count())]
        for i in items:
            layout.removeItem(i)

    def resize_grid(self, nrows, ncols):
        self._pixels = []
        self._nrows = nrows
        self._ncols = ncols
        self._horizontal_size_hint = 0
        self._vertical_size_hint = 0
        self._clearLayout()
        self.setLayout(self._create_layout(self._label, self._runtime_editable))
        self._pixel_button_group = QtWidgets.QButtonGroup()
        self._pixel_button_group.setExclusive(False)
        self._attach_buttons_and_handlers()
        self.update()

    def select_pixels(self, pixel_indices):
        indices = set(pixel_indices)
        for i in range(len(self._pixels)):
            if i in indices:
                self._pixels[i].setChecked(True)
            else:
                self._pixels[i].setChecked(False)
        if self._selection_callback:
            self._selection_callback()

    def _set_all_pixels_in_row_or_column(self, button_id, is_row, checked):
        for i in range(self._ncols if is_row else self._nrows):
            if is_row:
                pixel_index = button_id * self._ncols + i
            else:
                pixel_index = i * self._ncols + button_id
            pixel = self._pixels[pixel_index]
            pixel.setChecked(checked)

    def _make_right_click_handler(self, button_id, is_row):
        def handler(unused):
            self._set_all_pixels_in_row_or_column(button_id, is_row, False)
            if self._selection_callback:
                self._selection_callback()

        return handler

    def _make_left_click_handler(self, button_id, is_row):
        def handler(unused):
            self._set_all_pixels_in_row_or_column(button_id, is_row, True)
            if self._selection_callback:
                self._selection_callback()

        return handler

    def _make_pixel_button_handler(self):
        def handler(unused):
            if self._selection_callback:
                self._selection_callback()

        return handler

    _ROW_HEADER_OFFSET = 2
    _COLUMN_HEADER_OFFSET = 1

    def _attach_column_buttons(self):
        for column in range(self._ncols):
            button_id = column
            button = QtWidgets.QToolButton()
            button.setText("{:02d}".format(column))
            button.setMinimumWidth(1)
            button.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
            button.customContextMenuRequested.connect(
                self._make_right_click_handler(button_id, False)
            )
            button.clicked.connect(self._make_left_click_handler(button_id, False))
            button.setToolTip("Click to select column.\nRight-click to deselect.")
            self._horizontal_size_hint += button.sizeHint().width()
            self.layout().addWidget(button, 1, column + PixelGrid._COLUMN_HEADER_OFFSET)

    def _attach_row_buttons(self):
        for row in range(self._nrows):
            button_id = row
            button = QtWidgets.QToolButton()
            button.setText("{:02d}".format(row))
            button.setMinimumWidth(1)
            button.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
            button.customContextMenuRequested.connect(
                self._make_right_click_handler(button_id, True)
            )
            button.clicked.connect(self._make_left_click_handler(button_id, True))
            button.setToolTip("Click to select row.\nRight-click to deselect.")
            self._vertical_size_hint += button.sizeHint().height()
            self.layout().addWidget(button, row + PixelGrid._ROW_HEADER_OFFSET, 0)

    def _attach_pixel_buttons(self):
        button_id = 0
        for row in range(self._nrows):
            for column in range(self._ncols):
                # button = QtWidgets.QPushButton()
                button = QtWidgets.QCheckBox()
                button.setToolTip(
                    "Channel %d.\nClick to toggle selected/deselected." % button_id
                )
                # button.setMinimumWidth(3);
                self.layout().addWidget(
                    button,
                    row + self._ROW_HEADER_OFFSET,
                    column + self._COLUMN_HEADER_OFFSET,
                    QtCore.Qt.AlignCenter,
                )
                self._pixels.append(button)
                self._pixel_button_group.addButton(button, button_id)
                button_id += 1

    def _make_all_handler(self, checked):
        def handler(unused):
            for button in self._pixels:
                button.setChecked(checked)
            if self._selection_callback:
                self._selection_callback()

        return handler

    def _attach_all_button(self):
        button = QtWidgets.QToolButton()
        button.setText("All")
        button.setMinimumWidth(1)
        button.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        button.customContextMenuRequested.connect(self._make_all_handler(False))
        button.clicked.connect(self._make_all_handler(True))
        button.setToolTip("Click to select all pixels.\nRight-click to deselect.")
        self._vertical_size_hint += button.sizeHint().height()
        self._horizontal_size_hint += button.sizeHint().width()
        self.layout().addWidget(button, 1, 0)

    def _attach_buttons_and_handlers(self):
        self._attach_column_buttons()
        self._attach_row_buttons()
        self._attach_pixel_buttons()
        self._attach_all_button()
        self._pixel_button_group.buttonReleased.connect(
            self._make_pixel_button_handler()
        )

    def get_enabled_pixels(self):
        return [i for i in range(len(self._pixels)) if self._pixels[i].isChecked()]

    def set_selection_callback(self, callback):
        self._selection_callback = callback

    def sizeHint(self):
        return QtCore.QSize(self._horizontal_size_hint, self._vertical_size_hint)


class PixelGridWithScroll(QtWidgets.QScrollArea):
    def __init__(self, label, nrows, ncols, runtimeEditable):
        QtWidgets.QScrollArea.__init__(self)
        self._pixel_grid = PixelGrid(label, nrows, ncols, runtimeEditable)
        self._label = label
        self.setWidget(self._pixel_grid)
        self.setWidgetResizable(True)
        self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.runtimeEditable = runtimeEditable

    def select_pixels(self, pixel_indices):
        self._pixel_grid.select_pixels(pixel_indices)

    def get_enabled_pixels(self):
        return self._pixel_grid.get_enabled_pixels()

    def resize_grid(self, nrows, ncols):
        self._pixel_grid = PixelGrid(self._label, nrows, ncols, self.runtimeEditable)
        self.setWidget(self._pixel_grid)
        self.setWidgetResizable(True)
        self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.update()

    def set_selection_callback(self, callback):
        self._pixel_grid.set_selection_callback(callback)
