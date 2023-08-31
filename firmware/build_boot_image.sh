#!/bin/bash

# Copyright (c) Meta Platforms, Inc. and affiliates.

set -e

pushd pando_box_petalinux_project

echo ^e^e | petalinux-config -v --get-hw-description ../vivado_output
petalinux-build
petalinux-package --boot --format BIN --fsbl images/linux/zynqmp_fsbl.elf --u-boot images/linux/u-boot.elf --fpga images/linux/system.bit --force
popd
