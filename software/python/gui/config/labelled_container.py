from PyQt5 import QtCore, QtWidgets, QtGui


class LabelledContainer(QtWidgets.QWidget):
    """A container widget with a label widget

        The container is the unit widget that is added to the config layout.
        It has a QLabel child widget and one or more additional widgets that
        help the user select a config option. If the option is going to be tied
        to a value in the config struct, the single child widget containing the
        value MUST be named 'entry.'
        """

    def __init__(self, label, runtimeEditable=False):
        QtWidgets.QWidget.__init__(self)
        self._layout = QtWidgets.QHBoxLayout()
        self._layout.setAlignment(QtCore.Qt.AlignLeft)
        self.setLayout(self._layout)
        self.runtimeEditable = runtimeEditable
        self._label = QtWidgets.QLabel()
        if self.runtimeEditable:
            self._label.setStyleSheet("background-color: yellow")
        self._label.setText(label)
        self._layout.addWidget(self._label)
