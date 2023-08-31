import numpy as np
import pandas as pd

import glob
import h5py
import os.path
import sys


def checkTable(ds, chunk_size=10000):
    last_seq_num = -1
    record_count = 0
    naan_count = 0
    failures = set()
    while record_count < len(ds):
        df = pd.DataFrame(np.array(ds[record_count : record_count + chunk_size]))
        diffs = (
            df.sequence_number
            - pd.to_numeric(
                df.sequence_number.shift(1).fillna(last_seq_num), downcast="integer"
            )
        ).unique()
        naan_count += df.isnull().sum().sum()
        if len(diffs) != 1:
            failures.add(("Sequence Number Check"))
        record_count += len(df)
        last_seq_num = df.iloc[-1, 1]
    if naan_count != 0:
        failures.add("Naan Check")
    return failures


def checkFile(filename, data):
    results = []
    with h5py.File(filename, "r") as h5_file:
        entry = {}
        parts = os.path.split(filename)
        entry["filename"] = parts[1]
        entry["test_dir"] = os.path.split(parts[0])[1]
        table_names = [name for name in h5_file.keys() if "_headers" in name]
        for n in table_names:
            row = dict(entry)
            row["table_name"] = n
            failures = checkTable(h5_file[n])
            row["failures"] = str(list(failures))
            row["pass_fail"] = "FAIL" if failures else "PASS"
            results.append(row)
    df = pd.DataFrame(results)
    return data.append(df, ignore_index=True, sort=True)


def main(path_to_files):
    filenames = glob.glob(path_to_files + "/**/*.h5")
    df = pd.DataFrame()
    for filename in filenames:
        df = checkFile(filename, df)
    failure_count = len(df[df.pass_fail == "FAIL"])
    print("Test Results:", failure_count, "failure(s)")
    print(df)
    df.to_csv("results.csv")


if __name__ == "__main__":

    if len(sys.argv) > 1:
        path_to_files = sys.argv[1]
    else:
        print("Usage: python check_analyzed_files.py <path_to_files>")
        sys.exit(2)

    main(path_to_files)
