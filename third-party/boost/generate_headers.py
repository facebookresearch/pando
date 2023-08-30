#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import json
import sys
from subprocess import Popen, PIPE

TARGET_PREFIX = "//third-party/boost"
RAW_HEADERS_ATTRIBUTE = "raw_headers"
NAME_ATTRIBUTE = "name"

def output_target(target, target_json):
    target_name = target_json[NAME_ATTRIBUTE]

    values = target_json.get(RAW_HEADERS_ATTRIBUTE)
    if values:
        bzl = f"{target_name}_headers.bzl"

        print(f"Writing headers for target {target} into {bzl}")

        with open(bzl, "w") as file:

            file.write("# To regenerate: python generate_headers.py\n")
            # This python file is not generated so insert some symbols between `@` and `generated`
            file.write("# @" + "generated\n")

            headers = []
            for path in values:
                headers.append("\"%s\"," % path)

            file.write("\n%s_RAW_HEADERS_GENERATED = [\n    " % (target_name.upper()))
            file.write("\n    ".join(headers))

            file.write("\n]\n")
    else:
        print(f"Skipping writing headers for {target} because there are none")

command = " ".join([
    "buck",
    "query",
    "--config",
    "xplat.boost_use_glob=True",
    TARGET_PREFIX + "/...",
    "--output-attributes",
    "%s %s" % (RAW_HEADERS_ATTRIBUTE, NAME_ATTRIBUTE),
])
print(f"Invoking buck to fetch headers: {command}")
proc = Popen(command, stdout = PIPE, stderr = PIPE, shell = True, universal_newlines = True)
out, err = proc.communicate()
if proc.returncode != 0:
    sys.stderr.write(err)
    sys.exit(proc.returncode)

# Parse result from buck query.
query = json.loads(out)

for target, target_json in query.items():
    output_target(target, target_json)
