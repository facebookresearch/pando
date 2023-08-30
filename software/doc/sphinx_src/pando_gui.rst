.. cpp:namespace:: pnd::pando

==========
Pando GUI
==========
The Pando GUI is a frontend for libpando that presents all of the available settings in a (hopefully) intuitive format.

.. figure:: img/gui_main.png
   
   Screenshot of the Pando GUI

Experiment Control
------------------
Configure
  Applies the current configuration as specified by all other GUI elements.
  Settings don't take effect until either **Configure** or **Start** is clicked.
  See :cpp:func:`PandoInterface::Configure`

Start
  Applies the current configuration, then starts a new experiment.
  See :cpp:func:`PandoInterface::Start`

Stop
  Stop the current experiment and wait until all in-flight data has been written to the HDF5 files.

System Configuration Pane
-------------------------

Config file
  Load/save all settings (from all panes, visible or not) to a YAML file.

.. list-table::
   :header-rows: 1

   * - Setting
     - Description
   * - Device Type
     - See :cpp:member:`PandoInterface::Config::device`
   * - Experiment Type
     - See :cpp:member:`PandoInterface::Config::exp_type`
   * - Output Directory
     - See :cpp:member:`PandoInterface::Config::output_dir`
   * - Auto Stop
     - See :cpp:member:`PandoInterface::Config::auto_stop`
   * - Publish Raw Data
     - See :cpp:member:`PandoInterface::Config::publish_raw_data`
   * - Log Raw Data
     - See :cpp:member:`PandoInterface::Config::log_raw_data`
   * - Log Analyzed Data
     - See :cpp:member:`PandoInterface::Config::log_analyzed_data`
   * - Log Peripheral Data
     - See :cpp:member:`PandoInterface::Config::log_peripheral_data`
   * - Enable Pandoboxd
     - See :cpp:member:`PandoInterface::Config::use_pandoboxd`
   * - Adjust timestamps using PPS
     - See :cpp:member:`PandoInterface::Config::use_pps`
   * - Pandoboxd IP
     - See :cpp:member:`PandoInterface::Config::pandoboxd_ip_addr`
   * - Experiment Length
     - See :cpp:member:`PandoInterface::Config::experiment_length`

General Tab
-----------

Settings related to general device operation.

.. list-table::
   :header-rows: 1

   * - Setting
     - Description
   * - Enabled Channels
     - See :cpp:member:`PandoInterface::Config::enabled_channels`
   * - Laser Sync Rep Period
     - See :cpp:member:`PandoInterface::Config::laser_sync_period_ps`
   * - Sync Divider
     - See :cpp:member:`PandoInterface::Config::xharp_sync_div`

Channel Tab
-----------
Settings related to the individual input channel frontends of PicoQuant devices.

.. list-table::
   :header-rows: 1

   * - Setting
     - Description
   * - Sync (Channel 0) CFD level
     - See :cpp:member:`PandoInterface::Config::xharp_sync_level`
   * - Sync (Channel 0) Zero Crossing level
     - See :cpp:member:`PandoInterface::Config::xharp_sync_zerox`
   * - Channel CFD level
     - See :cpp:member:`PandoInterface::Config::xharp_level`
   * - Channel Zero Crossing
     - See :cpp:member:`PandoInterface::Config::xharp_zerox`
   * - Channel Zero Crossing
     - See :cpp:member:`PandoInterface::Config::xharp_zerox`
   * - Channel Offsets
     - See :cpp:member:`PandoInterface::Config::hharp_input_offsets`

The MultiHarp has unique channel settings (the inputs don't have CFDs):

.. list-table::
   :header-rows: 1

   * - Setting
     - Description
   * - Sync Trigger Edge
     - See :cpp:member:`PandoInterface::Config::multi_harp_sync_trigger_edge`
   * - Sync Trigger Level
     - See :cpp:member:`PandoInterface::Config::multi_harp_sync_trigger_level_mv`
   * - Sync Tdead
     - See :cpp:member:`PandoInterface::Config::multi_harp_sync_tdead_ps`
   * - Trigger edges
     - See :cpp:member:`PandoInterface::Config::multi_harp_sync_trigger_edge`
   * - Trigger Levels
     - See :cpp:member:`PandoInterface::Config::multi_harp_trigger_level_mv`
   * - Tdead
     - See :cpp:member:`PandoInterface::Config::multi_harp_tdead_ps`