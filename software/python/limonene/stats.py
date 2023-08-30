import json
import sys
import logging
from datetime import datetime
from collections import defaultdict

from limonene.limonene_module import LimoneneModule

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("stats")


def _print_stats(stats, msg_per_sec, total_messages):
    log.info("\n---Message statistics---")
    log.info("Total messages: {}".format(total_messages))
    # Make sure we got some messages so we don't get a divide-by-zero
    # error
    if total_messages:
        log.info("{}/Sec avg".format(sum(msg_per_sec) / len(msg_per_sec)))
        log.info("=" * 30)
        for key, val in stats.items():
            percentage = round((val / total_messages) * 100, 2)
            log.info("{}: {} ({}%)".format(key, val, percentage))
    log.info("-" * 30)


def entry(filename, events, **kwargs):
    timeframe_start = datetime.now()
    timeframe_total = 0
    grand_total = 0
    msg_per_sec = []
    stats = defaultdict(int)

    for (topic, _) in events:
        stats[topic] += 1
        grand_total += 1
        timeframe_total += 1
        if (datetime.now() - timeframe_start).total_seconds() >= 1:
            log.info("Messages/Sec: {}".format(timeframe_total))
            msg_per_sec.append(timeframe_total)
            timeframe_total = 0
            timeframe_start = datetime.now()
    _print_stats(stats, msg_per_sec, grand_total)


mod = LimoneneModule("stats", entry)
