import sys
import logging

from arvr import pandoboxd_rpc_pb2 as clem_pb
from arvr import pando_pb2 as jul_pb

from limonene.limonene_module import LimoneneModule

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("validation")

# class Statistics:
#   def __init__(self):
#       self.sum = 0
#       self.sum_of_squares = 0
#       self.n_samples = 0
#       self.max = 0

#   def add_samples(self, list_of_samples):
#       self.sum += sum(list_of_samples)
#       self.sum_of_squares += sum([i**2 for i in list_of_samples])
#       self.n_samples += len(list_of_samples)
#       for item in list_of_samples:
#           if abs(item) > abs(self.max):
#               self.max = item

#   def finalize(self):
#       mean = self.sum / float(self.n_samples)
#       stddev = ((self.sum_of_squares - 2*mean*self.sum + self.n_samples*mean**2) / float(self.n_samples - 1))**(0.5)
#       return mean, self.max, stddev

# My concern with the original stats implementation (above) is that sum and sum_of_squares could get large
# Possible that they could overflow if an experiment runs long enough
# So this complicates the math, but should hopefully keep the values smaller, assuming python respects OoO correctly
class Statistics:
    def __init__(self):
        self.sum = 0
        self.mean = 0
        self.stddev_square_term = 0
        self.stddev_linear_term = 0
        self.n_samples = 0
        self.max = 0

    def add_samples(self, list_of_samples):
        new_n_samples = self.n_samples + len(list_of_samples)

        self.stddev_square_term = (
            (self.n_samples - 1) / float(new_n_samples - 1)
        ) * self.stddev_square_term + sum([i ** 2 for i in list_of_samples]) / float(
            new_n_samples - 1
        )
        self.stddev_linear_term = (
            (self.n_samples - 1) / float(new_n_samples - 1)
        ) * self.stddev_linear_term + sum(list_of_samples) / float(new_n_samples - 1)
        self.mean = (self.n_samples / float(new_n_samples)) * self.mean + sum(
            list_of_samples
        ) / float(new_n_samples)

        self.n_samples = new_n_samples

        for item in list_of_samples:
            if abs(item) > abs(self.max):
                self.max = item

    def finalize(self):
        stddev = (
            self.stddev_square_term
            - 2 * self.mean * self.stddev_linear_term
            + self.n_samples / float(self.n_samples - 1) * (self.mean ** 2)
        ) ** 0.5
        return self.mean, self.max, stddev


def validate(_, samples, **kwargs):
    timetagger_timestamps = []
    digital_in_timestamps = []
    converted_timestamps = []
    pps_timestamps = []
    pps_offsets = []

    if "timestamps_channel" not in kwargs.keys():
        log.error(
            "Please provide the timestamp channel to validate against with --mod_arg."
        )
        sys.exit(1)

    # This is the channel with the 100Hz signal that should be validated against the digital input.
    timestamps_channel = int(kwargs.get("timestamps_channel"))

    statistics = Statistics()

    for topic, sample in samples:
        if topic == "pando.time_tags_array":
            for macro_time in sample.payload.timestamps.channels[
                timestamps_channel
            ].macro_times:
                timetagger_timestamps.append(macro_time)
        elif topic == "pando.markers":
            if 0 in sample.payload.markers.channels.keys():
                for macro_time in sample.payload.markers.channels[0].macro_times:
                    pps_timestamps.append(macro_time)
            else:
                continue
        elif topic == "pando.pps_stats":
            pps_offsets.append(sample.payload.pps_stats.offset_ns * 1000)
        elif topic == "pandoboxd.digital_in":
            # Ignore samples not on channel 0
            if sample.payload.digital_in.device_id != 0:
                continue
            # Ignore spurious samples that occur at t=0
            if sample.header.timestamp == 0:
                continue
            # Ignore falling edges, timing is checked against the rising edge
            if sample.payload.digital_in.edge == jul_pb.DigitalEdgeType.FALLING_EDGE:
                continue
            digital_in_timestamps.append(sample.header.timestamp * 1000)
        else:
            # Ignore this sample
            continue

        # need two PPSes to do a conversion, so timestamps between the two edges can be selected
        # also need at least one pps_offset (associated with the first PPS edge)
        while (len(pps_timestamps) >= 2) and (len(pps_offsets) >= 1):
            if len(timetagger_timestamps) > 0:
                timestamp = timetagger_timestamps[0]
                # timestamp is between the two PPS edges, apply current offset
                if (timestamp >= pps_timestamps[0]) and (timestamp < pps_timestamps[1]):
                    converted_timestamps.append(timestamp - pps_offsets[0])
                # timestamp is between next set of PPS edges, done with the current leading edge + offset, so discard.
                elif timestamp >= pps_timestamps[1]:
                    pps_timestamps.pop(0)
                    pps_offsets.pop(0)
                    continue
                # removed processed timestamp (note that this also removes any timestamps before the first PPS edge)
                timetagger_timestamps.pop(0)
            else:
                # wait until more timestamps are available
                break

        diffs = []
        if len(converted_timestamps) > 0:
            # diff as many timestamps as able
            n_timestamps_to_diff = min(
                len(converted_timestamps), len(digital_in_timestamps)
            )
            for i in range(n_timestamps_to_diff):
                diffs.append(
                    (converted_timestamps[i] - digital_in_timestamps[i]) / 1000
                )  # truncate to ns
            # remove timestamps that have been diff'd
            converted_timestamps = converted_timestamps[n_timestamps_to_diff:]
            digital_in_timestamps = digital_in_timestamps[n_timestamps_to_diff:]
            # add to the statistics handler object
            statistics.add_samples(diffs)

    # Done, finalize the statistics
    mean_dis, max_dis, stddev_dis = statistics.finalize()
    log.info(
        f"\nmean: {mean_dis/1000.0:.3f}us\nmax: {max_dis/1000.0:.3f}us\nstddev: {stddev_dis/1000.0:.3f}us\ntotal samples processed: {statistics.n_samples}"
    )


mod = LimoneneModule("validation", validate)
