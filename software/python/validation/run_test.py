import glob
import os
import os.path
import shutil
import subprocess
import sys

DeviceSpecificArgs = {
    "pf32": [
        {
            "name": "firmware",
            "option": "-f",
            "description": "Path to pf32 firmware file",
            "type": "path",
        }
    ]
}


def cleanTestRoot(testroot):
    to_be_deleted = glob.glob(testroot + os.sep + "*.h5") + glob.glob(
        testroot + os.sep + "pando_logs" + os.sep + "*.log"
    )
    print("to_be-deleted:", to_be_deleted)
    for f in to_be_deleted:
        print("Deleting:", f)
        os.remove(f)


def getDeviceSpecificArgs(test):
    args = []
    device_type = test.split("-")[0]
    for a in DeviceSpecificArgs.get(device_type, []):
        value = input(
            "Enter value for '%s' (or hit ENTER to not override the default in config.yaml):"
            % a["description"]
        )
        value = value.strip()
        if value != "":
            args.append(a["option"])
            if a["type"] == "path":
                # convert to absolute path
                value = os.path.realpath(value)
            args.append(value)
    return args


def runTest(testroot, test, pex):
    print("Pex Under Test:", pex)
    print("Test:", test)
    print("Test Root:", testroot)
    dargs = getDeviceSpecificArgs(test)
    cleanTestRoot(testroot)
    # store old dir
    old_dir = os.getcwd()
    # cd to test dir
    testdir = testroot + os.sep + test
    config_file = testdir + os.sep + "config.yaml"
    os.chdir(testdir)
    # run with config.yaml
    completed = subprocess.run(
        ["python3", pex, "-c", config_file, "-d", testdir, "-a", "3600"] + dargs
    )
    # cd back to old dir
    os.chdir(old_dir)
    # run checks on h5 file


def getTestChoice(tests):
    while True:
        for t in tests:
            print("\t", t)
        choice = input("Enter name of test to run:")
        if choice == "quit":
            return None
        if choice in tests:
            return choice
        print('Unrecognized selection "' + choice + '".')


def parseArgs(args):
    parsed = {}
    parsed["pex"] = "../../../buck-out/gen/software/python/gui/pando.pex"
    for i in range(0, len(args), 2):
        if args[i] == "-t":
            parsed["test"] = args[i + 1]
        elif args[i] == "-r":
            parsed["testroot"] = args[i + 1]
        elif args[i] == "-p":
            parsed["pex"] = args[i + 1]
        else:
            raise Exception("Unrecognized argument.")
    return parsed


def getTestsInRoot(testroot):
    return [t.split("\\")[-1] for t in glob.glob(testroot + "\\*")]


def usage():
    print("USAGE: python3 run_test.py [-t <test-name>] [-p <pex-file>] -r <test-root>")


if __name__ == "__main__":

    if len(sys.argv) <= 1:
        usage()
        sys.exit(2)

    p = parseArgs(sys.argv[1:])

    if "testroot" in p.keys():
        testroot = os.path.realpath(p["testroot"])
        pex = os.path.realpath(p["pex"])
        if not "test" in p.keys():
            # get list of tests in testroot
            tests = getTestsInRoot(testroot)
            test = ""
            while test is not None and test != "quit":
                if test:
                    runTest(testroot, test, pex)
                test = getTestChoice(tests)
        else:
            runTest(testroot, p["test"], pex)
