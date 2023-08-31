"""
Copyright (c) 2013, Martin Fitzpatrick
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the software nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

# Repurposed from https://github.com/mfitzp/pyqtconfig to keep a attributes of a pybind11 class in sync with GUI elements

# -*- coding: utf-8 -*-
from __future__ import unicode_literals
import logging

# Import PyQt5 classes
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtNetwork import *
from PyQt5.QtWidgets import *


import os
import sys
import types
import copy

from collections import defaultdict, OrderedDict
import operator
import logging

try:
    QVariant
except NameError:
    QVariant = None

RECALCULATE_ALL = 1
RECALCULATE_VIEW = 2


def types_MethodType(fn, handler):
    try:
        return types.MethodType(fn, handler, type(handler))
    except TypeError:
        return types.MethodType(fn, handler)


def build_dict_mapper(mdict):
    """
    Build a map function pair for forward and reverse mapping from a specified dict

    Mapping requires both a forward and reverse (get, set) mapping function. This function
    is used to automatically convert a supplied dict to a forward and reverse paired lambda.

    :param mdict: A dictionary of display values (keys) and stored values (values)
    :type mdict: dict
    :rtype: 2-tuple of lambdas that perform forward and reverse map

    """
    rdict = {v: k for k, v in mdict.items()}
    return (
        lambda x: mdict[x] if x in mdict else x,
        lambda x: rdict[x] if x in rdict else x,
    )


try:
    # Python2.7
    unicode
except:
    # Python3 recoding
    def unicode(s):
        if isinstance(s, bytes):
            return s.decode("utf-8")
        else:
            return s


# Basestring for typechecking
try:
    basestring
except:
    basestring = str


def build_tuple_mapper(mlist):
    """
    Build a map function pair for forward and reverse mapping from a specified list of tuples

    :param mlist: A list of tuples of display values (keys) and stored values (values)
    :type mlist: list-of-tuples
    :rtype: 2-tuple of lambdas that perform forward and reverse map

    """
    rdict = {v: k for k, v in mlist}
    return (
        lambda x: mdict[x] if x in mdict else x,
        lambda x: rdict[x] if x in rdict else x,
    )


# CUSTOM HANDLERS

# QComboBox
def _get_QComboBox(self):
    """
        Get value QCombobox via re-mapping filter
    """
    return self._get_map(self.currentText())


def _set_QComboBox(self, v):
    """
        Set value QCombobox via re-mapping filter
    """
    self.setCurrentIndex(self.findText(unicode(self._set_map(v))))


def _event_QComboBox(self):
    """
        Return QCombobox change event signal
    """
    return self.currentIndexChanged


# QCheckBox
def _get_QCheckBox(self):
    """
        Get state of QCheckbox
    """
    return self.isChecked()


def _set_QCheckBox(self, v):
    """
        Set state of QCheckbox
    """
    self.setChecked(v)


def _event_QCheckBox(self):
    """
        Return state change signal for QCheckbox
    """
    return self.stateChanged


# QAction
def _get_QAction(self):
    """
        Get checked state of QAction
    """
    return self.isChecked()


def _set_QAction(self, v):
    """
        Set checked state of QAction
    """
    self.setChecked(v)


def _event_QAction(self):
    """
        Return state change signal for QAction
    """
    return self.toggled


# QActionGroup
def _get_QActionGroup(self):
    """
        Get checked state of QAction
    """
    if self.checkedAction():
        return self.actions().index(self.checkedAction())
    else:
        return None


def _set_QActionGroup(self, v):
    """
        Set checked state of QAction
    """
    self.actions()[v].setChecked(True)


def _event_QActionGroup(self):
    """
        Return state change signal for QAction
    """
    return self.triggered


# QPushButton
def _get_QPushButton(self):
    """
        Get checked state of QPushButton
    """
    return self.isChecked()


def _set_QPushButton(self, v):
    """
        Set checked state of QPushButton
    """
    self.setChecked(v)


def _event_QPushButton(self):
    """
        Return state change signal for QPushButton
    """
    return self.toggled


# QSpinBox
def _get_QSpinBox(self):
    """
        Get current value for QSpinBox
    """
    return self.value()


def _set_QSpinBox(self, v):
    """
        Set current value for QSpinBox
    """
    self.setValue(v)


def _event_QSpinBox(self):
    """
        Return value change signal for QSpinBox
    """
    return self.valueChanged


# QDoubleSpinBox
def _get_QDoubleSpinBox(self):
    """
        Get current value for QDoubleSpinBox
    """
    return self._get_map(self.value())


def _set_QDoubleSpinBox(self, v):
    """
        Set current value for QDoubleSpinBox
    """
    self.setValue(self._set_map(v))


def _event_QDoubleSpinBox(self):
    """
        Return value change signal for QDoubleSpinBox
    """
    return self.valueChanged


# QPlainTextEdit
def _get_QPlainTextEdit(self):
    """
        Get current document text for QPlainTextEdit
    """
    return self.document().toPlainText()


def _set_QPlainTextEdit(self, v):
    """
        Set current document text for QPlainTextEdit
    """
    self.setPlainText(unicode(v))


def _event_QPlainTextEdit(self):
    """
        Return current value changed signal for QPlainTextEdit box.

        Note that this is not a native Qt signal but a signal manually fired on 
        the user's pressing the "Apply changes" to the code button. Attaching to the 
        modified signal would trigger recalculation on every key-press.
    """
    return self.sourceChangesApplied


# QLineEdit
def _get_QLineEdit(self):
    """
        Get current text for QLineEdit
    """
    return self._get_map(self.text())


def _set_QLineEdit(self, v):
    """
        Set current text for QLineEdit
    """
    self.setText(unicode(self._set_map(v)))


def _event_QLineEdit(self):
    """
        Return current value changed signal for QLineEdit box.
    """
    return self.textChanged


# CodeEditor
def _get_CodeEditor(self):
    """
        Get current document text for CodeEditor. Wraps _get_QPlainTextEdit.
    """
    _get_QPlainTextEdit(self)


def _set_CodeEditor(self, v):
    """
        Set current document text for CodeEditor. Wraps _set_QPlainTextEdit.
    """
    _set_QPlainTextEdit(self, unicode(v))


def _event_CodeEditor(self):
    """
        Return current value changed signal for CodeEditor box. Wraps _event_QPlainTextEdit.
    """
    return _event_QPlainTextEdit(self)


# QListWidget
def _get_QListWidget(self):
    """
        Get currently selected values in QListWidget via re-mapping filter.

        Selected values are returned as a list.
    """
    return [self._get_map(s.text()) for s in self.selectedItems()]


def _set_QListWidget(self, v):
    """
        Set currently selected values in QListWidget via re-mapping filter.

        Supply values to be selected as a list.
    """
    if v:
        for s in v:
            self.findItems(unicode(self._set_map(s)), Qt.MatchExactly)[0].setSelected(
                True
            )


def _event_QListWidget(self):
    """
        Return current selection changed signal for QListWidget.
    """
    return self.itemSelectionChanged


# QListWidgetWithAddRemoveEvent
def _get_QListWidgetAddRemove(self):
    """
        Get current values in QListWidget via re-mapping filter.

        Selected values are returned as a list.
    """
    return [self._get_map(self.item(n).text()) for n in range(0, self.count())]


def _set_QListWidgetAddRemove(self, v):
    """
        Set currently values in QListWidget via re-mapping filter.

        Supply values to be selected as a list.
    """
    block = self.blockSignals(True)
    self.clear()
    self.addItems([unicode(self._set_map(s)) for s in v])
    self.blockSignals(block)
    self.itemAddedOrRemoved.emit()


def _event_QListWidgetAddRemove(self):
    """
        Return current selection changed signal for QListWidget.
    """
    return self.itemAddedOrRemoved


# QColorButton
def _get_QColorButton(self):
    """
        Get current value for QColorButton
    """
    return self.color()


def _set_QColorButton(self, v):
    """
        Set current value for QColorButton
    """
    self.setColor(v)


def _event_QColorButton(self):
    """
        Return value change signal for QColorButton
    """
    return self.colorChanged


# QNoneDoubleSpinBox
def _get_QNoneDoubleSpinBox(self):
    """
        Get current value for QDoubleSpinBox
    """
    return self.value()


def _set_QNoneDoubleSpinBox(self, v):
    """
        Set current value for QDoubleSpinBox
    """
    self.setValue(v)


def _event_QNoneDoubleSpinBox(self):
    """
        Return value change signal for QDoubleSpinBox
    """
    return self.valueChanged


# QCheckTreeWidget
def _get_QCheckTreeWidget(self):
    """
        Get currently checked values in QCheckTreeWidget via re-mapping filter.

        Selected values are returned as a list.
    """
    return [self._get_map(s) for s in self._checked_item_cache]


def _set_QCheckTreeWidget(self, v):
    """
        Set currently checked values in QCheckTreeWidget via re-mapping filter.

        Supply values to be selected as a list.
    """
    if v:
        for s in v:
            f = self.findItems(
                unicode(self._set_map(s)), Qt.MatchExactly | Qt.MatchRecursive
            )
            if f:
                f[0].setCheckState(0, Qt.Checked)


def _event_QCheckTreeWidget(self):
    """
        Return current checked changed signal for QCheckTreeWidget.
    """
    return self.itemCheckedChanged


# QSlider
def _get_QSlider(self):
    """
        Get current value for QSlider
    """
    return self.value()


def _set_QSlider(self, v):
    """
        Set current value for QSlider
    """
    self.setValue(v)


def _event_QSlider(self):
    """
        Return value change signal for QSlider
    """
    return self.valueChanged


# QButtonGroup
def _get_QButtonGroup(self):
    """
        Get a list of (index, checked) tuples for the buttons in the group
    """
    return [(nr, btn.isChecked()) for nr, btn in enumerate(self.buttons())]


def _set_QButtonGroup(self, v):
    """
        Set the states for all buttons in a group from a list of (index, checked) tuples
    """
    for idx, state in v:
        self.buttons()[idx].setChecked(state)


def _event_QButtonGroup(self):
    """
        Return button clicked signal for QButtonGroup
    """
    return self.buttonClicked


# QTabWidget
def _get_QTabWidget(self):
    """
        Get the current tabulator index
    """
    return self.currentIndex()


def _set_QTabWidget(self, v):
    """
        Set the current tabulator index
    """
    self.setCurrentIndex(v)


def _event_QTabWidget(self):
    """
        Return currentChanged signal for QTabWidget
    """
    return self.currentChanged


HOOKS = {
    QComboBox: (_get_QComboBox, _set_QComboBox, _event_QComboBox),
    QCheckBox: (_get_QCheckBox, _set_QCheckBox, _event_QCheckBox),
    QAction: (_get_QAction, _set_QAction, _event_QAction),
    QActionGroup: (_get_QActionGroup, _set_QActionGroup, _event_QActionGroup),
    QPushButton: (_get_QPushButton, _set_QPushButton, _event_QPushButton),
    QSpinBox: (_get_QSpinBox, _set_QSpinBox, _event_QSpinBox),
    QDoubleSpinBox: (_get_QDoubleSpinBox, _set_QDoubleSpinBox, _event_QDoubleSpinBox),
    QPlainTextEdit: (_get_QPlainTextEdit, _set_QPlainTextEdit, _event_QPlainTextEdit),
    QLineEdit: (_get_QLineEdit, _set_QLineEdit, _event_QLineEdit),
    QListWidget: (_get_QListWidget, _set_QListWidget, _event_QListWidget),
    QSlider: (_get_QSlider, _set_QSlider, _event_QSlider),
    QButtonGroup: (_get_QButtonGroup, _set_QButtonGroup, _event_QButtonGroup),
    QTabWidget: (_get_QTabWidget, _set_QTabWidget, _event_QTabWidget),
}

# ConfigManager handles configuration for a given appview
# Supports default values, change signals, export/import from file (for workspace saving)


class ConfigManager(QObject):

    # Signals
    # Triggered anytime configuration is changed (refresh)
    updated = pyqtSignal(int)

    def __init__(self, config_type, *args, **kwargs):
        super(ConfigManager, self).__init__(*args, **kwargs)

        self._config = config_type()
        self.mutex = QMutex()
        self.hooks = HOOKS
        self.handlers = {}
        self.handler_callbacks = {}
        self.eventhooks = {}

    # Get config
    def get(self, key):
        """ 
            Get config value for a given key from the config manager.

            Returns the value that matches the supplied key.

            :param key: The configuration key to return a config value for
            :type key: str
            :rtype: Any supported (str, int, bool, list-of-supported-types)
        """

        if key.startswith("_"):
            raise AttributeError("Attribute {} is private".format(key))

        with QMutexLocker(self.mutex):
            attr = getattr(self._config, key)
            if callable(attr):
                raise AttributeError("Attribute {} is callable".format(key))
            return attr

    def set(self, key, value, trigger_handler=True, trigger_update=True):
        """ 
            Set config value for a given key in the config manager.

            Set key to value. The optional trigger_update determines whether event hooks
            will fire for this key (and so re-calculation). It is useful to suppress these
            when updating multiple values for example.

            :param key: The configuration key to set
            :type key: str
            :param value: The value to set the configuration key to
            :type value: Any supported (str, int, bool, list-of-supported-types)
            :rtype: bool (success)   
        """
        old = self.get(key)
        if old == value:
            return False  # Not updating

        # Set value
        with QMutexLocker(self.mutex):
            setattr(self._config, key, value)

        if trigger_handler and key in self.handlers:
            # Trigger handler to update the view
            getter = self.handlers[key].getter
            setter = self.handlers[key].setter

            if setter and getter() != self.get(key):
                setter(self.get(key))

        # Trigger update notification
        if trigger_update:
            self.updated.emit(
                self.eventhooks[key] if key in self.eventhooks else RECALCULATE_ALL
            )

        return True

    # Completely replace current config (wipe all other settings)
    def replace(self, keyvalues, trigger_update=True):
        """
        Completely reset the config with a set of key values.

        Note that this does not wipe handlers or triggers, it simply replaces the values
        in the config entirely. It is the equivalent of unsetting all keys, followed by a
        set_many. Anything not in the supplied keyvalues will revert to default.

        :param keyvalues: A dictionary of keys and values to set as defaults
        :type keyvalues: dict
        :param trigger_update: Flag whether to trigger a config update (+recalculation) after all values are set. 
        :type trigger_update: bool

        """
        self._config = self.config_type()
        self.set_many(keyvalues)

    def set_many(self, keyvalues, trigger_update=True):
        """
        Set the value of multiple config settings simultaneously.

        This postpones the 
        triggering of the update signal until all values are set to prevent excess signals.
        The trigger_update option can be set to False to prevent any update at all.

        :param keyvalues: A dictionary of keys and values to set.
        :type key: dict
        :param trigger_update: Flag whether to trigger a config update (+recalculation) after all values are set. 
        :type trigger_update: bool
        """
        has_updated = False
        for k, v in list(keyvalues.items()):
            u = self.set(k, v, trigger_update=False)
            has_updated = has_updated or u

        if has_updated and trigger_update:
            self.updated.emit(RECALCULATE_ALL)

        return has_updated

    # HANDLERS

    # Handlers are UI elements (combo, select, checkboxes) that automatically update
    # and updated from the config manager. Allows instantaneous updating on config
    # changes and ensuring that elements remain in sync

    def add_handler(
        self,
        key,
        handler,
        mapper=(lambda x: x, lambda x: x),
        auto_set_default=True,
        default=None,
    ):
        """
        Add a handler (UI element) for a given config key.

        The supplied handler should be a QWidget or QAction through which the user
        can change the config setting. An automatic getter, setter and change-event
        handler is attached which will keep the widget and config in sync. The attached
        handler will default to the correct value from the current config.

        An optional mapper may also be provider to handler translation from the values
        shown in the UI and those saved/loaded from the config.

        """
        # Add map handler for converting displayed values to internal config data
        if isinstance(
            mapper, (dict, OrderedDict)
        ):  # By default allow dict types to be used
            mapper = build_dict_mapper(mapper)

        elif isinstance(mapper, list) and isinstance(mapper[0], tuple):
            mapper = build_tuple_mapper(mapper)

        handler._get_map, handler._set_map = mapper

        if key in self.handlers:  # Already there; so skip must remove first to replace
            return

        self.handlers[key] = handler

        # Look for class in hooks and add getter, setter, updater
        cls = self._get_hook(handler)
        hookg, hooks, hooku = self.hooks[cls]

        handler.getter = types_MethodType(hookg, handler)
        handler.setter = types_MethodType(hooks, handler)
        handler.updater = types_MethodType(hooku, handler)

        logging.debug("Add handler %s for %s" % (type(handler).__name__, key))

        def handler_callback(x=None):
            return self.set(key, handler.getter(), trigger_handler=False)

        handler.updater().connect(handler_callback)

        # Store this so we can issue a specific remove on deletes
        self.handler_callbacks[key] = handler_callback

        # Keep handler and data consistent
        if self.get(key) is not None:
            handler.setter(self.get(key))

    def _get_hook(self, handler):
        def fst(x):
            return next(x, None)

        cls = fst(x for x in self.hooks.keys() if x == type(handler))
        if cls is None:
            cls = fst(x for x in self.hooks.keys() if isinstance(handler, x))

        if cls is None:
            raise TypeError(
                "No handler-functions available for this widget "
                "type (%s)" % type(handler).__name__
            )
        return cls

    def add_handlers(self, keyhandlers):
        for key, handler in list(keyhandlers.items()):
            self.add_handler(key, handler)

    def remove_handler(self, key):
        if key in self.handlers:
            handler = self.handlers[key]
            handler.updater().disconnect(self.handler_callbacks[key])
            del self.handlers[key]

    def add_hooks(self, key, hooks):
        self.hooks[key] = hooks

    def as_dict(self):
        """
        Return the config as a flat dict (so it can be pickled)
        """
        result_dict = {}
        for attr in dir(self._config):
            try:
                result_dict[attr] = self.get(attr)
            except AttributeError:
                continue

        return result_dict

    def get_config(self):
        # TODO figure out how to make a copy here
        return self._config
