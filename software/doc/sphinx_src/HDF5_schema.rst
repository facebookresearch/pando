===========
HDF5 Schema
===========

Pando is capable of logging all of the data it acquires and produces to disk in the form of HDF5 files. These files can be conveniently analyzed off-line in many languages (eg. `Python <https://docs.h5py.org>`_, `Julia <https://juliaio.github.io/HDF5.jl>`_, `Matlab <https://www.mathworks.com/help/matlab/import_export/importing-hierarchical-data-format-hdf5-files.html>`_.) `HDFView <https://www.hdfgroup.org/downloads/hdfview/>`_ is useful for quick inspection.

Raw, Analyzed, and Peripheral HDF5 files
----------------------------------------

Pando splits the data it logs into three different HDF5 files: Raw, Analyzed, and Peripheral. The creation of each of these files can be disabled individually as needed. For example, the raw file can become extremely large (terabytes over a 1 hour experiment, depending on the device in use), so it's often necessary to disable its creation.

Raw File (``sawyer_raw_YYYY-MM-DD-HHMMSS.h5``)
  Contains unprocessed "raw data" produced by the timetagger device. See: :ref:`raw_types`
Analyzed File (``sawyer_analyzed_YYYY-MM-DD-HHMMSS.h5``)
  Contains the results of processing performed by Pando. See: :ref:`analyzed_types`
Peripheral File (``sawyer_peripheral_YYYY-MM-DD-HHMMSS.h5``)
  Contains data acquired by Pandoboxd (the Pando companion application that runs on the sync box.) See: :ref:`peripheral_types`

Relationship with Protobuf Schema
---------------------------------
For the most part, the HDF5 schema follows directly from the :doc:`protobuf_schema`. There are three schemes in use (detailed below) for translating instances of our top-level protobuf message type (:ref:`packet`) into rows in HDF5 datasets. In general, the elements of the HDF5 datasets are `compound datatypes <https://portal.hdfgroup.org/display/HDF5/Datatype+Basics#DatatypeBasics-compound>`_ whose fields directly correspond to those in protobuf messages.
With the exception of raw data, each protobuf packet gets translated into a single row in each of several associated datasets. There is a one-to-one mapping between rows in associated datasets. For example, row 100 in the G2i_headers dataset would contain the timestamp for row 100 in the G2i dataset.


Multi-Channel Dataset Groups
++++++++++++++++++++++++++++

As discussed in :ref:`meta`, many payload types contain data pertaining to multiple channels. We translate these messages into three HDF5 datasets:

[*PayloadName*]_headers
  Corresponds to the ``headers`` field of the :ref:`packet` message type.
[*PayloadName*]_channels
  Corresponds to the ``channels`` field of the message present in the ``payload`` field of the :ref:`packet` message type. This is a 2D dataset; each column represents a single channel. The dataset has an attribute named ``selected_channels``, which is an array of channel indices indicating which device channel is represented by each column.
[*PayloadName*]_meta
  Corresponds to the ``meta`` field of the message present in the ``payload`` field of the :ref:`packet` message type.

Channel-Less Dataset Groups
+++++++++++++++++++++++++++
Other payload types (mostly those enumerated in :ref:`peripheral_types`) do not pertain to multiple channels. We refer to these payloads as "ChannelLess". They are translated into two HDF5 datasets:

[*PayloadName*]_headers
  Corresponds to the ``headers`` field of the :ref:`packet` message type (Exactly as in the Multi-channel case.)
[*PayloadName*]
  Corresponds to the message present in the ``payload`` field of the :ref:`packet` message type. 
  
Raw dataset groups
++++++++++++++++++
The previous two translation schemes are implemented programmatically: the HDF5 types are automatically generated from the protobuf types, so we don't have to implement or maintain the translations by hand. For performance reasons, raw data logging is not implemented this way, and so the HDF5 schema looks a little different.

There are three types of raw data: Timestamps, Histograms, and Markers (See :ref:`raw_types`.) A given device produces either timestamps or histograms, but not both, so only one of these datasets can be present in a file. The Markers dataset is always present.

Timestamps
  Data from each enabled channel is stored in its own 1D dataset named **TimestampsChannel[n]**, where **[n]** is the channel index. The selected_channels attribute of each dataset also indicates the channel index.
  Each cell in the **TimestampsChannel[n]** contains the following fields:

  macro_times
    A photon macrotime, in picoseconds
  micro_times
    A photon microtime, in picoseconds. Set to 0 for PicoQuant devices in T2 mode.

Markers
  Similar to photon timestamps, Marker timestamps are stored in datasets named **MarkersChannel[n]**. Unlike photon timestamps, there is no micro_times field.

Histograms
  Data from all channels is stored in a single 2D dataset named **Histograms**, where each column contains counts from a single channel and each row represents counts from a single g2 Tint. As with 2D datasets in the analyzed file, the selected_channels dataset attribute indicates which device channel is represented by each dataset column.

  Each cell in the **Histograms** dataset contains the following fields:

  bin_size
    the histogram bin width in nanoseconds
  first_bin_idx
    The experiment-global index of the first bin stored in the counts field
  last_bin_idx
    The experiment-global index of the last bin stored in the counts field
  counts
    An array of last_bin_idx - first_bin_idx + 1 photon counts from contiguous bins with indices first_bin_idx through last_bin_idx
