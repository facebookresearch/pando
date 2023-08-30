import os
import shutil
import sys


def createDirectories(src, dst):
    shutil.copytree(src, dst)


def parseArgs(args):
    parsed = {}
    parsed["src"] = "./templates"
    i = 0
    while i < len(args):
        if args[i] == "-d":
            parsed["dst"] = args[i + 1]
            i += 1
        i += 1
    return parsed


def usage():
    print("USAGE: python3 init.py -d <destination-directory>")


if __name__ == "__main__":

    if len(sys.argv) <= 1:
        usage()
        sys.exit(2)

    parsed = parseArgs(sys.argv[1:])
    createDirectories(parsed["src"], parsed["dst"])
