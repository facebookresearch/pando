#!/usr/bin/env python3

import argparse
import h5py
import sys
from pathlib import Path


class DefaultHelpParser(argparse.ArgumentParser):
    """ArgumentParser, but prints complete help on any error."""

    def error(self, message):
        print("error: {}".format(message), file=sys.stderr)
        self.print_help()
        sys.exit(2)


def parse_args():
    parser = DefaultHelpParser(
        description="Split an HDF5 file produced by Pando containing camera images into multiple chunks"
    )
    parser.add_argument(
        "in_file", type=Path, help="The source HDF5 file produced by Pando"
    )
    parser.add_argument(
        "--n_images",
        "-n",
        type=int,
        default=90,
        help="number of images to store in each output file",
    )
    parser.add_argument(
        "--out_dir",
        "-o",
        type=Path,
        help="output directory to create output files in. Will be created if it does not already exist. Defaults to a new directory in the same directory as in_file)",
    )

    return parser.parse_args()


def main():
    args = parse_args()

    if args.out_dir == None:
        args.out_dir = args.in_file.parent / "{}_split".format(args.in_file.stem)

    args.out_dir.mkdir(parents=True, exist_ok=True)

    with h5py.File(args.in_file) as f_i:
        images_dset = f_i["Images"]
        metadata_dset = f_i["Metadata"]

        # Same number of rows in both dsets
        n_rows = images_dset.shape[0]
        assert metadata_dset.shape[0] == n_rows

        for i in range(0, n_rows, args.n_images):
            # output file name
            chunk_index = i // args.n_images
            out_file = args.out_dir / "{}_{:03d}.h5".format(
                args.in_file.stem, chunk_index
            )

            # Last file may contain fewer than n_images
            n_to_copy = min(args.n_images, n_rows - i)

            with h5py.File(out_file, "x") as f_o:
                # Copy metadata
                f_o.create_dataset("Metadata", data=metadata_dset[i : i + n_to_copy])

                # Copy images (one by one to limit memory use)
                f_o.create_dataset("Images", n_to_copy, dtype=images_dset.dtype)
                for j in range(n_to_copy):
                    f_o["Images"][j] = images_dset[i + j]


if __name__ == "__main__":
    main()
