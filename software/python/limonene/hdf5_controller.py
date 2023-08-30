import logging
import sys
import h5py as h5
from limonene.controller_base import LimoneneController, Sample

from arvr import pandoboxd_rpc_pb2 as clem_pb
from arvr import pando_pb2 as jul_pb

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("controller")


class OnlineValidationTestDataReader:
    def __init__(self, raw_fname, analyzed_fname, peripheral_fname):
        self.raw_fname = raw_fname
        self.analyzed_fname = analyzed_fname
        self.peripheral_fname = peripheral_fname

        # Set marker dataset, currently always channel 0
        self.markers_dataset = "MarkersChannel0"

        # Find correct timestamp dataset, channel 1 on PH-T3, channel 0 otherwise
        with h5.File(self.raw_fname, "r") as raw_file:
            if "TimestampsChannel0" in raw_file.keys():
                self.timestamps_dataset = "TimestampsChannel0"
            elif "TimestampsChannel1" in raw_file.keys():
                self.timestamps_dataset = "TimestampsChannel1"
            else:
                log.error("TimestampsChannel[0/1] not present in HDF5 file")
                sys.exit(1)

        # Get total row count for all the datasets
        with h5.File(self.raw_fname, "r") as raw_file:
            self.timestamps_total_rows = raw_file[self.timestamps_dataset].len()
            self.markers_total_rows = raw_file[self.timestamps_dataset].len()

        with h5.File(self.analyzed_fname, "r") as analyzed_file:
            self.pps_stats_total_rows = analyzed_file["PpsStats"].len()

        with h5.File(self.peripheral_fname, "r") as peripheral_file:
            self.digital_inputs_total_rows = peripheral_file["DigitalIn"].len()

        # Set up operation ordering for validation testing
        # On each call to read_next_sample, executes the function at
        # order_of_operations[operation_index] and increments operation_index
        self.order_of_operations = (
            [lambda: self.sample_timestamps(5)]
            + [self.sample_digital_inputs] * 5
            + [self.sample_markers, self.sample_pps_stats]
        )

        self.reset()

    def reset(self, experiment_id=0):
        self.experiment_id = experiment_id

        self.timestamp_seq_num = 0
        self.markers_seq_num = 0
        self.pps_stats_seq_num = 0
        self.digital_inputs_seq_num = 0

        self.timestamps_current_row = 0
        self.markers_current_row = 0
        self.pps_stats_current_row = 0
        self.digital_inputs_current_row = 0

        self.operation_index = 0

    def read_next_sample(self):
        sample = None
        starting_operation_index = self.operation_index
        while sample is None:
            sample = self.order_of_operations[self.operation_index]()
            self.operation_index = (self.operation_index + 1) % len(
                self.order_of_operations
            )

            # If all operations have returned None then all data has been read.
            if sample is None and (self.operation_index == starting_operation_index):
                return None

        return sample

    def sample_timestamps(self, n_rows=1):
        if self.timestamps_current_row < self.timestamps_total_rows:
            with h5.File(self.raw_fname, "r") as raw_file:
                packet = jul_pb.Packet()

                packet.header.experiment_id = self.experiment_id
                packet.header.sequence_number = self.timestamp_seq_num
                packet.header.timestamp = (
                    self.timestamp_seq_num
                )  # note that the timestamp field in timestamp message isn't used
                self.timestamp_seq_num += 1

                timestamps_field = packet.payload.timestamps
                timestamps_channel = timestamps_field.channels[
                    int(self.timestamps_dataset[-1])
                ]

                final_row = self.timestamps_current_row + n_rows - 1
                if final_row >= self.timestamps_total_rows:
                    final_row = self.timestamps_total_rows - 1

                while self.timestamps_current_row <= final_row:
                    mtime = raw_file[self.timestamps_dataset]["macro_times"][
                        self.timestamps_current_row
                    ]
                    utime = raw_file[self.timestamps_dataset]["micro_times"][
                        self.timestamps_current_row
                    ]

                    timestamps_channel.macro_times.append(mtime)
                    timestamps_channel.micro_times.append(utime)

                    self.timestamps_current_row += 1

                return Sample("pando.time_tags_array", packet)

        else:
            return None

    def sample_markers(self, n_rows=1):
        if self.markers_current_row < self.markers_total_rows:
            with h5.File(self.raw_fname, "r") as raw_file:
                packet = jul_pb.Packet()

                packet.header.experiment_id = self.experiment_id
                packet.header.sequence_number = self.markers_seq_num
                packet.header.timestamp = (
                    self.markers_seq_num
                )  # note that the timestamp field in markers message isn't used
                self.markers_seq_num += 1

                markers_field = packet.payload.markers
                markers_channel = markers_field.channels[int(self.markers_dataset[-1])]

                final_row = self.markers_current_row + n_rows - 1
                if final_row >= self.markers_total_rows:
                    final_row = self.markers_total_rows - 1

                while self.markers_current_row <= final_row:
                    mtime = raw_file[self.markers_dataset]["macro_times"][
                        self.markers_current_row
                    ]
                    markers_channel.macro_times.append(mtime)
                    self.markers_current_row += 1

                return Sample("pando.markers", packet)

        else:
            return None

    def sample_pps_stats(self):
        if self.pps_stats_current_row < self.pps_stats_total_rows:
            with h5.File(self.analyzed_fname, "r") as analyzed_file:
                packet = jul_pb.Packet()

                packet.header.experiment_id = self.experiment_id
                packet.header.sequence_number = self.pps_stats_seq_num
                packet.header.timestamp = analyzed_file["PpsStats_headers"][
                    "timestamp"
                ][self.pps_stats_current_row]
                self.pps_stats_seq_num += 1

                pps_stats_field = packet.payload.pps_stats

                pps_stats_field.jitter_ns = analyzed_file["PpsStats"]["jitter_ns"][
                    self.pps_stats_current_row
                ]
                pps_stats_field.offset_ns = analyzed_file["PpsStats"]["offset_ns"][
                    self.pps_stats_current_row
                ]

                self.pps_stats_current_row += 1

                return Sample("pando.pps_stats", packet)
        else:
            return None

    def sample_digital_inputs(self):
        if self.digital_inputs_current_row < self.digital_inputs_total_rows:
            with h5.File(self.peripheral_fname, "r") as peripheral_file:
                packet = jul_pb.Packet()

                packet.header.experiment_id = self.experiment_id
                packet.header.sequence_number = self.digital_inputs_seq_num
                packet.header.timestamp = peripheral_file["DigitalIn_headers"][
                    "timestamp"
                ][self.digital_inputs_current_row]
                self.digital_inputs_seq_num += 1

                digital_in_field = packet.payload.digital_in

                digital_in_field.device_id = peripheral_file["DigitalIn"]["device_id"][
                    self.digital_inputs_current_row
                ]
                digital_in_field.edge = peripheral_file["DigitalIn"]["edge"][
                    self.digital_inputs_current_row
                ]

                self.digital_inputs_current_row += 1

                return Sample("pandoboxd.digital_in", packet)
        else:
            return None


class HDF5Controller(LimoneneController):
    """Processes a series of hdf5 files and returns samples as if we were
    connected to pando.  Useful for playing back data for algorithm
    devleopment without direct access to hardware

    """

    def __init__(
        self, hdf5_files=None, sort_buffer_size=16,
    ):
        # TODO: Parse the hdf5_files (a list of strings) into the appropriate objects
        super().__init__(False, sort_buffer_size)

        analyzed_fname, peripheral_fname, raw_fname = sorted(hdf5_files.split())
        self.online_validation_reader = OnlineValidationTestDataReader(
            raw_fname, analyzed_fname, peripheral_fname
        )

    def start(self, experiment_id=0):
        self.online_validation_reader.reset(experiment_id=experiment_id)
        return True

    def ping(self):
        return True

    def _read_sample(self):
        """Reads a single sample from the HDF files.  Returns a
        Sample object.

        If there is no data left to return, simply returns None

        """
        smp = self.online_validation_reader.read_next_sample()
        return smp

    def stop(self):
        """Ends the current experiment run

        """
        return True
