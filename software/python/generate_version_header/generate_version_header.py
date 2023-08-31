#!/usr/bin/env python3

import subprocess
import datetime
import argparse


class InfoGrabberException(Exception):
    pass


class InfoGrabber:
    def run_cmd(self, cmd):
        try:
            p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            p.check_returncode()
        except (FileNotFoundError, subprocess.CalledProcessError):
            raise InfoGrabberException
        return p.stdout.decode("utf-8").strip()

    def get_date(self):
        return datetime.datetime.now().strftime("%Y_%m_%d")

    def get_exact_tag(self):
        return None

    def get_short_hash(selfs):
        return None

    def is_dirty(self):
        return None


class GitInfoGrabber(InfoGrabber):
    def __init__(self):
        # Check if this is a git repo by executing git
        self.run_cmd(["git", "rev-parse", "--is-inside-work-tree"])

    def git_describe(self, exact=False, dirty=False):
        args = ["git", "describe", "--tags", "--abbrev=0"]

        if exact:
            args.append("--exact-match")

        if dirty:
            args.append("--dirty")

        try:
            tag = self.run_cmd(args)
            return tag
        except InfoGrabberException:
            return None

    def get_exact_tag(self):
        return self.git_describe(exact=True)

    def is_dirty(self):
        d = self.git_describe(dirty=True)
        if d:
            return True if "-dirty" in d else False
        else:
            return None

    def get_short_hash(self):
        try:
            short_hash = self.run_cmd(["git", "rev-parse", "--short", "HEAD"])
            return short_hash
        except InfoGrabberException:
            return None


class HgInfoGrabber(InfoGrabber):
    # Not implemented yet!
    def __init__(self):
        raise InfoGrabberException

    def get_exact_tag(self):
        return None

    def get_short_hash(selfs):
        return None

    def is_dirty(self):
        return None


def main():
    parser = argparse.ArgumentParser(description="Generate build version strings")
    parser.add_argument(
        "--header",
        dest="header",
        action="store_true",
        help="Generate a define appropriate for use in a c/c++ header",
    )

    parser.add_argument(
        "--define-name",
        dest="define_name",
        help="Name of the define to use when emitting as a header",
        default="VERSION_STRING",
    )

    args = parser.parse_args()

    info_grabber = None
    try:
        # Default to using Git
        info_grabber = GitInfoGrabber()
    except InfoGrabberException:
        # Could not find Git... attempt to use hg
        try:
            info_grabber = HgInfoGrabber()
        except InfoGrabberException:
            # Could not find hg... fallback to the default
            info_grabber = InfoGrabber()

    short_hash = info_grabber.get_short_hash()
    tag = info_grabber.get_exact_tag()
    dirty = info_grabber.is_dirty()
    build_date = info_grabber.get_date()

    version = "UNDEFINED"

    if tag:
        version = tag
    elif short_hash:
        version = "build_" + build_date + "-" + short_hash
    else:
        version = "build_" + build_date + "-UNKNOWN"

    if dirty:
        version += "-dirty"

    if args.header:
        print('#define {} "{}"'.format(args.define_name, version))
    else:
        print(version)


if __name__ == "__main__":
    main()
