# Inspired / based on https://forums.xilinx.com/t5/Vivado-TCL-Community/Block-Design-in-non-project-mode/td-p/484200
# Define output directory area.

set outputDir ./vivado_output
file mkdir $outputDir

source create_in_mem_project.tcl


# FIXME: Do we need this?
#set ip repository path
set_property IP_REPO_PATHS ./ip_rep [current_fileset ]
update_ip_catalog

# create the BD-Design
source pando_box_bd.tcl

# Make a top level design wrapper
# TODO: Should we just maintain this manually?
make_wrapper -files [get_files pando_box_bd.bd] -top
read_verilog [glob .srcs/sources_1/bd/pando_box_bd/hdl/*.v ]

# Generate targets for our IP
generate_target all [get_files pando_box_bd.bd]

synth_ip [get_ips pando_box_bd.bd]

# Run synthesis, report utilization and timing estimates, write checkpoint design
synth_design -top pando_box_bd_wrapper -part xczu3eg-sfva625-1-i -flatten rebuilt -rtl
