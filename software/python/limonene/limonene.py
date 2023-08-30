#!/usr/bin/env python3
"""limonene: A simple pandoboxd control interface (and sniffer)

"""
import argparse
from datetime import datetime
import sys
import logging
import importlib
import collections

from limonene.pandoboxd_controller import PandoboxdController
from limonene.hdf5_controller import HDF5Controller
from limonene.pando_controller import PandoController

# Add any additional modules here
from limonene import vcd_logger
from limonene import json_logger
from limonene import stats
from limonene import online_validator

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("core")


MODULES = {
    x.mod.name: x.mod for x in (vcd_logger, json_logger, stats, online_validator)
}


def _get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("controller_args", default="")
    parser.add_argument(
        "--msg_filter",
        "-f",
        type=str,
        action="append",
        help="Message filter. Accepts wildcard (*)",
    )

    parser.add_argument(
        "--module",
        "-m",
        type=str,
        action="append",
        help=f"Processing modules to load. Current options are: {list(MODULES.keys())}",
    )

    parser.add_argument(
        "--mod_arg",
        "-a",
        type=str,
        action="append",
        help="Module arguments in the form <mod>:<key>:<val> e.g. validation:channel:2",
    )

    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        default=False,
        help="Log.Info all messages to screen",
    )

    parser.add_argument(
        "--duration",
        "-d",
        default=None,
        type=int,
        help="Length of time to run, in milliseconds",
    )

    parser.add_argument(
        "--output", "-o", default=None, type=str, help="Override output filename",
    )

    controller = parser.add_mutually_exclusive_group(required=True)
    controller.add_argument(
        "--hdf5",
        action="store_true",
        help="Load data from saved hdf5 files rather than live streaming",
    )
    controller.add_argument(
        "--pandoboxd",
        action="store_true",
        help="live-stream data from the pandoboxd application",
    )
    controller.add_argument(
        "--pando",
        action="store_true",
        help="live-stream data from the pando application",
    )

    return parser.parse_args()


def _get_run_condition(args):
    """Given the command line argument namespace, returns a closure that
    returns a boolean indicating whether or not sampling should
    continue.

    The default is to simply return True and rely on a CTRL-C to end the stream.

    In the case of `-d/--duration` being supplied, a time check is
    used to see if the specified milliseconds have elapsed.

    """
    if args.duration:
        test_start = datetime.now()
        return (
            lambda: (datetime.now() - test_start).total_seconds() * 1000 < args.duration
        )

    return lambda: True


def _show_start_instructions(args):
    if args.duration:
        log.info(
            (
                "Starting stream. Test will run for {} milliseconds. "
                "Ctrl-C exits early. Press any key to continue"
            ).format(args.duration)
        )
    else:
        log.info("Starting stream. CTRL-C exits. Press any key to continue")


def _parse_mod_args(args, modules):
    result = collections.defaultdict(dict)
    if args.mod_arg is not None:
        for arg in args.mod_arg:
            try:
                mod, key, val = arg.split(":")
            except:
                log.error(f"Unable to parse module argument: {arg}")
                sys.exit(1)

            if mod not in modules:
                log.error(f"Module {mod} is an unknown module")
                sys.exit(1)

            result[mod][key] = val
    return result


def main():
    args = _get_args()

    if args.hdf5:
        ctrl = HDF5Controller(hdf5_files=args.controller_args)
    elif args.pandoboxd:
        ctrl = PandoboxdController(addr=args.controller_args, filters=args.msg_filter)
    elif args.pando:
        ctrl = PandoController(addr=args.controller_args, filters=args.msg_filter)

    if args.pandoboxd:
        log.info("Pinging Pandoboxd")
        if not ctrl.ping():
            log.info("Pandoboxd doesn't appear to be running!")
            sys.exit(1)
    try:
        run_start = datetime.now()
        if args.pandoboxd:
            _show_start_instructions(args)
            # Wait for the user to press a key and begin the stream
            input()
            if not ctrl.start():
                log.info("Error starting transaction")
                sys.exit(1)

        grand_total = 0

        should_run = _get_run_condition(args)

        if args.output is None:
            filename = "limonene-{}".format(run_start.strftime("%y_%m_%d_%H%M%S"))
        else:
            filename = args.output

        active_modules = []
        mod_args = _parse_mod_args(args, MODULES)

        for mod_name in set(args.module):
            if mod_name not in MODULES:
                log.warn(f"{mod_name} is not a known module")
                continue
            mod = MODULES[mod_name]
            active_modules.append(mod.launch(filename, **mod_args[mod_name]))
        while should_run():
            # TODO: Figure out a clean way to flush out the samples
            # that have collected in the sorting filter
            topic, sample = ctrl.read_sample()
            if sample is None:
                log.info("Got an empty response from the controller")
                return
            grand_total += 1

            if args.verbose:
                log.info("[{}]: ".format(grand_total), topic, sample)

            for (mod, queue) in active_modules:
                queue.put((topic, sample))
    except KeyboardInterrupt:
        pass
    finally:
        log.info("Stopping stream")
        log.info(ctrl.stop())
        for (mod, queue) in active_modules:
            queue.put(None)
            mod.join()


if __name__ == "__main__":
    main()
