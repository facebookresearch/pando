#!/bin/bash

docker run --init -it --network host --env DISPLAY -v $HOME/.Xauthority:/home/user/.Xauthority -v $(pwd):/home/user/pando_box-platform registry.gitlab.leaflabs.com/bldg8/pando_box-platform/vivado:2019.2
