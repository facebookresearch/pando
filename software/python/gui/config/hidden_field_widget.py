from PyQt5 import QtCore, QtWidgets, QtGui
from .labelled_container import LabelledContainer


class HiddenFieldWidget(LabelledContainer):
    def __init__(self, label, param, mappers, config, runtimeEditable):
        LabelledContainer.__init__(self, label, runtimeEditable)
        self._mappers = mappers
        self._config = config
        self._channels = range(0)

        # Hidden QLineEdit contains the actual value
        self._entry = QtWidgets.QLineEdit()
        self._entry.setObjectName("entry")
        self._entry.setReadOnly(True)
        self.layout().addWidget(self._entry)
        self._entry.hide()
        self._entry.textChanged.connect(lambda x: self._on_hidden_entry_update(param))

    # called when the value of the hidden field Changes
    def _on_hidden_entry_update(self, param):
        self._entry.blockSignals(True)
        values_dct = {}
        if self._entry.text() != "":
            values_dct = self._mappers[param][0](self._entry.text())
        self._update_dependent_fields(param, values_dct)
        self._entry.blockSignals(False)


class HiddenFieldSpinBox(HiddenFieldWidget):
    def __init__(
        self,
        label,
        param,
        param_range,
        suffix,
        mappers,
        config,
        runtimeEditable,
        channel,
    ):
        HiddenFieldWidget.__init__(self, label, param, mappers, config, runtimeEditable)

        self._channel = channel
        self._box = QtWidgets.QSpinBox()
        self._box.setRange(param_range[0], param_range[1])
        self._box.setSuffix(suffix)
        self._box.setSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed
        )
        self.layout().addWidget(self._box)
        self._box.valueChanged.connect(lambda x: self._on_spin_box_update(param))

    # called to update widgets that depend on the value of the hidden field
    def _update_dependent_fields(self, param, values_dct):
        if self._box is not None:
            self._box.blockSignals(True)
            self._box.setValue(values_dct.get(self._channel, 0))
            self._box.blockSignals(False)
            self._config.set(param, self._mappers[param][0](self._entry.text()))

    # called to update the value of the hidden field from
    # the spinbox value
    def _update_entry(self, param):
        txt = str(self._channel) + ":" + str(self._box.value())
        self._entry.blockSignals(True)
        self._entry.setText(txt)
        self._entry.blockSignals(False)

    # called when the spinbox value changes and needs to update the hidden entry field
    def _on_spin_box_update(self, param):
        self._box.blockSignals(True)
        self._update_entry(param)
        self._box.blockSignals(False)
        self._config.set(param, self._mappers[param][0](self._entry.text()))

    # factory method for creating a HiddenFieldSpinBox
    @staticmethod
    def create(
        label,
        param,
        param_range,
        suffix,
        mappers,
        config,
        runtimeEditable=False,
        channel=1,
    ):
        widget = HiddenFieldSpinBox(
            label, param, param_range, suffix, mappers, config, runtimeEditable, channel
        )
        widget._entry.setText(widget._mappers[param][1](config.get(param)))
        return widget


class HiddenFieldPf32SpinBox(HiddenFieldSpinBox):
    def __init__(
        self,
        label,
        param,
        param_range,
        suffix,
        mappers,
        config,
        channels,
        runtimeEditable,
    ):
        HiddenFieldSpinBox.__init__(
            self, label, param, param_range, suffix, mappers, config, runtimeEditable, 0
        )
        self._channels = channels

    # called to update the value of the hidden field from
    # the spinbox value
    def _update_entry(self, param):
        entry_value = ""
        for channel in self._channels:
            entry_value += str(channel) + ":" + str(self._box.value()) + ","
        self._entry.setText(entry_value)

    @staticmethod
    def create(
        label,
        param,
        param_range,
        suffix,
        mappers,
        config,
        channels,
        runtimeEditable=False,
    ):
        widget = HiddenFieldPf32SpinBox(
            label,
            param,
            param_range,
            suffix,
            mappers,
            config,
            channels,
            runtimeEditable,
        )
        widget._entry.setText(widget._mappers[param][1](config.get(param)))
        return widget


class HiddenFieldSpinBoxGrid(HiddenFieldWidget):
    def __init__(
        self,
        label,
        param,
        param_range,
        suffix,
        channels,
        mappers,
        config,
        ncols,
        runtimeEditable,
    ):
        HiddenFieldWidget.__init__(self, label, param, mappers, config, runtimeEditable)
        self.layout().setSpacing(0)
        self._channels = channels

        # Arrange spinboxes in a grid so its easier to read
        self._grid = QtWidgets.QWidget()
        self._grid.setLayout(QtWidgets.QGridLayout())
        self._grid.setObjectName("grid")
        self._grid.layout().setSpacing(0)
        self.layout().addWidget(self._grid)
        self._grid.setSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed
        )

        # Spinboxes labelled with a channel number are exposed to the user
        # and the values are propogated to the entry with handlers
        self._spinboxes = []
        for i in channels:
            labelled_spinbox = QtWidgets.QWidget()
            labelled_spinbox.setObjectName(str(i))
            spinbox_layout = QtWidgets.QHBoxLayout()
            spinbox_layout.setAlignment(QtCore.Qt.AlignCenter)
            labelled_spinbox.setLayout(spinbox_layout)

            spinbox_label = QtWidgets.QLabel()
            spinbox_label.setText(" " + str(i))
            spinbox_layout.addWidget(spinbox_label)

            spinbox_box = QtWidgets.QSpinBox()
            spinbox_box.setRange(param_range[0], param_range[1])
            spinbox_box.setSuffix(suffix)
            spinbox_box.setSizePolicy(
                QtWidgets.QSizePolicy.MinimumExpanding,
                QtWidgets.QSizePolicy.MinimumExpanding,
            )
            spinbox_layout.addWidget(spinbox_box)

            self._spinboxes.append(labelled_spinbox)

            self._grid.layout().addWidget(
                self._spinboxes[-1], (i - 1) // ncols, (i - 1) % ncols
            )
            spinbox_box.valueChanged.connect(
                lambda x: self._on_grid_boxes_update(param)
            )

    # called to update widgets that depend on the value of the hidden field
    def _update_dependent_fields(self, param, values_dct):
        for c in self._channels:
            v = values_dct.get(c - 1, 0)
            box = self._grid.findChild(QtWidgets.QWidget, str(c)).findChild(
                QtWidgets.QSpinBox
            )
            if box is not None:
                box.blockSignals(True)
                box.setValue(v)
                box.blockSignals(False)
        self._update_entry(param)

    # called to update the value of the hidden field from
    # the spinbox value
    def _update_entry(self, param):
        entry_value = ""

        for child in self._grid.findChildren(QtWidgets.QWidget):
            spinbox = child.findChild(QtWidgets.QSpinBox)
            if spinbox is not None:
                # spinbox.blockSignals(True)  # block spinbox signals
                # translate from 1-indexed user facing values to 0-indexed actual values
                entry_value += (
                    str(int(child.objectName()) - 1) + ":" + str(spinbox.value()) + ","
                )
        entry_value = entry_value[:-1]  # remove trailing comma

        # block entry signals and set entry text
        self._entry.blockSignals(True)
        self._entry.setText(entry_value)
        self._entry.blockSignals(False)

    # called when a spinbox value is changed
    def _on_grid_boxes_update(self, param):
        self._update_entry(param)
        self._config.set(param, self._mappers[param][0](self._entry.text()))

    @staticmethod
    def create(
        label,
        param,
        param_range,
        suffix,
        channels,
        mappers,
        config,
        ncols=4,
        runtimeEditable=False,
    ):
        widget = HiddenFieldSpinBoxGrid(
            label,
            param,
            param_range,
            suffix,
            channels,
            mappers,
            config,
            ncols,
            runtimeEditable,
        )
        widget._entry.setText(widget._mappers[param][1](widget._config.get(param)))
        return widget
