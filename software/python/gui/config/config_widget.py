import math
import pypando
import os
import sys
import yaml

from .config import ConfigManager
from .gui_config import GuiConfig
from .labelled_container import LabelledContainer
from .hidden_field_widget import (
    HiddenFieldWidget,
    HiddenFieldSpinBox,
    HiddenFieldSpinBoxGrid,
    HiddenFieldPf32SpinBox,
)
from .pixel_grid import PixelGridWithScroll
from PyQt5 import QtCore, QtWidgets, QtGui


class ConfigWidget(QtWidgets.QWidget):
    """Displays configuration options for user to select and edit

    The fundemental unit for a configuration option is the 'container' widget. A container
    can be generated with makeLabelledContainer(). The container belongs to a confLayout,
    which is one of sysLayout (for system-wide options) and devLayout (for device-specific
    options). The container is shown/hidden depending on whether the option it contains is
    relevent to the currently selected device. Inside the container, there are two required
    widgets: the label widget and the entry widget. The label widget provides the name that
    is shown to the user for the option in question. The entry widget contains the value that
    sets the option. The entry widget can either be directly set by the user or indirectly set
    based on the values of other, auxillary widgets owned by the container (this is the case,
    for example, in the widgets created by addXHarpEnabledChannels()).

    Each created container widget must be added to one (and only one) widget dict. The widget
    dicts are logical, mutually exclusive collections of widgets, generally based on a device or
    a processing action (picoHarpWidgetDict, g2WidgetDict, etc.). The reason for this is so that
    different devices can set the same config option in a way that makes sense for the device in
    question (for example, enabled_channels is quite different between the PicoHarp and the PF32).
    The keys of these dicts must be the variables within the config struct that will be set by
    each widget.

    If a container widget should not set any value in the config struct (like, for example, the
    tau_max widget), the container should not include an entry widget. It can still be added to
    widget dict as normal.

    If a variable requires mapping from the entered value to what must be stored in the config
    struct (like enabled_channels), an entry in the mappers dict can be made. The key in mappers
    needs to match the key used in the widget dict.

    Widget Hierachy:
    Device Group Box
        |--container widget
            |--label widget
            |--entry widget
            |--auxillary widget(s)
        |--container widget
            |-- ...
        |-- ...

    How To add a new configuration option:
    1. Create an add* method or choose a generic one that fits your use case.
    2. Add an entry with the correct key to the most reasonable widget dict.
        2.a. if you need to create a new widget dict, make sure to add it to widgetDictList
            and to the processing done in on_DeviceTypeSelection
    3. Add methods for any required auxillary handling (file browsing, etc.) and tie widget to methods
    4. Add a mapper to mappers if required.
    """

    def addWidgetToDictAndLayout(
        self, widget, param, dictionary, layout, layout_args=()
    ):
        dictionary[param] = widget
        layout.addWidget(widget, *layout_args)

    def __init__(self, parent=None, **kwargs):
        super().__init__(parent=parent)

        # unpack any args we allow to be overridden
        self.overrides = {}
        self.overrides["config_filename"] = kwargs.get("config_filename")
        self.overrides["output_directory"] = kwargs.get("output_directory")
        self.overrides["auto_stop"] = kwargs.get("auto_stop")
        self.overrides["firmware"] = kwargs.get("firmware")

        self.setObjectName("ConfigWidget")
        self.layout = QtWidgets.QHBoxLayout(self)
        # set parent window to resize when layout changes
        parent.layout().setSizeConstraint(QtWidgets.QLayout.SetFixedSize)

        # Initalize pando and config manager
        self.pandoConfigManager = ConfigManager(
            GuiConfig.make_factory(pypando.Pando.Config), parent=self
        )
        self.pandoConfigManager.setObjectName("pandoConfigManager")

        # Manage default configurations
        self.default_config_dict = self.pandoConfigManager.as_dict()

        self.default_config_device_overrides = {}
        self.default_config_device_overrides["HYDRA_HARP_T2"] = {}
        self.default_config_device_overrides["HYDRA_HARP_T2"]["xharp_level"] = {
            0: 500,
            1: 500,
            2: 500,
            3: 500,
            4: 500,
            5: 500,
            6: 500,
            7: 500,
        }
        self.default_config_device_overrides["HYDRA_HARP_T2"]["xharp_zerox"] = {
            0: 10,
            1: 10,
            2: 10,
            3: 10,
            4: 10,
            5: 10,
            6: 10,
            7: 10,
        }
        self.default_config_device_overrides["HYDRA_HARP_T2"]["hharp_input_offsets"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
        }

        self.default_config_device_overrides["HYDRA_HARP_T3"] = {}
        self.default_config_device_overrides["HYDRA_HARP_T3"]["xharp_level"] = {
            0: 500,
            1: 500,
            2: 500,
            3: 500,
            4: 500,
            5: 500,
            6: 500,
            7: 500,
        }
        self.default_config_device_overrides["HYDRA_HARP_T3"]["xharp_zerox"] = {
            0: 10,
            1: 10,
            2: 10,
            3: 10,
            4: 10,
            5: 10,
            6: 10,
            7: 10,
        }
        self.default_config_device_overrides["HYDRA_HARP_T3"]["hharp_input_offsets"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
        }
        self.default_config_device_overrides["HYDRA_HARP_T3"]["cri_offset"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
        }
        self.default_config_device_overrides["HYDRA_HARP_T3"]["cri_width"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
        }

        self.default_config_device_overrides["PICO_HARP_T2"] = {}
        self.default_config_device_overrides["PICO_HARP_T2"]["xharp_level"] = {1: 500}
        self.default_config_device_overrides["PICO_HARP_T2"]["xharp_zerox"] = {1: 10}

        self.default_config_device_overrides["PICO_HARP_T3"] = {}
        self.default_config_device_overrides["PICO_HARP_T3"]["xharp_level"] = {1: 500}
        self.default_config_device_overrides["PICO_HARP_T3"]["xharp_zerox"] = {1: 10}
        self.default_config_device_overrides["PICO_HARP_T3"]["cri_offset"] = {1: 0}
        self.default_config_device_overrides["PICO_HARP_T3"]["cri_width"] = {1: 0}

        self.default_config_device_overrides["MULTI_HARP_T2"] = {}
        self.default_config_device_overrides["MULTI_HARP_T2"]["hharp_input_offsets"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T2"][
            "multi_harp_trigger_edge"
        ] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T2"][
            "multi_harp_trigger_level_mv"
        ] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T2"]["multi_harp_tdead_ps"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }

        self.default_config_device_overrides["MULTI_HARP_T3"] = {}
        self.default_config_device_overrides["MULTI_HARP_T3"]["hharp_input_offsets"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T3"][
            "multi_harp_trigger_edge"
        ] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T3"][
            "multi_harp_trigger_level_mv"
        ] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T3"]["multi_harp_tdead_ps"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T3"]["cri_offset"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }
        self.default_config_device_overrides["MULTI_HARP_T3"]["cri_width"] = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0,
            11: 0,
            12: 0,
            13: 0,
            14: 0,
            15: 0,
        }

        self.default_config_device_overrides["PF32_G2"] = {}
        self.default_config_device_overrides["PF32_G2"]["bin_size_ns"] = 1600

        self.default_config_device_overrides["BASLER_A2"] = {}
        self.default_config_device_overrides["BASLER_A2"]["use_pandoboxd"] = True
        self.default_config_device_overrides["BASLER_A2"]["publish_raw_data"] = True
        self.default_config_device_overrides["BASLER_A2"]["camera_roi_width"] = 1920
        self.default_config_device_overrides["BASLER_A2"]["camera_roi_height"] = 1200

        self.default_config_device_overrides["ORCA_FUSION"] = {}
        self.default_config_device_overrides["ORCA_FUSION"]["use_pandoboxd"] = True
        self.default_config_device_overrides["ORCA_FUSION"]["publish_raw_data"] = True
        self.default_config_device_overrides["ORCA_FUSION"][
            "camera_frame_trigger_period_10ns"
        ] = 1424501
        self.default_config_device_overrides["ORCA_FUSION"][
            "camera_exposure_time_us"
        ] = 3000.0
        self.default_config_device_overrides["ORCA_FUSION"]["camera_roi_width"] = 2304
        self.default_config_device_overrides["ORCA_FUSION"]["camera_roi_height"] = 2304

        self.default_config_device_overrides["BASLER_ACA2000_CL"] = {}
        self.default_config_device_overrides["BASLER_ACA2000_CL"][
            "use_pandoboxd"
        ] = True
        self.default_config_device_overrides["BASLER_ACA2000_CL"][
            "publish_raw_data"
        ] = True
        self.default_config_device_overrides["BASLER_ACA2000_CL"][
            "camera_frame_trigger_period_10ns"
        ] = 294117  # 340 fps
        self.default_config_device_overrides["BASLER_ACA2000_CL"][
            "camera_exposure_time_us"
        ] = 2800.0
        self.default_config_device_overrides["BASLER_ACA2000_CL"][
            "camera_roi_width"
        ] = 2048
        self.default_config_device_overrides["BASLER_ACA2000_CL"][
            "camera_roi_height"
        ] = 1088

        self.default_config_device_overrides["BASLER_BOOST"] = {}
        self.default_config_device_overrides["BASLER_BOOST"]["use_pandoboxd"] = True
        self.default_config_device_overrides["BASLER_BOOST"][
            "camera_frame_trigger_period_10ns"
        ] = 270270  # 370 fps
        self.default_config_device_overrides["BASLER_BOOST"][
            "camera_exposure_time_us"
        ] = 2600.0
        self.default_config_device_overrides["BASLER_BOOST"]["camera_roi_width"] = 1920
        self.default_config_device_overrides["BASLER_BOOST"]["camera_roi_height"] = 1472

        self.default_config_device_overrides["CAMERA_MOCK_HDF5"] = {}
        self.default_config_device_overrides["CAMERA_MOCK_HDF5"][
            "publish_raw_data"
        ] = True
        self.default_config_device_overrides["CAMERA_MOCK_HDF5"]["log_raw_data"] = False

        # Set up a layout for config options (whole window)
        # and a layout for control buttons (upper right corner of window)
        self.configLayout = QtWidgets.QGridLayout()
        self.layout.addLayout(self.configLayout)

        self.controlLayout = QtWidgets.QVBoxLayout()
        self.controlLayout.setAlignment(QtCore.Qt.AlignCenter)
        self.configLayout.addLayout(self.controlLayout, 0, 1)

        # Create group boxes for logically similar configuration options
        self.sysConfigGroup = QtWidgets.QGroupBox("System Configuration")

        devConfigGroup = QtWidgets.QGroupBox("Device Configuration")

        chanConfigGroup = QtWidgets.QGroupBox("Channel Configuration")

        g2ConfigGroup = QtWidgets.QGroupBox("G2 Configuration")

        pf32g2ConfigGroup = QtWidgets.QGroupBox("PF32 Correlator Configuration")

        dtofConfigGroup = QtWidgets.QGroupBox("DTOF/CRi Configuration")

        countRateConfigGroup = QtWidgets.QGroupBox("Count Rate Configuration")

        # Set up System Configuration Group Box
        self.sysLayout = QtWidgets.QVBoxLayout()
        self.sysLayout.setSpacing(0)
        self.sysLayout.setAlignment(QtCore.Qt.AlignTop)
        self.sysConfigGroup.setLayout(self.sysLayout)
        self.configLayout.addWidget(
            self.sysConfigGroup, 0, 0
        )  # add to main config layout

        # Set up tab group to divide general device, channel-specific, and calculation-specific parameters
        self.devTabs = QtWidgets.QTabWidget()
        self.devTabs.setMaximumWidth(1200)
        self.configLayout.addWidget(self.devTabs, 1, 0, 2, 1)

        self.tabsDict = {}

        # Set up Device Configuration Group Box
        # using an optional QGridLayout
        # for devices that might need a more
        # complex layout
        self.devGridLayout = QtWidgets.QGridLayout()
        self.devGridLayout.setSpacing(0)
        self.devGridLayout.setAlignment(QtCore.Qt.AlignTop)
        devLayoutContainer = QtWidgets.QVBoxLayout()
        self.devLayout = QtWidgets.QVBoxLayout()
        self.devLayout.setSpacing(0)
        self.devLayout.setAlignment(QtCore.Qt.AlignTop)
        devLayoutContainer.addLayout(self.devLayout)
        devLayoutContainer.addLayout(self.devGridLayout)
        devConfigGroup.setLayout(devLayoutContainer)
        self.devTabs.addTab(devConfigGroup, "General")  # add to tab group
        self.tabsDict["General"] = self.devTabs.indexOf(devConfigGroup)

        # Set up Channel Configuration Group Box
        self.chanLayout = QtWidgets.QVBoxLayout()
        self.chanLayout.setSpacing(0)
        self.chanLayout.setAlignment(QtCore.Qt.AlignTop)
        chanConfigGroup.setLayout(self.chanLayout)
        self.devTabs.addTab(chanConfigGroup, "Channel")  # add to tab group
        self.tabsDict["Channel"] = self.devTabs.indexOf(chanConfigGroup)

        # Set up G2 Configuration Group Box
        self.g2Layout = QtWidgets.QVBoxLayout()
        self.g2Layout.setSpacing(0)
        self.g2Layout.setAlignment(QtCore.Qt.AlignTop)
        g2ConfigGroup.setLayout(self.g2Layout)
        self.devTabs.addTab(g2ConfigGroup, "G2")  # add to tab group
        self.tabsDict["G2"] = self.devTabs.indexOf(g2ConfigGroup)

        # Set up G2 Configuration Group Box (different g2 config scheme for PF32_G2)
        self.pf32g2Layout = QtWidgets.QVBoxLayout()
        self.pf32g2Layout.setSpacing(0)
        self.pf32g2Layout.setAlignment(QtCore.Qt.AlignTop)
        pf32g2ConfigGroup.setLayout(self.pf32g2Layout)
        self.devTabs.addTab(pf32g2ConfigGroup, "PF32 Correlator")  # add to tab group
        self.tabsDict["PF32 Correlator"] = self.devTabs.indexOf(pf32g2ConfigGroup)

        # Set up DTOF/CRi Configuration Group Box
        self.dtofLayout = QtWidgets.QVBoxLayout()
        self.dtofLayout.setSpacing(0)
        self.dtofLayout.setAlignment(QtCore.Qt.AlignTop)
        dtofConfigGroup.setLayout(self.dtofLayout)
        self.devTabs.addTab(dtofConfigGroup, "DTOF/CRi")  # add to tab group
        self.tabsDict["DTOF"] = self.devTabs.indexOf(dtofConfigGroup)

        # Set up Count Rate Configuration Group Box
        self.countRateLayout = QtWidgets.QVBoxLayout()
        self.countRateLayout.setSpacing(0)
        self.countRateLayout.setAlignment(QtCore.Qt.AlignTop)
        countRateConfigGroup.setLayout(self.countRateLayout)
        self.devTabs.addTab(countRateConfigGroup, "Count Rate")  # add to tab group
        self.tabsDict["Count Rate"] = self.devTabs.indexOf(countRateConfigGroup)

        # change enabled tabs
        for key, i in self.tabsDict.items():
            self.devTabs.setTabEnabled(i, False)
        self.devTabs.setTabEnabled(self.tabsDict["General"], True)  # enable general
        self.devTabs.setTabEnabled(
            self.tabsDict["Count Rate"], True
        )  # enable count rate tab
        self.devTabs.setCurrentIndex(0)

        # Add Process info box
        self.processLayout = QtWidgets.QVBoxLayout()
        self.processLayout.setAlignment(QtCore.Qt.AlignCenter)
        self.configLayout.addLayout(self.processLayout, 1, 1)

        # this is our instance of pando
        self.pando = pypando.SingletonPando.Get()
        self.acquisition_handle = None

        # Add control widgets (configure, start, stop buttons)
        self.controlLayout.addStretch(1)
        self.configureButton = self.addButton(
            self.controlLayout, "configureButton", "Configure"
        )
        self.startButton = self.addButton(self.controlLayout, "startButton", "Start")
        self.stopButton = self.addButton(self.controlLayout, "stopButton", "Stop")
        self.stopProgress = self.addButton(
            self.controlLayout, "stopProgressIndicator", "Saving Data..."
        )
        self.stopProgress.hide()
        self.stopProgress.setStyleSheet("background-color: red")
        self.stopProgress.setEnabled(False)
        size_policy = self.stopProgress.sizePolicy()
        size_policy.setRetainSizeWhenHidden(True)
        self.stopProgress.setSizePolicy(size_policy)
        self.controlLayout.addStretch(1)

        # add process widgets
        self.processLayout.addStretch(1)
        self.portTextBox = self.addLabelledButton(
            self.processLayout,
            "TCP Port",
            "egress_port",
            str(self.pando.GetProxyEgressPort()),
        )
        self.processLayout.addStretch(1)

        # Add system-level configuration options
        devExpLayout = QtWidgets.QHBoxLayout()
        self.deviceType = self.addDeviceType(devExpLayout)
        self.expType = self.addExpType(devExpLayout)
        self.sysLayout.addLayout(devExpLayout)
        self.outputDir = self.addLineEditWithBrowseButton(
            self.sysLayout, "Output Directory"
        )
        checkboxRowLayout = QtWidgets.QHBoxLayout()
        self.sysLayout.addLayout(checkboxRowLayout)
        self.autoStop = self.addCheckBox(checkboxRowLayout, "Auto Stop")
        checkboxRowLayout.addStretch(1)
        self.publishRaw = self.addCheckBox(checkboxRowLayout, "Publish Raw Data")
        self.logRawW = self.addCheckBox(checkboxRowLayout, "Log Raw Data")
        self.logAnalyzedW = self.addCheckBox(checkboxRowLayout, "Log Analyzed Data")
        self.logPeripheralW = self.addCheckBox(checkboxRowLayout, "Log Peripheral Data")
        configFileLayout = QtWidgets.QHBoxLayout()
        self.sysLayout.addLayout(configFileLayout)
        self.pandoboxdIpAddr = self.addLineEdit(configFileLayout, "Pandoboxd IP")
        self.experimentLength = self.addSpinBoxWithRange(
            configFileLayout,
            "Experiment Length",
            (1, 86400),
            step=1,
            suffix="s",
            double=True,
        )
        self.configfileW = self.addConfigFileWidget(configFileLayout)
        self.usePandoboxd = self.addCheckBox(checkboxRowLayout, "Enable Pandoboxd")
        self.usePps = self.addCheckBox(checkboxRowLayout, "Adjust timestamps using PPS")

        # for any config options that require a mapper, the mapper
        # should be put in this dict with the key matching the name
        # of the config option
        self.mappers = {}
        self.mappers["enabled_channels"] = (
            # Convert string of comma seperated digits to list of ints
            lambda txt: [int(s) for s in txt.split(",") if s.isdigit()],
            # Convert a list of ints to a string of comma separated digits
            lambda lst: ",".join(str(i) for i in lst) + ",",
        )
        self.mappers["hharp_input_offsets"] = (
            # convert string of form a:b,c:d... to dictionary {a:b, c:d, ...} of ints
            lambda txt: {
                int(item.split(":")[0]): int(item.split(":")[1])
                for item in txt.split(",")
                if item != ""
            },
            # convert a dictionary of ints to a string of form a:b,c:d,...
            lambda dct: ",".join(
                s for s in [str(k) + ":" + str(v) for k, v in dct.items()]
            ),
        )
        self.mappers["camera_frame_trigger_period_10ns"] = (
            # Convert from FPS to closest achievable period (1 LSB = 10ns)
            lambda fps: 0 if fps == 0 else round(100e6 / fps),
            # Convert from period to FPS
            lambda period: 0 if period == 0 else 100e6 / period,
        )
        self.mappers["aux_frame_trigger_period_10ns"] = self.mappers[
            "camera_frame_trigger_period_10ns"
        ]
        self.mappers["xharp_level"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper
        self.mappers["xharp_zerox"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper
        self.mappers["cri_offset"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper
        self.mappers["cri_width"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper
        self.mappers["multi_harp_trigger_edge"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper
        self.mappers["multi_harp_trigger_level_mv"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper
        self.mappers["multi_harp_tdead_ps"] = self.mappers[
            "hharp_input_offsets"
        ]  # use same mapper

        self.mappers["exp_type"] = (
            lambda txt: pypando.Pando.ExperimentType.__members__[txt],
            lambda exptype: [
                k
                for k, v in pypando.Pando.ExperimentType.__members__.items()
                if v == exptype
            ][0],
        )
        self.mappers["pf32_bit_mode"] = (
            lambda txt: pypando.Pando.PF32BitMode.__members__[txt],
            lambda bitmode: [
                k
                for k, v in pypando.Pando.PF32BitMode.__members__.items()
                if v == bitmode
            ][0],
        )
        self.mappers["count_integ_period_ns"] = (
            lambda to_struct: int(to_struct * 1000000),
            lambda from_struct: from_struct / 1000000.0,
        )
        self.mappers["dtof_integ_period_ns"] = self.mappers["count_integ_period_ns"]
        self.mappers["cri_integ_period_ns"] = self.mappers["count_integ_period_ns"]
        self.mappers["hharp_input_offsets"] = (
            # convert string of form a:b,c:d... to dictionary {a:b, c:d, ...} of ints
            lambda txt: {
                int(item.split(":")[0]): int(item.split(":")[1])
                for item in txt.split(",")
                if item != ""
            },
            # convert a dictionary of ints to a string of form a:b,c:d,...
            lambda dct: ",".join(
                s for s in [str(k) + ":" + str(v) for k, v in dct.items()]
            ),
        )
        # Maps which experiment types are legal for each device
        # TODO: it would be nicer to somehow populate this automatically
        # or else maybe do it from Pando and bind it here.
        self.devTypeToExpTypes = {}
        self.devTypeToExpTypes["NONE"] = [
            "NONE",
        ]
        self.devTypeToExpTypes["HYDRA_HARP_T2"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["HYDRA_HARP_T3"] = ["NONE", "TD", "TDDCS"]
        self.devTypeToExpTypes["PICO_HARP_T2"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["PICO_HARP_T3"] = ["NONE", "TD", "TDDCS"]
        self.devTypeToExpTypes["MULTI_HARP_T3"] = ["NONE", "TD", "TDDCS"]
        self.devTypeToExpTypes["MULTI_HARP_T2"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["PF32_DCS"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["PF32_TCSPC"] = [
            "NONE",
            "TD",
            "TDDCS",
        ]
        self.devTypeToExpTypes["PF32_G2"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["HYDRA_HARP_T2_MOCK_PTU"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["HYDRA_HARP_T3_MOCK_PTU"] = ["NONE", "TD", "TDDCS"]
        self.devTypeToExpTypes["PICO_HARP_T2_MOCK_PTU"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["PICO_HARP_T3_MOCK_PTU"] = ["NONE", "TD", "TDDCS"]
        self.devTypeToExpTypes["MULTI_HARP_T2_MOCK_PTU"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["MULTI_HARP_T3_MOCK_PTU"] = ["NONE", "TD", "TDDCS"]
        self.devTypeToExpTypes["TIMETAGGER_MOCK_HDF5"] = [
            "NONE",
            "DCS",
            "TD",
            "TDDCS",
        ]
        self.devTypeToExpTypes["HISTOGRAMMER_MOCK_HDF5"] = [
            "NONE",
            "DCS",
        ]
        self.devTypeToExpTypes["BASLER_A2"] = [
            "NONE",
        ]
        self.devTypeToExpTypes["ORCA_FUSION"] = [
            "NONE",
        ]
        self.devTypeToExpTypes["BASLER_ACA2000_CL"] = [
            "NONE",
        ]
        self.devTypeToExpTypes["BASLER_BOOST"] = [
            "NONE",
        ]
        self.devTypeToExpTypes["CAMERA_MOCK_HDF5"] = [
            "NONE",
        ]

        # Add device-specific configuration options:

        # Widget dicts are mutually exclusive collections of
        # config option widgets that are visible/hidden as a
        # group. They are organized logically according to their
        # related purposes. The keys are the config option name in
        # the pando config struct (or another logical name for those
        # that are not included in the struct)
        self.xHarpWidgetDict = {}
        self.hydraHarpWidgetDict = {}
        self.picoHarpWidgetDict = {}
        self.multiHarpWidgetDict = {}
        self.mockDeviceWidgetDict = {}
        self.mockChannelGridWidgetDict = {}
        self.pf32WidgetDict = {}
        self.pf32NonG2WidgetDict = {}
        self.cameraWidgetDict = {}
        self.g2WidgetDict = {}
        self.pf32g2WidgetDict = {}
        self.dtofWidgetDict = {}
        self.alldevicesWidgetDict = {}

        # list of all the config widgets
        self.widgetDictList = [
            self.xHarpWidgetDict,
            self.hydraHarpWidgetDict,
            self.picoHarpWidgetDict,
            self.multiHarpWidgetDict,
            self.mockDeviceWidgetDict,
            self.mockChannelGridWidgetDict,
            self.pf32WidgetDict,
            self.pf32NonG2WidgetDict,
            self.cameraWidgetDict,
            self.g2WidgetDict,
            self.pf32g2WidgetDict,
            self.dtofWidgetDict,
            self.alldevicesWidgetDict,
        ]

        # add widgets. Widgets will appear in the window in the order that they are created
        self.mockDeviceWidgetDict["mock_file"] = self.addLineEditWithBrowseButton(
            self.devLayout, "Mock File"
        )

        self.hydraHarpWidgetDict["enabled_channels"] = self.addXHarpEnabledChannels(
            self.devLayout, 8
        )
        self.picoHarpWidgetDict["enabled_channels"] = self.addXHarpEnabledChannels(
            self.devLayout, 2
        )
        self.multiHarpWidgetDict["enabled_channels"] = self.addXHarpEnabledChannels(
            self.devLayout, 16
        )
        self.pf32WidgetDict["enabled_channels"] = self.addPf32EnabledChannels(
            self.devGridLayout, self.pf32WidgetDict, layout_args=(2, 0, 4, 4)
        )
        self.pf32WidgetDict["load_pixels"] = self.addLabelledButton(
            self.devGridLayout,
            "Load Pixel Selection from Config",
            "load_pixels",
            "Load",
            layout_args=(7, 2, 1, 2),
        )
        self.alldevicesWidgetDict["count_integ_period_ns"] = self.addSpinBoxWithMin(
            self.countRateLayout,
            "Count Rate Integration Period",
            0,
            0.1,
            "ms",
            True,
            True,
        )

        self.pf32WidgetDict["pf32_laser_master"] = self.addPf32LaserMaster(
            self.devGridLayout, layout_args=(0, 0, 1, 1)
        )
        self.pf32WidgetDict["laser_sync_period_ps"] = self.addSpinBoxWithMin(
            self.devGridLayout,
            "Laser Sync Period",
            0,
            suffix="ps",
            layout_args=(0, 1, 1, 1),
        )
        self.pf32NonG2WidgetDict["pf32_bit_mode"] = self.addPF32BitMode(
            self.devGridLayout, layout_args=(0, 2, 1, 1)
        )

        self.cameraWidgetDict[
            "camera_frame_trigger_period_10ns"
        ] = self.addSpinBoxWithRange(
            self.devLayout,
            "Framerate",
            (0.1, 1000),
            suffix="FPS",
            double=True,
            decimals=1,
        )
        self.cameraWidgetDict[
            "aux_frame_trigger_period_10ns"
        ] = self.addSpinBoxWithRange(
            self.devLayout,
            "Secondary Camera Framerate",
            (0, 1000),
            suffix="FPS",
            double=True,
            decimals=1,
        )
        self.cameraWidgetDict["aux_frame_trigger_period_10ns"].setEnabled(False)

        self.cameraWidgetDict["camera_exposure_time_us"] = self.addSpinBoxWithRange(
            self.devLayout,
            "Exposure time",
            (0, 10000000.0),
            suffix="us",
            double=True,
            decimals=1,
        )

        self.cameraWidgetDict["camera_enable_test_pattern"] = self.addCheckBox(
            self.devLayout, "Enable Test Pattern",
        )

        self.cameraWidgetDict["camera_enable_roi"] = self.addCheckBox(
            self.devLayout, "Enable Rectangular ROI",
        )

        self.cameraWidgetDict["camera_roi_x_offset"] = self.addSpinBoxWithMin(
            self.devLayout, "ROI X Offset", 0,
        )

        self.cameraWidgetDict["camera_roi_y_offset"] = self.addSpinBoxWithMin(
            self.devLayout, "ROI Y Offset", 0,
        )

        self.cameraWidgetDict["camera_roi_width"] = self.addSpinBoxWithMin(
            self.devLayout, "ROI Width", 0,
        )

        self.cameraWidgetDict["camera_roi_height"] = self.addSpinBoxWithMin(
            self.devLayout, "ROI Height", 0,
        )

        self.mockChannelGridWidgetDict["channel_rows"] = self.addSpinBoxWithMin(
            self.devGridLayout, "Number of Rows", 0, layout_args=(6, 0, 1, 1)
        )

        self.mockChannelGridWidgetDict["channel_columns"] = self.addSpinBoxWithMin(
            self.devGridLayout, "Number of Columns", 0, layout_args=(6, 1, 1, 1)
        )

        self.mockChannelGridWidgetDict[
            "enabled_channels"
        ] = self.addGridEnabledChannels(
            self.devGridLayout,
            self.pandoConfigManager.get("channel_rows"),
            self.pandoConfigManager.get("channel_columns"),
            self.mockChannelGridWidgetDict,
            layout_args=(1, 0, 4, 4),
        )

        self.mockChannelGridWidgetDict["channel_update"] = self.addLabelledButton(
            self.devGridLayout,
            "Update Channel Grid",
            "channel_update",
            "Update",
            layout_args=(7, 0, 1, 2),
        )

        self.mockChannelGridWidgetDict["load_pixels"] = self.addLabelledButton(
            self.devGridLayout,
            "Load Pixel Selection from Config",
            "load_pixels",
            "Load",
            layout_args=(7, 2, 1, 2),
        )

        # self.xHarpWidgetDict['enabled_channels'] = self.addxHarpEnabledChannels(self.devLayout)
        self.xHarpWidgetDict["laser_sync_period_ps"] = self.addSpinBoxWithMin(
            self.devLayout, "Sync Laser Rep Period", 0, suffix="ps"
        )
        self.alldevicesWidgetDict["dtof_integ_period_ns"] = self.addSpinBoxWithMin(
            self.dtofLayout, "DTOF Integration Period", 0, 0.1, "ms", True, True
        )
        self.alldevicesWidgetDict["cri_integ_period_ns"] = self.addSpinBoxWithMin(
            self.dtofLayout, "CRi Integration Period", 0, 0.1, "ms", True, True
        )

        self.multiHarpWidgetDict["laser_sync_period_ps"] = self.addSpinBoxWithMin(
            self.devLayout, "Sync Laser Rep Period", 0, suffix="ps"
        )

        self.dtofWidgetDict["dtof_range_min"] = self.addSpinBoxWithMin(
            self.dtofLayout, "DTOF Minimum Microtime", 0, suffix="ps"
        )
        self.dtofWidgetDict["dtof_range_max"] = self.addSpinBoxWithMin(
            self.dtofLayout, "DTOF Maximum Microtime", 0, suffix="ps"
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBox.create(
                "Channel 1 CRi Offset",
                "cri_offset",
                (0, 2 ** 31 - 1),
                "ps",
                self.mappers,
                self.pandoConfigManager,
                runtimeEditable=True,
            ),
            "cri_offset",
            self.picoHarpWidgetDict,
            self.dtofLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBox.create(
                "Channel 1 Cri Width",
                "cri_width",
                (0, 2 ** 31 - 1),
                "ps",
                self.mappers,
                self.pandoConfigManager,
                runtimeEditable=True,
            ),
            "cri_width",
            self.picoHarpWidgetDict,
            self.dtofLayout,
        )
        self.addWidgetToDictAndLayout(
            HiddenFieldPf32SpinBox.create(
                "CRi Width",
                "cri_width",
                (0, 2 ** 31 - 1),
                "ps",
                self.mappers,
                self.pandoConfigManager,
                range(1024),
                True,
            ),
            "cri_width",
            self.pf32WidgetDict,
            self.dtofLayout,
        )
        self.addWidgetToDictAndLayout(
            HiddenFieldPf32SpinBox.create(
                "CRi Offset",
                "cri_offset",
                (0, 2 ** 31 - 1),
                "ps",
                self.mappers,
                self.pandoConfigManager,
                range(1024),
                True,
            ),
            "cri_offset",
            self.pf32WidgetDict,
            self.dtofLayout,
        )
        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "CRi Offset",
                "cri_offset",
                (0, 2 ** 31 - 1),
                "ps",
                range(1, 9),
                self.mappers,
                self.pandoConfigManager,
                runtimeEditable=True,
            ),
            "cri_offset",
            self.hydraHarpWidgetDict,
            self.dtofLayout,
        )
        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Cri Width",
                "cri_width",
                (0, 2 ** 31 - 1),
                "ps",
                range(1, 9),
                self.mappers,
                self.pandoConfigManager,
                runtimeEditable=True,
            ),
            "cri_width",
            self.hydraHarpWidgetDict,
            self.dtofLayout,
        )
        self.hydraHarpWidgetDict["cri_sync_button"] = self.addHydraHarpCriSyncButton(
            self.dtofLayout, self.hydraHarpWidgetDict, 8
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "CRi Offset",
                "cri_offset",
                (0, 2 ** 31 - 1),
                "ps",
                range(1, 17),
                self.mappers,
                self.pandoConfigManager,
                runtimeEditable=True,
            ),
            "cri_offset",
            self.multiHarpWidgetDict,
            self.dtofLayout,
        )
        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Cri Width",
                "cri_width",
                (0, 2 ** 31 - 1),
                "ps",
                range(1, 17),
                self.mappers,
                self.pandoConfigManager,
                runtimeEditable=True,
            ),
            "cri_width",
            self.multiHarpWidgetDict,
            self.dtofLayout,
        )
        self.multiHarpWidgetDict["cri_sync_button"] = self.addHydraHarpCriSyncButton(
            self.dtofLayout, self.multiHarpWidgetDict, 16
        )

        # these three widgets technically do not correspond to any config option in configurationManager
        # they are used to help the user set up other parameters. Unlike the other widgets in the system,
        # they do not have a child widget called "entry", which the GUI logic uses to attach values to fields
        # in the config struct. They are ignored.
        self.g2WidgetDict["integration_period"] = self.addIntegrationPeriod(
            self.g2Layout
        )
        self.g2WidgetDict["tau_max"] = self.addTauMax(self.g2Layout)
        self.g2WidgetDict["auto_calc_g2_params"] = self.addEnableAutoCalc(self.g2Layout)

        self.g2WidgetDict["bin_size_ns"] = self.addSpinBoxWithRange(
            self.g2Layout, "Level 0 Bin Size", (1, 1e6), step=10, suffix="ns"
        )
        self.g2WidgetDict["points_per_level"] = self.addSpinBoxWithMin(
            self.g2Layout, "Points per Level", 2
        )
        self.g2WidgetDict["n_levels"] = self.addSpinBoxWithRange(
            self.g2Layout, "Number of Levels", (1, 18)
        )
        self.g2WidgetDict["rebin_factor"] = self.addSpinBoxWithRange(
            self.g2Layout, "Rebin Factor", (2, 4)
        )
        self.g2WidgetDict["final_bin_count"] = self.addSpinBoxWithMin(
            self.g2Layout, "Final Bin Count", 1, runtimeEditable=True
        )
        self.g2WidgetDict["calc_g2i"] = self.addCheckBox(
            self.g2Layout, "Caclculate G2 over RoI"
        )

        self.pf32g2WidgetDict["bin_size_ns"] = self.addSpinBoxWithRange(
            self.pf32g2Layout, "Frame Period", (1, 1e6), step=10, suffix="ns"
        )

        self.pf32g2WidgetDict["pf32_g2_frame_count"] = self.addPF32G2FrameCount(
            self.pf32g2Layout
        )

        self.pf32g2WidgetDict["pf32_g2_burst_mode"] = self.addCheckBox(
            self.pf32g2Layout, "Enable Burst Mode"
        )

        self.pf32g2WidgetDict.update(self.addPF32G2RebinFactors(self.pf32g2Layout))

        self.xHarpWidgetDict["xharp_sync_div"] = self.addSpinBoxWithRange(
            self.devLayout, "Sync Divider", (1, 8)
        )

        self.multiHarpWidgetDict["xharp_sync_div"] = self.addSpinBoxWithRange(
            self.devLayout, "Sync Divider", (1, 16)
        )

        self.xHarpWidgetDict["xharp_sync_level"] = self.addSpinBoxWithRange(
            self.chanLayout, "Sync (Channel 0) CFD Level", (0, 1000), suffix="mV"
        )
        self.xHarpWidgetDict["xharp_sync_zerox"] = self.addSpinBoxWithRange(
            self.chanLayout,
            "Sync (Channel 0) Zero Crossing Level",
            (0, 40),
            suffix="mV",
        )

        self.xHarpWidgetDict["xharp_sync_offset"] = self.addSpinBoxWithRange(
            self.chanLayout, "Sync (Channel 0) Offset", (-99999, 99999), suffix="ps"
        )

        mharp_hbox_layout = QtWidgets.QHBoxLayout()
        self.chanLayout.addLayout(mharp_hbox_layout)

        self.multiHarpWidgetDict[
            "multi_harp_sync_trigger_edge"
        ] = self.addComboBoxWithList(
            mharp_hbox_layout,
            "multi_harp_sync_trigger_edge",
            "Sync Trigger Edge",
            ["Falling", "Rising"],
        )

        self.multiHarpWidgetDict[
            "multi_harp_sync_trigger_level_mv"
        ] = self.addSpinBoxWithRange(
            mharp_hbox_layout, "Sync Trigger Level", (-1200, 1200), suffix="mV"
        )

        self.multiHarpWidgetDict["multi_harp_sync_tdead_ps"] = self.addSpinBoxWithRange(
            mharp_hbox_layout, "Sync Tdead", (0, 16000, 100), suffix="ps"
        )

        mharp_hbox_layout.addStretch(2)

        mharp_hbox_layout = QtWidgets.QHBoxLayout()
        self.chanLayout.addLayout(mharp_hbox_layout)

        self.multiHarpWidgetDict["xharp_sync_offset"] = self.addSpinBoxWithRange(
            mharp_hbox_layout, "Sync Offset", (-99999, 99999), suffix="ps"
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBox.create(
                "Channel 1 CFD Level",
                "xharp_level",
                (0, 1000),
                "mV",
                self.mappers,
                self.pandoConfigManager,
            ),
            "xharp_level",
            self.picoHarpWidgetDict,
            self.chanLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBox.create(
                "Channel 1 Zero Crossing",
                "xharp_zerox",
                (0, 40),
                "mV",
                self.mappers,
                self.pandoConfigManager,
            ),
            "xharp_zerox",
            self.picoHarpWidgetDict,
            self.chanLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldPf32SpinBox.create(
                "CRi Offset",
                "cri_offset",
                (0, 2 ** 31 - 1),
                "ps",
                self.mappers,
                self.pandoConfigManager,
                range(1024),  # currently the max channels of any devices
                True,
            ),
            "cri_offset",
            self.mockDeviceWidgetDict,
            self.dtofLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldPf32SpinBox.create(
                "CRi Width",
                "cri_width",
                (0, 2 ** 31 - 1),
                "ps",
                self.mappers,
                self.pandoConfigManager,
                range(1024),  # currently the max channels of any devices
                True,
            ),
            "cri_width",
            self.mockDeviceWidgetDict,
            self.dtofLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Channel CFD Level",
                "xharp_level",
                (0, 1000),
                "mV",
                range(1, 9),
                self.mappers,
                self.pandoConfigManager,
            ),
            "xharp_level",
            self.hydraHarpWidgetDict,
            self.chanLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Channel Zero Crossing",
                "xharp_zerox",
                (0, 40),
                "mV",
                range(1, 9),
                self.mappers,
                self.pandoConfigManager,
            ),
            "xharp_zerox",
            self.hydraHarpWidgetDict,
            self.chanLayout,
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Channel Offsets",
                "hharp_input_offsets",
                (-99999, 99999),
                "ps",
                range(1, 9),
                self.mappers,
                self.pandoConfigManager,
            ),
            "hharp_input_offsets",
            self.hydraHarpWidgetDict,
            self.chanLayout,
        )

        multiHarpChanGridLayout = QtWidgets.QGridLayout()
        self.chanLayout.addLayout(multiHarpChanGridLayout)

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Channel Offsets",
                "hharp_input_offsets",
                (-99999, 99999),
                "ps",
                range(1, 17),
                self.mappers,
                self.pandoConfigManager,
            ),
            "hharp_input_offsets",
            self.multiHarpWidgetDict,
            multiHarpChanGridLayout,
            layout_args=(0, 0, 1, 1),
        )

        self.multiHarpWidgetDict[
            "multi_harp_trigger_edge"
        ] = self.addChannelComboBoxGrid(
            multiHarpChanGridLayout,
            "Trigger Edges",
            "multi_harp_trigger_edge",
            ["Falling", "Rising"],
            range(1, 17),
            layout_args=(0, 1, 1, 1),
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Trigger Levels",
                "multi_harp_trigger_level_mv",
                (-1200, 1200),
                "mV",
                range(1, 17),
                self.mappers,
                self.pandoConfigManager,
            ),
            "multi_harp_trigger_level_mv",
            self.multiHarpWidgetDict,
            multiHarpChanGridLayout,
            layout_args=(1, 0, 1, 1),
        )

        self.addWidgetToDictAndLayout(
            HiddenFieldSpinBoxGrid.create(
                "Tdead",
                "multi_harp_tdead_ps",
                (0, 160000, 100),
                "ns",
                range(1, 17),
                self.mappers,
                self.pandoConfigManager,
            ),
            "multi_harp_tdead_ps",
            self.multiHarpWidgetDict,
            multiHarpChanGridLayout,
            layout_args=(1, 1, 1, 1),
        )

        self.pf32WidgetDict["pf32_firmware_path"] = self.addLineEditWithBrowseButton(
            self.devGridLayout, "Firmware Path", layout_args=(1, 0, 1, 4)
        )

        # add all widgets and their keys to list
        self.allDevWidgetsList = [
            entry for dct in self.widgetDictList for entry in dct.items()
        ]

        # Most devices support runtime editable params
        self.disable_all_params_at_runtime = False

        # auto hide all device widgets (start up in in device_type = None state)
        for k, w in self.allDevWidgetsList:
            w.hide()

        # Make global signal/slot connections:
        # Changes visible config options on device type change and sets device field in config struct
        self.deviceType.findChild(QtWidgets.QWidget, "entry").currentTextChanged[
            str
        ].connect(self.on_DeviceTypeSelection)

        # Handles browsing for and selecting an output directory
        self.pandoConfigManager.add_handler(
            "output_dir", self.outputDir.findChild(QtWidgets.QWidget, "entry")
        )
        self.outputDir.findChild(QtWidgets.QWidget, "browseButton").clicked.connect(
            self.on_outputDirBrowse
        )

        self.mockChannelGridWidgetDict["channel_update"].findChild(
            QtWidgets.QWidget, "channel_update"
        ).clicked.connect(
            lambda: self.on_MockChannelsUpdate(self.mockChannelGridWidgetDict)
        )

        self.mockChannelGridWidgetDict["load_pixels"].findChild(
            QtWidgets.QWidget, "load_pixels"
        ).clicked.connect(self.on_pixmapFileBrowse)

        # Disable "Secondary Camera Framerate" spinbox if we're not controlling pandoboxd
        self.usePandoboxd.findChild(QtWidgets.QWidget, "entry").stateChanged.connect(
            self.on_EnablePandoboxd
        )

        # Handle other system-level
        self.pandoConfigManager.add_handler(
            "exp_type",
            self.expType.findChild(QtWidgets.QWidget, "entry"),
            self.mappers["exp_type"],
        )

        self.pandoConfigManager.add_handler(
            "experiment_length",
            self.experimentLength.findChild(QtWidgets.QWidget, "entry"),
        )
        self.pandoConfigManager.add_handler(
            "auto_stop", self.autoStop.findChild(QtWidgets.QWidget, "entry")
        )
        self.pandoConfigManager.add_handler(
            "publish_raw_data", self.publishRaw.findChild(QtWidgets.QWidget, "entry")
        )
        self.pandoConfigManager.add_handler(
            "log_raw_data", self.logRawW.findChild(QtWidgets.QWidget, "entry")
        )
        self.pandoConfigManager.add_handler(
            "log_analyzed_data", self.logAnalyzedW.findChild(QtWidgets.QWidget, "entry")
        )
        self.pandoConfigManager.add_handler(
            "log_peripheral_data",
            self.logPeripheralW.findChild(QtWidgets.QWidget, "entry"),
        )
        self.pandoConfigManager.add_handler(
            "pandoboxd_ip_addr",
            self.pandoboxdIpAddr.findChild(QtWidgets.QWidget, "entry"),
        )
        self.pandoConfigManager.add_handler(
            "use_pandoboxd", self.usePandoboxd.findChild(QtWidgets.QWidget, "entry")
        )
        self.pandoConfigManager.add_handler(
            "use_pps", self.usePps.findChild(QtWidgets.QWidget, "entry")
        )
        self.alldevicesWidgetDict["count_integ_period_ns"].show()
        self.pandoConfigManager.add_handler(
            "count_integ_period_ns",
            self.alldevicesWidgetDict["count_integ_period_ns"].findChild(
                QtWidgets.QWidget, "entry"
            ),
            self.mappers["count_integ_period_ns"],
        )
        self.pandoConfigManager.add_handler(
            "multi_harp_sync_trigger_edge",
            self.multiHarpWidgetDict["multi_harp_sync_trigger_edge"].findChild(
                QtWidgets.QComboBox, "entry"
            ),
            mapper=self.mappers["multi_harp_sync_trigger_edge"],
        )

        # Handles browsing for a mock file
        self.mockDeviceWidgetDict["mock_file"].findChild(
            QtWidgets.QWidget, "browseButton"
        ).clicked.connect(self.on_mockFileBrowse)

        # Handles browing for pf32 firmware
        self.pf32WidgetDict["pf32_firmware_path"].findChild(
            QtWidgets.QWidget, "browseButton"
        ).clicked.connect(self.on_pf32FirmwareBrowse)

        # Handles laser master checkbox
        self.pf32WidgetDict["pf32_laser_master"].findChild(
            QtWidgets.QWidget, "entry"
        ).clicked.connect(self.on_pf32LaserMasterCheck)

        self.pf32WidgetDict["load_pixels"].findChild(
            QtWidgets.QWidget, "load_pixels"
        ).clicked.connect(self.on_pixmapFileBrowse)

        # Updates other parameters and ranges when target values are changed
        self.g2WidgetDict["tau_max"].findChild(
            QtWidgets.QWidget, "target"
        ).editingFinished.connect(self.on_targetTauMaxUpdate)
        self.g2WidgetDict["integration_period"].findChild(
            QtWidgets.QWidget, "target"
        ).editingFinished.connect(self.on_targetIntegrationPeriodUpdate)
        self.g2WidgetDict["auto_calc_g2_params"].findChild(
            QtWidgets.QWidget, "enable"
        ).stateChanged.connect(self.on_autoCalcG2ParamsStateChanged)

        # button presses
        self.configureButton.clicked.connect(self.on_configure)
        self.startButton.clicked.connect(self.on_start)
        self.stopButton.clicked.connect(self.on_stop)

        # syncs updates and configurability
        self.pandoConfigManager.updated.connect(self.on_configManager_updated)

        # sets default values for tau_max and integration_period
        (tau_max_ns, integration_period_ns) = self.derive_actual_times()
        self.g2WidgetDict["tau_max"].findChild(QtWidgets.QWidget, "target").setValue(
            tau_max_ns * 1.0e-6
        )
        self.g2WidgetDict["integration_period"].findChild(
            QtWidgets.QWidget, "target"
        ).setValue(integration_period_ns * 1.0e-6)

        # timers
        self.healthTimer = QtCore.QTimer()
        self.healthTimer.timeout.connect(self.on_healthTimeout)

        self.stop_timer = QtCore.QTimer()
        self.stop_timer.timeout.connect(self.on_auto_stop)
        self.stop_timer.setSingleShot(True)

        # check for cmd line args that mean this is an automated
        # run. Set up timer to load file and directory data
        config_filename = self.overrides["config_filename"]
        output_directory = self.overrides["output_directory"]
        if config_filename is not None:
            # setup timer to load config
            QtCore.QTimer.singleShot(1, self.loadConfigAndStart)

    def loadConfigAndStart(self):
        self.loadConfiguration(self.overrides["config_filename"])
        output_dir = self.overrides["output_directory"]
        if output_dir is not None:
            self.pandoConfigManager.set("output_dir", output_dir)
        experiment_length = self.overrides["auto_stop"]
        if experiment_length:
            self.pandoConfigManager.set("auto_stop", True)
            self.pandoConfigManager.set("experiment_length", experiment_length)
        firmware_path = self.overrides["firmware"]
        if firmware_path is not None:
            self.pandoConfigManager.set("pf32_firmware_path", firmware_path)
        self.saveConfiguration(
            self.pandoConfigManager.get("output_dir") + os.sep + "autosaved-config.yaml"
        )
        self.on_start()

    # Generic Widget Creators:

    def makeLabelledContainer(self, label, runtimeEditable=False):
        """Creates a container widget and adds a label widget

        The container is the unit widget that is added to the config layout.
        It has a QLabel child widget and one or more additional widgets that
        help the user select a config option. If the option is going to be tied
        to a value in the config struct, the single child widget containing the
        value MUST be named 'entry.'
        """
        container = QtWidgets.QWidget()
        containerLayout = QtWidgets.QHBoxLayout()
        containerLayout.setAlignment(QtCore.Qt.AlignLeft)
        container.setLayout(containerLayout)
        container.runtimeEditable = runtimeEditable
        containerLabel = QtWidgets.QLabel()
        if runtimeEditable:
            containerLabel.setStyleSheet("background-color: yellow")
        containerLabel.setText(label)
        containerLayout.addWidget(containerLabel)
        return container

    def addComboBoxWithList(
        self, confLayout, param, label, values, runtimeEditable=False, layout_args=(),
    ):
        """Helper function creating a combobox widget with a preset list of values."""
        container = LabelledContainer(label, runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QComboBox()
        entry.setObjectName("entry")

        entry.addItems(values)
        entry.setEditable(False)
        entry.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        # define fns for custom mappers
        def map2Box(x):
            for i, v in enumerate(values):
                if i == x:
                    return v
            return None

        def map2Config(x):
            for i, v in enumerate(values):
                if v == x:
                    return i
            return None

        # add mapper
        self.mappers[param] = (map2Config, map2Box)

        container.layout().addWidget(entry)
        return container

    def addSpinBoxWithRange(
        self,
        confLayout,
        label,
        range,
        step=1,
        suffix="",
        double=False,
        runtimeEditable=False,
        layout_args=(),
        decimals=3,
    ):
        """Helper function creating a ranged spinbox widget."""
        container = LabelledContainer(label, runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QSpinBox()
        if double:
            entry = QtWidgets.QDoubleSpinBox()
            entry.setDecimals(decimals)
        entry.setObjectName("entry")

        entry.setRange(range[0], range[1])
        entry.setSingleStep(step)
        entry.setSuffix(suffix)
        entry.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        container.layout().addWidget(entry)
        return container

    def addSpinBoxWithMin(
        self,
        confLayout,
        label,
        min_value,
        step=1,
        suffix="",
        double=False,
        runtimeEditable=False,
        layout_args=(),
        decimals=3,
    ):
        """Helper function creating a spinbox widget with only a minimum value"""
        container = LabelledContainer(label, runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QSpinBox()
        if double:
            entry = QtWidgets.QDoubleSpinBox()
            entry.setDecimals(decimals)
        entry.setObjectName("entry")

        entry.setMinimum(min_value)
        entry.setMaximum(2 ** 31 - 1)  # set 'infinite' max
        entry.setSingleStep(step)
        entry.setSuffix(suffix)
        entry.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        container.layout().addWidget(entry)
        return container

    def addLineEdit(self, confLayout, label, runtimeEditable=False, layout_args=()):
        """Helper function creating a text box"""
        container = LabelledContainer(label, runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QLineEdit()
        entry.setObjectName("entry")

        container.layout().addWidget(entry)
        return container

    def addLabelledButton(
        self, confLayout, label, objectName, text, runtimeEditable=False, layout_args=()
    ):
        """Helper function creating a button."""
        container = LabelledContainer(label, runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        button = QtWidgets.QPushButton()
        button.setObjectName(objectName)
        button.setText(text)

        container.layout().addWidget(button)
        return container

    def addLineEditWithBrowseButton(
        self, confLayout, label, runtimeEditable=False, layout_args=()
    ):
        """Helper function creating a 'browse' interface with line edit"""
        container = LabelledContainer(label, runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QLineEdit()
        entry.setObjectName("entry")
        button = QtWidgets.QPushButton()
        button.setObjectName("browseButton")
        button.setText("Browse")

        container.layout().addWidget(entry)
        container.layout().addWidget(button)
        return container

    def addConfigFileWidget(self, confLayout):
        container = LabelledContainer("Config File", False)
        confLayout.addWidget(container)

        loadButton = QtWidgets.QPushButton()
        loadButton.setObjectName("loadButton")
        loadButton.setText("Load")
        saveButton = QtWidgets.QPushButton()
        saveButton.setObjectName("saveButton")
        saveButton.setText("Save")

        container.layout().addWidget(loadButton)
        container.layout().addWidget(saveButton)

        loadButton.clicked.connect(self.on_configFileBrowse)
        saveButton.clicked.connect(self.on_configFileSave)
        return container

    def addButton(self, confLayout, name, text):
        """Helper function creating a push button. Note that this is not a containerized widget"""
        entry = QtWidgets.QPushButton()
        entry.setObjectName(name)
        entry.setText(text)
        entry.setEnabled(True)
        confLayout.addWidget(entry)
        return entry

    # Specific Widget Creators:

    def addDeviceType(self, confLayout, runtimeEditable=False):
        container = LabelledContainer("Device Type", runtimeEditable)
        confLayout.addWidget(container)

        entry = QtWidgets.QComboBox()
        entry.setObjectName("entry")
        entry.addItems(pypando.Pando.DeviceType.__members__.keys())
        entry.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        container.layout().addWidget(entry)
        return container

    def addExpType(self, confLayout, runtimeEditable=False):
        container = LabelledContainer("Experiment Type", runtimeEditable)
        confLayout.addWidget(container)

        entry = QtWidgets.QComboBox()
        entry.setObjectName("entry")
        entry.addItem("NONE")
        entry.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )
        container.layout().addWidget(entry)
        return container

    def addPF32BitMode(self, confLayout, runtimeEditable=False, layout_args=()):
        container = LabelledContainer("Bit Mode", runtimeEditable)
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QComboBox()
        entry.setObjectName("entry")
        entry.addItems(pypando.Pando.PF32BitMode.__members__.keys())

        container.layout().addWidget(entry)
        return container

    def addCheckBox(self, confLayout, label, runtimeEditable=False):
        container = LabelledContainer(label)
        confLayout.addWidget(container)

        entry = QtWidgets.QCheckBox()
        entry.setObjectName("entry")

        container.layout().addWidget(entry)
        return container

    def addPf32LaserMaster(self, confLayout, runtimeEditable=False, layout_args=()):
        container = LabelledContainer("Laser is Master")
        confLayout.addWidget(container, *layout_args)

        entry = QtWidgets.QCheckBox()
        entry.setObjectName("entry")
        entry.setChecked(self.pandoConfigManager.get("pf32_laser_master"))

        container.layout().addWidget(entry)

        return container

    def addXHarpEnabledChannels(self, confLayout, nchannels, runtimeEditable=False):
        container = LabelledContainer("Enabled Channels", runtimeEditable)
        confLayout.addWidget(container)

        # Hidden QLineEdit contains the actual value
        entry = QtWidgets.QLineEdit()
        entry.setObjectName("entry")
        entry.setReadOnly(True)
        container.layout().addWidget(entry)
        entry.hide()
        entry.textChanged.connect(lambda x: self.on_ChannelsUpdate(container))

        vboxLayout = QtWidgets.QVBoxLayout()
        container.layout().addLayout(vboxLayout)

        # Checkboxes labelled with a channel number are exposed to the user
        # and the values are propogated to the entry with handlers
        layout = None
        checks = []
        for i in range(nchannels):
            if i % 8 == 0:
                if layout:
                    vboxLayout.addLayout(layout)
                layout = QtWidgets.QHBoxLayout()
                layout.setAlignment(QtCore.Qt.AlignLeft)
            labelled_check = QtWidgets.QWidget()
            labelled_check.setObjectName(str(i))
            check_layout = QtWidgets.QVBoxLayout()
            check_layout.setAlignment(QtCore.Qt.AlignCenter)
            labelled_check.setLayout(check_layout)

            check_label = QtWidgets.QLabel()
            check_label.setText(" " + str(i))
            check_layout.addWidget(check_label)

            check_box = QtWidgets.QCheckBox()
            check_layout.addWidget(check_box)

            checks.append(labelled_check)
            layout.addWidget(checks[-1])
            check_box.stateChanged.connect(lambda x: self.on_ChecksUpdate(container))
        vboxLayout.layout().addLayout(layout)

        # This has to be here so that the already-connected handlers catch the change
        entry.setText(
            ",".join(str(i) for i in self.pandoConfigManager.get("enabled_channels"))
            + ","
        )
        return container

    def addGenericEnabledChannels(self, confLayout, runtimeEditable=False):
        container = LabelledContainer("Enabled Channels", runtimeEditable)
        confLayout.addWidget(container)

        entry = QtWidgets.QLineEdit()
        entry.setObjectName("entry")
        container.layout().addWidget(entry)

        return container

    def addPf32EnabledChannels(
        self, confLayout, widgetDict, runtimeEditable=False, layout_args=()
    ):
        container = PixelGridWithScroll("Enabled Channels", 32, 32, runtimeEditable)
        confLayout.addWidget(container, *layout_args)
        container.set_selection_callback(
            lambda: self.on_GridChannelSelection(widgetDict)
        )
        return container

    def addGridEnabledChannels(
        self,
        confLayout,
        nrows,
        ncols,
        widgetDict,
        runtimeEditable=False,
        layout_args=(),
    ):
        container = PixelGridWithScroll(
            "Enabled Channels", nrows, ncols, runtimeEditable
        )
        confLayout.addWidget(container, *layout_args)
        container.set_selection_callback(
            lambda: self.on_GridChannelSelection(widgetDict)
        )
        return container

    def addIntegrationPeriod(self, confLayout, runtimeEditable=True):
        container = LabelledContainer("G2 Integration Period", runtimeEditable)
        confLayout.addWidget(container)

        target = QtWidgets.QDoubleSpinBox()
        target.setObjectName("target")
        target.setDecimals(3)
        target.setRange(0.001, 10.0e3)
        target.setSuffix("ms")
        target.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )
        actual = QtWidgets.QLineEdit()
        actual.setObjectName("actual")
        actual.setReadOnly(True)
        actual.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        container.layout().addWidget(QtWidgets.QLabel("Target: "))
        container.layout().addWidget(target)
        container.layout().addWidget(QtWidgets.QLabel("Actual: "))
        container.layout().addWidget(actual)

        return container

    def addPF32G2FrameCount(self, confLayout):
        """A spinbox for setting the # of frames in a t_int, plus a translation to milliseconds."""

        container = LabelledContainer("G2 Integration Period", False)
        confLayout.addWidget(container)

        frame_count = QtWidgets.QSpinBox()
        frame_count.setObjectName("entry")
        frame_count.setRange(
            18, 65535
        )  # valid range from PF32 Correlation System Artix 7 Guide.pdf
        frame_count.setSuffix(" frames")
        frame_count.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )
        milliseconds = QtWidgets.QLineEdit()
        milliseconds.setObjectName("milliseconds")
        milliseconds.setReadOnly(True)
        milliseconds.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        container.layout().addWidget(frame_count)
        container.layout().addWidget(milliseconds)

        return container

    def addPF32G2RebinFactors(self, confLayout):
        """Four dropdowns for selecting rebin factors."""

        widgetDict = {}

        layout = QtWidgets.QVBoxLayout()
        confLayout.addLayout(layout)

        for i in range(4):
            param = "pf32_g2_rebin_factor_{}".format(i)

            container = LabelledContainer("Rebin Factor @ tau =", False)
            layout.addWidget(container)

            tau = QtWidgets.QLineEdit()
            tau.setObjectName("tau")
            tau.setReadOnly(True)
            tau.setSizePolicy(
                QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
            )

            container.layout().addWidget(tau)

            entry = QtWidgets.QComboBox()
            entry.setObjectName("entry")

            entry.addItems(("1", "2", "4", "8"))
            entry.setSizePolicy(
                QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
            )

            self.mappers[param] = (int, str)

            container.layout().addWidget(entry)

            widgetDict[param] = container

        # Add tau max display
        container = LabelledContainer("tau_max =", False)
        layout.addWidget(container)
        tau_max = QtWidgets.QLineEdit()
        tau_max.setObjectName("tau_max")
        tau_max.setReadOnly(True)
        tau_max.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )
        container.layout().addWidget(tau_max)

        widgetDict["tau_max"] = container

        return widgetDict

    def addTauMax(self, confLayout, runtimeEditable=False):
        container = LabelledContainer("Tau Max", runtimeEditable)
        confLayout.addWidget(container)

        target = QtWidgets.QDoubleSpinBox()
        target.setObjectName("target")
        target.setDecimals(3)
        target.setRange(0.001, 10.0e3)
        target.setSingleStep(0.1)
        target.setSuffix("ms")
        target.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        actual = QtWidgets.QLineEdit()
        actual.setObjectName("actual")
        actual.setReadOnly(True)
        actual.setSizePolicy(
            QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Preferred
        )

        container.layout().addWidget(QtWidgets.QLabel("Target: "))
        container.layout().addWidget(target)
        container.layout().addWidget(QtWidgets.QLabel("Actual: "))
        container.layout().addWidget(actual)

        return container

    def addEnableAutoCalc(self, confLayout, runtimeEditable=True):
        container = LabelledContainer(
            "Autocalculate Number of Levels & Final Bin Count", runtimeEditable
        )
        confLayout.addWidget(container)

        enable = QtWidgets.QCheckBox()
        enable.setObjectName("enable")
        enable.setChecked(True)

        container.layout().addWidget(enable)
        return container

    # This expects the base parameter to be a map (in C++)/ list (in python) and requires a mapper entry exist for the param
    def addChannelComboBoxGrid(
        self,
        confLayout,
        label,
        param,
        items,
        channels,
        runtimeEditable=False,
        layout_args=(),
    ):
        container = LabelledContainer(label, runtimeEditable)
        container.layout().setSpacing(0)
        confLayout.addWidget(container, *layout_args)

        # Hidden QLineEdit contains the actual value
        entry = QtWidgets.QLineEdit()
        entry.setObjectName("entry")
        entry.setReadOnly(True)
        container.layout().addWidget(entry)
        container.layout().setSpacing(0)
        entry.hide()
        entry.textChanged.connect(
            lambda x: self.on_gridComboEntryUpdate(
                container, param, items, len(channels), x
            )
        )

        # Arrange channel offset spinboxes in a grid so its easier to read
        grid = QtWidgets.QWidget()
        grid.setLayout(QtWidgets.QGridLayout())
        grid.setObjectName("grid")
        grid.layout().setSpacing(0)
        container.layout().addWidget(grid)
        grid.setSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        # Comboboxes labelled with a channel number are exposed to the user
        # and the values are propogated to the entry with handlers
        comboboxes = []
        for i in channels:
            labelled_combobox = QtWidgets.QWidget()
            labelled_combobox.setObjectName(str(i))
            combobox_layout = QtWidgets.QHBoxLayout()
            combobox_layout.setAlignment(QtCore.Qt.AlignCenter)
            labelled_combobox.setLayout(combobox_layout)

            combobox_label = QtWidgets.QLabel()
            combobox_label.setText(" " + str(i))
            combobox_layout.addWidget(combobox_label)

            combobox_box = QtWidgets.QComboBox()
            combobox_box.addItems(items)
            combobox_box.setEditable(False)
            combobox_box.setSizePolicy(
                QtWidgets.QSizePolicy.MinimumExpanding,
                QtWidgets.QSizePolicy.MinimumExpanding,
            )
            combobox_layout.addWidget(combobox_box)

            comboboxes.append(labelled_combobox)
            # Grid is 4x2
            grid.layout().addWidget(comboboxes[-1], (i - 1) // 4, (i - 1) % 4)
            combobox_box.currentTextChanged.connect(
                lambda x: self.on_gridComboBoxesUpdate(container, param, items, x)
            )

        # This has to be here so that the already-connected handlers catch the change
        entry.setText(self.mappers[param][1](self.pandoConfigManager.get(param)))
        return container

    def addHydraHarpCriSyncButton(self, confLayout, widget_dct, num_channels):
        container = LabelledContainer(
            "Sync All CRI parameters with channel 1 values", True
        )
        # container.layout().setSpacing(0)
        confLayout.addWidget(container)

        button = QtWidgets.QPushButton()
        button.setObjectName("hhSyncButton")
        button.setText("Sync")
        button.clicked.connect(
            lambda: self.on_hhSyncButtonClick(widget_dct, num_channels)
        )

        container.layout().addWidget(button)

        return container

    # Handler Helper Functions:
    def derive_pf32_g2_times(self):
        PF32_REBINNING_INDICES = (
            8,
            24,
            40,
            56,
        )  # zero-based tau indices at which rebinning occurs
        PF32_N_TAU = 64  # Number of g2 points produced by PF32 hardware correlator

        bin_size_ns = self.pandoConfigManager.get("bin_size_ns")
        rebin_factors = (
            self.pandoConfigManager.get("pf32_g2_rebin_factor_0"),
            self.pandoConfigManager.get("pf32_g2_rebin_factor_1"),
            self.pandoConfigManager.get("pf32_g2_rebin_factor_2"),
            self.pandoConfigManager.get("pf32_g2_rebin_factor_3"),
        )
        pf32_g2_frame_count = self.pandoConfigManager.get("pf32_g2_frame_count")

        # Calculate all tau values
        tau_nframes = 0
        tau_spacing = 1
        taus_ns = []
        for k in range(PF32_N_TAU):
            tau_nframes += tau_spacing

            taus_ns.append(bin_size_ns * tau_nframes)

            # Iteratively multiply tau_spacing at each rebinning point
            if k in PF32_REBINNING_INDICES:
                tau_spacing *= rebin_factors[PF32_REBINNING_INDICES.index(k)]

        # Display effective tau at each rebinning point
        for i in range(4):
            self.pf32g2WidgetDict["pf32_g2_rebin_factor_{}".format(i)].findChild(
                QtWidgets.QWidget, "tau"
            ).setText("{:.3f}ms".format(taus_ns[PF32_REBINNING_INDICES[i]] * 1.0e-6))

        # Display tau_max
        self.pf32g2WidgetDict["tau_max"].findChild(
            QtWidgets.QWidget, "tau_max"
        ).setText("{:.3f}ms".format(taus_ns[-1] * 1.0e-6))

        # Display effective integration period
        pf32_g2_integration_period_ns = pf32_g2_frame_count * bin_size_ns
        self.pf32g2WidgetDict["pf32_g2_frame_count"].findChild(
            QtWidgets.QWidget, "milliseconds"
        ).setText("{:.3f}ms".format(pf32_g2_integration_period_ns * 1.0e-6))

    def derive_actual_times(self):
        bin_size_ns = self.pandoConfigManager.get("bin_size_ns")
        points_per_level = self.pandoConfigManager.get("points_per_level")
        n_levels = self.pandoConfigManager.get("n_levels")
        rebin_factor = self.pandoConfigManager.get("rebin_factor")
        final_bin_count = self.pandoConfigManager.get("final_bin_count")

        final_bin_size_ns = bin_size_ns * (rebin_factor ** (n_levels - 1))

        tau_max_ns = (points_per_level - 1) * final_bin_size_ns
        integration_period_ns = final_bin_count * final_bin_size_ns

        # Display effective tau_max and integration period
        self.g2WidgetDict["tau_max"].findChild(QtWidgets.QWidget, "actual").setText(
            "{:.3f}ms".format(tau_max_ns * 1.0e-6)
        )
        self.g2WidgetDict["integration_period"].findChild(
            QtWidgets.QWidget, "actual"
        ).setText("{:.3f}ms".format(integration_period_ns * 1.0e-6))

        # Assign new limits to some config options
        self.g2WidgetDict["final_bin_count"].findChild(
            QtWidgets.QWidget, "entry"
        ).setMinimum(points_per_level)
        self.g2WidgetDict["final_bin_count"].findChild(
            QtWidgets.QWidget, "entry"
        ).setMaximum(min((2 ** 31 - 1), (2 ** 63 - 1) // final_bin_size_ns))
        self.g2WidgetDict["points_per_level"].findChild(
            QtWidgets.QWidget, "entry"
        ).setMaximum(min((2 ** 31 - 1), (2 ** 63 - 1) // final_bin_size_ns))
        # integration periods for other calculations must be gte the g2 integration period
        self.alldevicesWidgetDict["count_integ_period_ns"].findChild(
            QtWidgets.QWidget, "entry"
        ).setMinimum(integration_period_ns * 1.0e-6)
        self.alldevicesWidgetDict["dtof_integ_period_ns"].findChild(
            QtWidgets.QWidget, "entry"
        ).setMinimum(integration_period_ns * 1.0e-6)
        self.alldevicesWidgetDict["cri_integ_period_ns"].findChild(
            QtWidgets.QWidget, "entry"
        ).setMinimum(integration_period_ns * 1.0e-6)
        return (tau_max_ns, integration_period_ns)

    def derive_g2_parameters(self):
        bin_size_ns = self.pandoConfigManager.get("bin_size_ns")
        points_per_level = self.pandoConfigManager.get("points_per_level")
        rebin_factor = self.pandoConfigManager.get("rebin_factor")

        # Choose n_levels based on target_integration_period_ms
        target_tau_max_ns = (
            1000
            * 1000
            * self.g2WidgetDict["tau_max"]
            .findChild(QtWidgets.QWidget, "target")
            .value()
        )
        n_levels = (
            round(
                math.log(
                    target_tau_max_ns / ((points_per_level - 1) * bin_size_ns),
                    rebin_factor,
                )
            )
            + 1
        )

        # Subject calculated n_levels to the constraints imposed by the QSpinBox
        self.g2WidgetDict["n_levels"].findChild(QtWidgets.QWidget, "entry").setValue(
            n_levels
        )
        n_levels = (
            self.g2WidgetDict["n_levels"].findChild(QtWidgets.QWidget, "entry").value()
        )

        final_bin_size_ns = bin_size_ns * (rebin_factor ** (n_levels - 1))

        # Choose final_bin_count based on target_integration_period_ms
        target_integration_period_ms = (
            self.g2WidgetDict["integration_period"]
            .findChild(QtWidgets.QWidget, "target")
            .value()
        )
        final_bin_count = round(
            1.0e6 * target_integration_period_ms / final_bin_size_ns
        )

        self.g2WidgetDict["final_bin_count"].findChild(
            QtWidgets.QWidget, "entry"
        ).setValue(final_bin_count)

    def saveConfiguration(self, config_file):
        yamlDict = self.pandoConfigManager.as_dict()
        # translate device type to a string
        yamlDict["device"] = [
            key
            for key, val in pypando.Pando.DeviceType.__members__.items()
            if val == yamlDict["device"]
        ][0]

        yamlDict["pf32_bit_mode"] = [
            key
            for key, val in pypando.Pando.PF32BitMode.__members__.items()
            if val == yamlDict["pf32_bit_mode"]
        ][0]

        yamlDict["exp_type"] = [
            key
            for key, val in pypando.Pando.ExperimentType.__members__.items()
            if val == yamlDict["exp_type"]
        ][0]

        with open(config_file, "w") as f:
            yaml.dump(yamlDict, f)

    def loadPixelSelection(self, config_file):
        yamlDict = {}
        with open(config_file) as f:
            yamlDict = yaml.safe_load(f)

        self.pandoConfigManager.set("enabled_channels", yamlDict["enabled_channels"])

        # pixels will only be loaded for PF32 and mock devices
        mock_channels = self.mockChannelGridWidgetDict["enabled_channels"]
        mock_channels.resize_grid(
            self.pandoConfigManager.get("channel_rows"),
            self.pandoConfigManager.get("channel_columns"),
        )
        mock_channels.select_pixels(self.pandoConfigManager.get("enabled_channels"))
        self.pf32WidgetDict["enabled_channels"].select_pixels(
            self.pandoConfigManager.get("enabled_channels")
        )

    def loadConfiguration(self, config_file):
        yamlDict = {}
        with open(config_file) as f:
            yamlDict = yaml.safe_load(f)

        # Handle config items that were renamed so that old config files can still be used
        renamed_items = {
            "use_clementine": "use_pandoboxd",
            "clementine_ip_addr": "pandoboxd_ip_addr",
        }
        for old_name, new_name in renamed_items.items():
            if old_name in yamlDict:
                yamlDict[new_name] = yamlDict.pop(old_name)

        # translate device type to a DeviceType
        # yaml file must have a device type
        yamlDict["device"] = pypando.Pando.DeviceType.__members__[yamlDict["device"]]

        # translate bit mode enum
        if "pf32_bit_mode" in yamlDict.keys():
            yamlDict["pf32_bit_mode"] = pypando.Pando.PF32BitMode.__members__[
                yamlDict["pf32_bit_mode"]
            ]

        # the setup of the experiment type spinbox in on_DeviceTypeSelection will get messed up
        # if experiment type is set to an illegal value for the pre-load device.
        # So save the value, remove it from the yamlDict and set it after setting everything else.
        exp_type = None
        if (
            "exp_type" in yamlDict.keys()
        ):  # will not crash if using old config with no exp type
            exp_type = pypando.Pando.ExperimentType.__members__[
                yamlDict.pop("exp_type")
            ]

        # Set device type first so that the defaults get set up and the GUI gets updated before
        # setting the rest of the parameters.
        self.pandoConfigManager.set("device", yamlDict["device"])
        yamlDict.pop("device")

        self.pandoConfigManager.set_many(yamlDict)

        # now set the experiment type
        if exp_type is not None:
            self.pandoConfigManager.set("exp_type", exp_type)

        self.hydraHarpWidgetDict["enabled_channels"].findChild(
            QtWidgets.QWidget, "entry"
        ).setText(
            self.mappers["enabled_channels"][1](
                self.pandoConfigManager.get("enabled_channels")
            )
        )
        self.picoHarpWidgetDict["enabled_channels"].findChild(
            QtWidgets.QWidget, "entry"
        ).setText(
            self.mappers["enabled_channels"][1](
                self.pandoConfigManager.get("enabled_channels")
            )
        )
        self.multiHarpWidgetDict["enabled_channels"].findChild(
            QtWidgets.QWidget, "entry"
        ).setText(
            self.mappers["enabled_channels"][1](
                self.pandoConfigManager.get("enabled_channels")
            )
        )
        mock_channels = self.mockChannelGridWidgetDict["enabled_channels"]
        mock_channels.resize_grid(
            self.pandoConfigManager.get("channel_rows"),
            self.pandoConfigManager.get("channel_columns"),
        )
        mock_channels.select_pixels(self.pandoConfigManager.get("enabled_channels"))
        self.pf32WidgetDict["enabled_channels"].select_pixels(
            self.pandoConfigManager.get("enabled_channels")
        )

        self.pf32WidgetDict["pf32_laser_master"].findChild(
            QtWidgets.QWidget, "entry"
        ).setChecked(self.pandoConfigManager.get("pf32_laser_master"))

    def EnableNonRunTimeParameters(self, enabled):
        for key, w in self.allDevWidgetsList:
            if self.disable_all_params_at_runtime:
                w.setEnabled(enabled)
            elif not w.runtimeEditable:
                w.setEnabled(enabled)
        self.sysConfigGroup.setEnabled(enabled)

    # Handler functions:

    def on_ChannelsUpdate(self, w):
        entryW = w.findChild(QtWidgets.QWidget, "entry")
        entryW.blockSignals(True)  # block entry signals

        channels_list = entryW.text().split(",")
        children_list = w.findChildren(QtWidgets.QWidget)
        for child in children_list:
            checkbox = child.findChild(QtWidgets.QCheckBox)
            if checkbox is not None:
                checkbox.blockSignals(True)  # block checkbox signals
                if child.objectName() in channels_list:
                    checkbox.setChecked(True)
                else:
                    checkbox.setChecked(False)
                checkbox.blockSignals(False)  # unblock checkbox signals

        entryW.blockSignals(False)  # unblock entry signals
        # set the value in the config struct
        self.pandoConfigManager.set(
            "enabled_channels", self.mappers["enabled_channels"][0](entryW.text())
        )

    def on_ChecksUpdate(self, w):
        entryW = w.findChild(QtWidgets.QWidget, "entry")
        channels = ""

        for child in w.findChildren(QtWidgets.QWidget):
            checkbox = child.findChild(QtWidgets.QCheckBox)
            if checkbox is not None:
                checkbox.blockSignals(True)  # block checkbox signals
                if checkbox.isChecked():
                    channels += child.objectName() + ","

        # block entry signals and set entry text
        entryW.blockSignals(True)
        entryW.setText(channels)
        entryW.blockSignals(False)

        # unblock checkbox signals
        for child in w.findChildren(QtWidgets.QWidget):
            checkbox = child.findChild(QtWidgets.QCheckBox)
            if checkbox is not None:
                checkbox.blockSignals(False)

        # set the value in the config struct
        self.pandoConfigManager.set(
            "enabled_channels", self.mappers["enabled_channels"][0](entryW.text())
        )

    def on_gridComboBoxesUpdate(self, w, key, items, index):
        entryW = w.findChild(QtWidgets.QWidget, "entry")
        entry_value = ""

        grid = w.findChild(QtWidgets.QWidget, "grid")
        for child in grid.findChildren(QtWidgets.QWidget):
            combobox = child.findChild(QtWidgets.QComboBox)
            if combobox is not None:
                combobox.blockSignals(True)  # block spinbox signals
                entry_value += (
                    str(int(child.objectName()) - 1)
                    + ":"
                    + str(combobox.currentIndex())
                    + ","
                )

        entry_value = entry_value[:-1]

        # block entry signals and set entry text
        entryW.blockSignals(True)
        entryW.setText(entry_value)
        entryW.blockSignals(False)

        # unblock spinbox signals
        for child in grid.findChildren(QtWidgets.QWidget):
            combobox = child.findChild(QtWidgets.QComboBox)
            if combobox is not None:
                combobox.blockSignals(False)

        # set the value in the config struct
        self.pandoConfigManager.set(key, self.mappers[key][0](entryW.text()))

    def on_gridComboEntryUpdate(self, w, key, values, channel_count, index):
        entryW = w.findChild(QtWidgets.QWidget, "entry")
        entryW.blockSignals(True)

        entry_dct = {}
        # get the entry text or set to all defaults if empty
        if entryW.text() != "":
            entry_dct = self.mappers["hharp_input_offsets"][0](entryW.text())

        # set any unset channels to default (first) value
        for k in range(channel_count):
            if k not in entry_dct.keys():
                entry_dct[k] = 0
        grid = w.findChild(QtWidgets.QWidget, "grid")
        for k, v in entry_dct.items():
            box = grid.findChild(QtWidgets.QWidget, str(k + 1)).findChild(
                # translate from 0-indexed struct to 1-indexed user facing parameters
                QtWidgets.QComboBox
            )
            if box is not None:
                box.blockSignals(True)
                box.setCurrentIndex(v)
                box.blockSignals(False)

        entryW.blockSignals(False)

        # set the value in the config struct
        self.pandoConfigManager.set(key, self.mappers[key][0](entryW.text()))

    def on_targetTauMaxUpdate(self):
        self.derive_g2_parameters()

    def on_targetIntegrationPeriodUpdate(self):
        self.derive_g2_parameters()

    def on_autoCalcG2ParamsStateChanged(self, checked):
        # Here, we enable/disable the actual spinboxes, whereas EnableNonRunTimeParameters
        # enables/disables the LabelledContainer that holds them. This distinction allows
        # the two enablement strategies to coexist (the control is only usable if both it
        # and its container are enabled)
        if checked:
            self.g2WidgetDict["tau_max"].findChild(
                QtWidgets.QWidget, "target"
            ).setEnabled(True)
            self.g2WidgetDict["integration_period"].findChild(
                QtWidgets.QWidget, "target"
            ).setEnabled(True)
        else:
            self.g2WidgetDict["tau_max"].findChild(
                QtWidgets.QWidget, "target"
            ).setEnabled(False)
            self.g2WidgetDict["integration_period"].findChild(
                QtWidgets.QWidget, "target"
            ).setEnabled(False)

    def on_outputDirBrowse(self):
        output_dir = QtWidgets.QFileDialog.getExistingDirectory(
            self, "Select HDF5 output directory"
        )
        # don't change if nothing was selected
        if output_dir != "":
            self.pandoConfigManager.set("output_dir", output_dir)

    def on_MockChannelsUpdate(self, widgetDict):
        # get new ncols and _nrows
        # resize mock pixel grid
        # profit
        ncols = self.pandoConfigManager.get("channel_columns")
        nrows = self.pandoConfigManager.get("channel_rows")
        grid = widgetDict["enabled_channels"]
        grid.resize_grid(nrows, ncols)
        grid.set_selection_callback(lambda: self.on_GridChannelSelection(widgetDict))

    def on_mockFileBrowse(self):
        mock_file, filter_ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Select Mock File", filter="Data files (*.ptu *.h5)"
        )
        if mock_file != "":
            self.pandoConfigManager.set("mock_file", mock_file)

    def on_pf32FirmwareBrowse(self):
        firmware_file, filter_ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Select Firmware File", filter="Data files (*.bit)"
        )
        if firmware_file != "":
            self.pandoConfigManager.set("pf32_firmware_path", firmware_file)

    def on_pf32LaserMasterCheck(self, checked):
        self.pandoConfigManager.set("pf32_laser_master", checked)

    def on_configFileBrowse(self):
        config_file, filter_ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Select Configuration File", filter="YAML files (*.yaml)"
        )
        if config_file != "":
            self.loadConfiguration(config_file)

    def on_pixmapFileBrowse(self):
        config_file, filter_ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Select Configuration File", filter="YAML files (*.yaml)"
        )
        if config_file != "":
            self.loadPixelSelection(config_file)

    def on_configFileSave(self):
        config_file, filter_ = QtWidgets.QFileDialog.getSaveFileName(
            self, "Select Configuration File", filter="YAML files (*.yaml)"
        )
        if config_file != "":
            self.saveConfiguration(config_file)

    def on_configManager_updated(self, which):
        self.derive_actual_times()
        self.derive_pf32_g2_times()
        self.deviceType.findChild(QtWidgets.QWidget, "entry").setCurrentText(
            [
                key
                for key, val in pypando.Pando.DeviceType.__members__.items()
                if val == self.pandoConfigManager.get("device")
            ][0]
        )

        # Allow user to apply the new config
        self.configureButton.setEnabled(True)

    def on_hhSyncButtonClick(self, widget_dct, num_channels):
        """Set the CRi parameters for all channels to the value of channel 1"""
        offset_w = widget_dct["cri_offset"].findChild(QtWidgets.QWidget, "entry")
        width_w = widget_dct["cri_width"].findChild(QtWidgets.QWidget, "entry")

        offset_dct = self.mappers["cri_offset"][0](offset_w.text())
        width_dct = self.mappers["cri_width"][0](width_w.text())

        # accounts for channels that are currently not in the map
        offset = 0
        if 0 in offset_dct.keys():
            offset = offset_dct[0]
        width = 0
        if 0 in width_dct.keys():
            width = width_dct[0]

        # set the values
        for key in range(0, num_channels):
            offset_dct[key] = offset

        for key in range(0, num_channels):
            width_dct[key] = width

        offset_str = self.mappers["cri_offset"][1](offset_dct)
        width_str = self.mappers["cri_width"][1](width_dct)

        offset_w.setText(offset_str)
        width_w.setText(width_str)

    def on_configure(self):
        self.configureButton.setEnabled(False)
        try:
            self.pando.Configure(
                self.pandoConfigManager.get_config().get_wrapped_config()
            )
        except:
            QtWidgets.QMessageBox.critical(
                self,
                "Error",
                "Encounterd an exception while configuring Pando, check log for details.",
            )
            self.configureButton.setEnabled(True)

    def on_healthTimeout(self):
        if not self.pando.IsHealthy():
            self.on_stop()

            QtWidgets.QMessageBox.critical(
                self,
                "Error",
                "Encountered an exception running experiment, check log for details.",
            )

    def on_start(self):
        self.configureButton.setEnabled(False)
        self.startButton.setEnabled(False)

        try:
            self.pando.Configure(
                self.pandoConfigManager.get_config().get_wrapped_config()
            )
        except:
            QtWidgets.QMessageBox.critical(
                self,
                "Error",
                "Encounterd an exception while configuring Pando, check log for details.",
            )
            self.configureButton.setEnabled(True)
            self.startButton.setEnabled(True)
        else:
            try:
                self.acquisition_handle = self.pando.Start()
            except:
                QtWidgets.QMessageBox.critical(
                    self,
                    "Error",
                    "Encounterd an exception while starting Pando, check log for details.",
                )
                self.startButton.setEnabled(True)
            else:
                self.startButton.setEnabled(False)
                self.stopButton.setEnabled(True)
                self.healthTimer.start(1000)
                self.EnableNonRunTimeParameters(False)
                if self.pandoConfigManager.get("auto_stop"):
                    exp_length_ms = (
                        self.pandoConfigManager.get("experiment_length") * 1000
                    )
                    self.stop_timer.start(exp_length_ms)

    def on_stop(self):

        self.healthTimer.stop()
        self.stop_timer.stop()

        self.stopButton.setEnabled(False)
        self.stopProgress.show()

        # Give the UI thread a little bit of time to update the "Saving.." notification
        # before locking everything up
        QtCore.QTimer.singleShot(100, self.on_stop_async)

    def on_auto_stop(self):
        self.on_stop()

        QtWidgets.QMessageBox.information(
            self,
            "Auto Stop",
            "Experiment has auto-stopped after running for "
            + str(self.pandoConfigManager.get("experiment_length"))
            + "s.",
        )

    def on_stop_async(self):
        # Destroy existing acquisition_handle, this causes pando to stop.
        self.acquisition_handle = None
        self.startButton.setEnabled(True)
        self.EnableNonRunTimeParameters(True)
        self.stopProgress.hide()

    def on_GridChannelSelection(self, widgetDict):
        enabled_channels = widgetDict["enabled_channels"].get_enabled_pixels()
        self.pandoConfigManager.set("enabled_channels", enabled_channels)

    def on_DeviceTypeSelection(self, dev):
        """Shows device-specific widgets based on selected device. Ties handler to these widgets."""

        # Update the values allowed for exp_type based on new device type:
        # this resets the value prior to the updates so configMangager doesn't get confused after if the value no longer exists
        self.expType.findChild(QtWidgets.QWidget, "entry").setCurrentText("NONE")
        self.pandoConfigManager.remove_handler(
            "exp_type"
        )  # need to remove so clear() doesn't confuse configManager
        # Add new item set
        self.expType.findChild(QtWidgets.QWidget, "entry").clear()
        self.expType.findChild(QtWidgets.QWidget, "entry").addItems(
            self.devTypeToExpTypes[dev]
        )
        # Replace the handler
        self.pandoConfigManager.add_handler(
            "exp_type",
            self.expType.findChild(QtWidgets.QWidget, "entry"),
            self.mappers["exp_type"],
        )

        # Set device type
        self.pandoConfigManager.set("device", pypando.Pando.DeviceType.__members__[dev])

        # Remove current handlers and hide widgets
        for key, w in self.allDevWidgetsList:
            self.pandoConfigManager.remove_handler(key)
            w.hide()

        # change enabled tabs
        for key, i in self.tabsDict.items():
            self.devTabs.setTabEnabled(i, False)
        self.devTabs.setTabEnabled(self.tabsDict["General"], True)  # enable general
        self.devTabs.setTabEnabled(self.tabsDict["G2"], True)  # enable G2
        self.devTabs.setCurrentIndex(0)

        # select required widget dicts
        widgetDict = {}
        widgetDict.update(self.alldevicesWidgetDict)

        if dev == "PF32_G2":
            self.disable_all_params_at_runtime = True
        else:
            self.disable_all_params_at_runtime = False

        if dev == "HYDRA_HARP_T2":
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.hydraHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Channel"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "HYDRA_HARP_T3":
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.hydraHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Channel"], True)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "PICO_HARP_T2":
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.picoHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Channel"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "PICO_HARP_T3":
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.picoHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Channel"], True)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "MULTI_HARP_T2":
            # widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.multiHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Channel"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "MULTI_HARP_T3":
            # widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.multiHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Channel"], True)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "PF32_DCS":
            self.pf32WidgetDict["pf32_laser_master"].setEnabled(False)
            widgetDict.update(self.pf32WidgetDict)
            widgetDict.update(self.pf32NonG2WidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "PF32_TCSPC":
            self.pf32WidgetDict["pf32_laser_master"].setEnabled(True)
            widgetDict.update(self.pf32WidgetDict)
            widgetDict.update(self.pf32NonG2WidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
        elif dev == "PF32_G2":
            self.pf32WidgetDict["pf32_laser_master"].setEnabled(False)
            widgetDict.update(self.pf32WidgetDict)
            widgetDict.update(self.pf32g2WidgetDict)
            # PF32_G2 has different set of g2 params than other devices
            self.devTabs.setTabEnabled(self.tabsDict["G2"], False)
            self.devTabs.setTabEnabled(self.tabsDict["PF32 Correlator"], True)
        elif dev == "HYDRA_HARP_T2_MOCK_PTU":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.hydraHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "HYDRA_HARP_T3_MOCK_PTU":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.hydraHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "PICO_HARP_T2_MOCK_PTU":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.picoHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "PICO_HARP_T3_MOCK_PTU":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.picoHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "MULTI_HARP_T2_MOCK_PTU":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.multiHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "MULTI_HARP_T3_MOCK_PTU":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.xHarpWidgetDict)
            widgetDict.update(self.multiHarpWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "TIMETAGGER_MOCK_HDF5":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.mockChannelGridWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            widgetDict.update(self.dtofWidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["DTOF"], True)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "HISTOGRAMMER_MOCK_HDF5":
            widgetDict.update(self.mockDeviceWidgetDict)
            widgetDict.update(self.mockChannelGridWidgetDict)
            widgetDict.update(self.g2WidgetDict)
            self.devTabs.setTabEnabled(self.tabsDict["Count Rate"], True)
        elif dev == "BASLER_A2":
            widgetDict.update(self.cameraWidgetDict)
        elif dev == "ORCA_FUSION":
            widgetDict.update(self.cameraWidgetDict)
        elif dev == "BASLER_ACA2000_CL":
            widgetDict.update(self.cameraWidgetDict)
        elif dev == "BASLER_BOOST":
            widgetDict.update(self.cameraWidgetDict)
        elif dev == "CAMERA_MOCK_HDF5":
            widgetDict.update(self.mockDeviceWidgetDict)

        # Set values to default
        for k, v in self.default_config_dict.items():
            if (
                dev in self.default_config_device_overrides
                and k in self.default_config_device_overrides[dev]
            ):
                self.pandoConfigManager.set(
                    k, self.default_config_device_overrides[dev][k]
                )
            else:
                self.pandoConfigManager.set(k, v)

        # show widgets and add handlers if appropriate
        for v, w in widgetDict.items():
            w.show()
            obj = w.findChild(QtWidgets.QWidget, "entry")
            # this ignores widgets such as 'tau_max' that don't correspond to a config value
            if obj is not None:
                if v in self.mappers:
                    self.pandoConfigManager.add_handler(v, obj, self.mappers[v])
                else:
                    self.pandoConfigManager.add_handler(v, obj)

        # Reset the pixel grids to the newly reset enabled_channels
        mock_channels = self.mockChannelGridWidgetDict["enabled_channels"]
        mock_channels.resize_grid(
            self.pandoConfigManager.get("channel_rows"),
            self.pandoConfigManager.get("channel_columns"),
        )
        mock_channels.select_pixels(self.pandoConfigManager.get("enabled_channels"))
        self.pf32WidgetDict["enabled_channels"].select_pixels(
            self.pandoConfigManager.get("enabled_channels")
        )

    def on_EnablePandoboxd(self, enabled):
        if enabled:
            self.cameraWidgetDict["aux_frame_trigger_period_10ns"].setEnabled(True)
        else:
            self.cameraWidgetDict["aux_frame_trigger_period_10ns"].setEnabled(False)
            self.pandoConfigManager.set("aux_frame_trigger_period_10ns", 0)


def main():
    app = QtWidgets.QApplication(sys.argv)
    win = ConfigWidget()
    win.setWindowTitle("ConfigWidget")
    win.show()
    app.exec()


if __name__ == "__main__":
    main()
