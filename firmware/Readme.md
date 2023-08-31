# Pando Box
This repo contains a Vivado project and a Petalinux project for the UltraZed. The goal is to create a bootable image for the Ultrazed.

## Prerequisites
1. Install Vivado 2019.2 for Linux using the Xilinx Unified Installer: https://www.xilinx.com/member/forms/download/xef.html?filename=Xilinx_Unified_2019.2_1106_2127_Lin64.bin
    2. Alternatively (or to create the docker image): https://www.xilinx.com/member/forms/download/xef.html?filename=Xilinx_Vivado_2019.2_1106_2127.tar.gz
1. Install The Update to 2019.2.1: https://www.xilinx.com/member/forms/download/xef.html?filename=Xilinx_Vivado_Vitis_Update_2019.2.1_1205_0436.tar.gz
1. Install Docker

## (Optional) Build Petalinux Docker image
Petalinux is tricky to get installed correctly, so I've created a Dockerfile (petalinux.Dockerfile) that automates the process. We'll use the resulting image to run petalinux commands.

1. Download petalinux-v2019.2-final-installer.run from here: https://www.xilinx.com/member/forms/download/xef.html?filename=petalinux-v2019.2-final-installer.run
1. Put the installer in the same directory as the dockerfile, then run
    ```
    docker build -f petalinux.Dockerfile --build-arg PETA_RUN_FILE=petalinux-v2019.2-final-installer.run -t petalinux:2019.2 .
    ```
    This will take 10-15 minutes to complete.
1. To get an interactive shell, run
    ```
    docker run -it petalinux:2019.2
    ```
    NOTE: If you want to use the pre-built container instead of creating the container yourself, invoke docker run as:
    ```
    docker run -it registry.gitlab.leaflabs.com/bldg8/pando_box-platform/petalinux:2019.2
    ```
    You are logged in as a non-root user named 'user' (with sudo access though, password is 'user'). Petalinux `settings.sh` is sourced by `bash.rc`. Exit the container by typing `exit`.

## (Optional) Build Vivado Docker image
Creating this VM requires quite a lot of disk space, so ensure you have ~150GB free before attempting this!

1. Ensure you have the vivado installer (from Prerequisites step 1.1) and the update (from Prerequisites step 2) tarballs located in the same directory as `vivado.Dockerfile` (should be root of the repository)
1. Ensure that you're logged into the docker registry
1. Generate a personal access token (PAT) on gitlab. Be sure to save it as it only appears once!
1. execute `docker login registry.gitlab.leaflabs.com -u <user>@leaflabs.com -p <PAT>`
1. For the sake of sanity, fetch the petalinux image before attempting the build `docker fetch registry.gitlab.leaflabs.com/bldg8/pando_box-platform/petalinux:2019.2`
1. Build the image: `docker build -f vivado.Dockerfile -t vivado:2019.2 .`
1. Settle in, this can take a while (Takes 3.5 hours on a core i7 laptop with 8 gigs of ram)

## (optional) Adding sources and editing the block diagram

As we use a non-project workflow, we need to script the creation and editing of the block diagram

1. New RTL sources should be located in <project_root>/hdl/src
1. To add a source file to the block diagram: `./with_docker ./edit_block_diagram.sh`
1. In the left pane, locate the new module and drag it onto the block diagram
1. Edit as necessary (run automatic configuration / drag wires / etc)
1. `File->Export->Export Block Design` to save (make sure you overwrite the correct tcl file (`/home/user/pando_box-platform/pando_box_bd.tcl`)
1. Exit Vivado

## Build bitstream & hardware platform

`./with_docker ./build_bitstream.sh`

## Build boot images

### Manual build instructions
This is mostly taken from https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1144-petalinux-tools-reference-guide.pdf

Note: Petalinux project was originally created like so:
```
petalinux-create --type project --template zynqMP --name pando_box_petalinux_project
```

1. Run the following command to get an interactive shell inside a Docker container with the vivado & petalinux project directories mounted:
    ```
    docker run -it --mount type=bind,source=$(pwd)/pando_box_vivado_project_working_copy,destination=/home/user/pando_box_vivado_project_working_copy,readonly --mount type=bind,source=$(pwd)/pando_box_petalinux_project,destination=/home/user/pando_box_petalinux_project petalinux:2019.2
    ```
1. Run
   ```
   cd pando_box_petalinux_project/
   ```
1. Run
   ```
   petalinux-config --get-hw-description ../pando_box_vivado_project_working_copy/
   ```
    Per documentation: "This launches the top system configuration menu when petalinux-config --get-hw-description runs the firsttime for the PetaLinux project or the tool detects there is a changein the system primary hardware candidates"

    Exit the menu, saving the default config.

1. Run
   ```
   petalinux-build
   ```
   This takes a while.
1. Generate BOOT.BIN:
   ```
   petalinux-package --boot --format BIN --fsbl images/linux/zynqmp_fsbl.elf --u-boot images/linux/u-boot.elf --fpga images/linux/system.bit
   ```
1. Exit the Docker container:
   ```
   exit
   ```

### CLI / Scripted instructions

1. From the root of the pando_box project: `./with_docker ./build_boot_image.sh`

## Prep and boot the ultrazed board

1. Copy `images/linux/BOOT.BIN` and `images/linux/image.ub` to the root of the SD card
1. Set ultrazed boot mode switches to ON, OFF, ON, OFF (first switch is closest to board edge)
1. Power on, and observe boot over the USB UART (network interface doesn't seem to work)
