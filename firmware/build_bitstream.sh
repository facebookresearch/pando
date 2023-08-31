#!/bin/bash

# Copyright (c) Meta Platforms, Inc. and affiliates.

rm -rf ./.Xil
vivado -mode batch -source vivado_non_proj_build.tcl
