#!/usr/bin/env python3
# Copyright 2004-present Facebook. All Rights Reserved.

import sys
import shutil
import os

### A simple helper script to copy the relevant files from a downloaded Boost archive

if __name__ == "__main__":
    src_dir = sys.argv[1] # points to downloaded boost
    dst_dir = sys.argv[2] # points to target directory

    print("Copying {}".format(src_dir + "/boost"))
    shutil.copytree(src_dir + "/boost", dst_dir + "/boost")

    for filename in os.listdir(src_dir + "/libs"):
        full_src_path = src_dir + "/libs/" + filename + "/src"
        if (os.path.isdir(full_src_path)):
            print("Copying {}/ to libs".format(filename))
            full_dst_path = dst_dir + "/libs/" + filename + "/src"
            shutil.copytree(full_src_path, full_dst_path)
