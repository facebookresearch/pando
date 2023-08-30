import os
import shutil
import sys

from zipfile import ZipFile


def append_files_to_zip(zipfile, files):
    print("append_to_zip called with:", zipfile, str(files))
    with ZipFile(zipfile, "a") as zf:
        for f in files:
            zip_path = calc_zip_path(f["file"], f["zpath"])
            print("Writing", f["file"], "to", zip_path, "in zipfile", zipfile)
            zf.write(f["file"], zip_path)


def calc_zip_path(opath, zpath):
    basename = os.path.basename(opath)
    tmp = zpath
    return zpath.replace("$BASENAME", basename)


def parse_args(args):
    args = list(args)
    parsed = {}
    while len(args) > 0:
        arg = args.pop(0)
        if arg == "-z":
            parsed["zipfile"] = args.pop(0)
        elif arg == "-a":
            appends = parsed.get("appends", [])
            parsed["appends"] = appends + [{"file": args.pop(0), "zpath": args.pop(0)}]
        elif arg == "-o":
            parsed["outfile"] = args.pop(0)
    # return parsed if all args have been given
    if "zipfile" in parsed and "appends" in parsed and "outfile" in parsed:
        return parsed
    # else we didn't get all the arguments specified
    raise Exception("Did not receive all arguments: " + str(parsed))


if __name__ == "__main__":
    parsed_args = None
    print("Hello from add_to_zip.py")
    try:
        parsed_args = parse_args(sys.argv)
    except Exception as e:
        usage()
        raise e

    shutil.copyfile(parsed_args["zipfile"], parsed_args["outfile"])
    append_files_to_zip(parsed_args["outfile"], parsed_args["appends"])
