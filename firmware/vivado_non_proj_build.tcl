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
synth_design -top pando_box_bd_wrapper -part xczu3eg-sfva625-1-i -flatten rebuilt
write_checkpoint -force $outputDir/post_synth
report_timing_summary -file $outputDir/post_synth_timing_summary.rpt
report_power -file $outputDir/post_synth_power.rpt

# Run placement and logic optimzation, report utilization and timing estimates, write checkpoint design
opt_design
place_design
phys_opt_design
write_checkpoint -force $outputDir/post_place
report_timing_summary -file $outputDir/post_place_timing_summary.rpt

# Run router, report actual utilization and timing, write checkpoint design, run drc, write verilog and xdc out
route_design
write_checkpoint -force $outputDir/post_route
report_timing_summary -file $outputDir/post_route_timing_summary.rpt
report_timing -sort_by group -max_paths 100 -path_type summary -file $outputDir/post_route_timing.rpt
report_clock_utilization -file $outputDir/clock_util.rpt
report_utilization -file $outputDir/post_route_util.rpt
report_power -file $outputDir/post_route_power.rpt
report_drc -file $outputDir/post_imp_drc.rpt
write_verilog -force $outputDir/bft_impl_netlist.v
write_xdc -no_fixed_only -force $outputDir/bft_impl.xdc

# Generate a bitstream and hardware platform for petalinux
write_bitstream -force -bin_file $outputDir/pando_box_bd
write_hw_platform -fixed -force -include_bit $outputDir/pando_box_bd.xsa
