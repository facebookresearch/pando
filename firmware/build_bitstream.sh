#!/bin/bash

rm -rf ./.Xil
vivado -mode batch -source vivado_non_proj_build.tcl
