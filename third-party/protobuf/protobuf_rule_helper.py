#!/usr/bin/env python
# Copyright 2004-present Facebook. All Rights Reserved.

# Cross-platform `cp` and `mkdir` implementations

from __future__ import print_function

import sys
import shutil
import os

def split_path(path):
    assert "\\" not in path
    assert not path.startswith("/")
    return path.split("/") if path else []

def mkdir_in(dir, path):
    assert os.path.isdir(dir)
    components = split_path(path)
    for component in components:
        next_dir = dir + "/" + component
        if not os.path.isdir(next_dir):
            os.mkdir(next_dir)
        dir = next_dir
    return dir

def prepare_our_protos(srcdir, tmp, namespace, protos):
    copy_to_dir = mkdir_in(tmp, namespace)
    for proto in protos:
        if "/" in proto:
            proto_dir, proto_name = proto.rsplit("/", 1)
            mkdir_in(copy_to_dir, proto_dir)
        shutil.copy(srcdir + "/" + proto, copy_to_dir + "/" + proto)

def copy_tree_content_rec(src, dst):
    assert os.path.isdir(src)
    assert os.path.isdir(dst)
    for name in os.listdir(src):
        src1 = src + "/" + name
        dst1 = dst + "/" + name
        if os.path.isdir(src1):
            if not os.path.isdir(dst1):
                os.mkdir(dst1)
            copy_tree_content_rec(src1, dst1)
        else:
            shutil.copy(src1, dst1)

def prepare_dep_protos(tmp, dep_namespace, dep_location):
    copy_to_dir = mkdir_in(tmp, dep_namespace)
    copy_tree_content_rec(dep_location, copy_to_dir)

# Prepare $TMP directory to run protoc
def prepare(args):

    # take next arg from args
    def next():
        r = args[0]
        args[:1] = []
        return r
    # take and check next keyword
    def next_k(k):
        assert args[0] == k
        args[:1] = []
    # take next named kv
    def next_kv(k):
        next_k(k)
        return next()

    tmp = next_kv("TMP")
    srcdir = next_kv("SRCDIR")
    namespace = next_kv("NAMESPACE")
    assert namespace.startswith("N=")
    namespace = namespace[2:]

    next_k("PROTOS")
    protos = []
    while args[0] != "PROTO_DEPS":
        protos += [next()]

    next_k("PROTO_DEPS")
    proto_deps = []
    while args:
        dep_namespace = next()
        assert dep_namespace.startswith("N=")
        dep_namespace = dep_namespace[2:]
        dep_location = next()
        proto_deps += [(dep_namespace, dep_location)]

    prepare_our_protos(srcdir=srcdir, tmp=tmp, namespace=namespace, protos=protos)

    for dep_namespace, dep_location in proto_deps:
        prepare_dep_protos(tmp=tmp, dep_namespace=dep_namespace, dep_location=dep_location)


# cp src... dest
def cp(args):
    assert len(args) >= 2, args
    srcs = args[:-1]
    dst = args[-1]
    for src in srcs:
        if os.path.isdir(src):
            shutil.copytree(src, dst)
        else:
            shutil.copy(src, dst)


def main():
    args = sys.argv[1:]
    assert args
    if args[0] == "cp":
        return cp(args[1:])
    elif args[0] == "prepare":
        return prepare(args[1:])
    else:
        raise Exception("unknown command: {}".format(args[0]))

if __name__ == "__main__":
    main()

# vim: set ts=4 sw=4 et:
