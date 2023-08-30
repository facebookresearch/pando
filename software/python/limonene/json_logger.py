import json
import sys
import logging

from limonene.limonene_module import LimoneneModule

from google.protobuf.json_format import MessageToDict

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("json")


def _write_samples(ofile, samples):
    """Writes all collected samples to a JSON file

    """
    ofile.write("[\n")
    for topic, sample in samples:
        # NOTE: We use `MessageToDict` and then json.dumps rather
        # than `MessageToJson` as this way we get the message
        # classification (IMU/HRM/etc) which isn't present in
        # `MessageToJSON`
        s_dict = MessageToDict(sample)
        ofile.write(json.dumps(s_dict))
        ofile.write(",\n")
    # Remove the trailing comma since it isn't valid JSON
    ofile.seek(ofile.tell() - 2)
    ofile.write("\n]\n")


def entry(filename, events, **kwargs):
    with open(f"{filename}.json", "w") as ofile:
        for event in events:
            _write_samples(ofile, events)
    log.info(f"JSON logfile written to {filename}.json")


mod = LimoneneModule("json", entry)
