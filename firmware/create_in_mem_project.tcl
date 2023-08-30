# Create a project IN MEMORY (prevents the creation of a bunch of unnecessary files on disk
create_project -part xczu3eg-sfva625-1-i -force -in_memory

set origin_dir "."

# Set project properties
set obj [current_project]
set_property -name "board_part_repo_paths" -value "[file normalize "$origin_dir/bsp"]" -objects $obj
set_property -name "board_part" -value "em.avnet.com:ultrazed_eg_iocc_production:part0:1.1" -objects $obj
set_property -name "default_lib" -value "xil_defaultlib" -objects $obj
set_property -name "enable_vhdl_2008" -value "1" -objects $obj
set_property -name "mem.enable_memory_map_generation" -value "1" -objects $obj
set_property -name "platform.board_id" -value "ultrazed_eg_iocc_production" -objects $obj
set_property -name "platform.default_output_type" -value "sd_card" -objects $obj
set_property -name "platform.design_intent.datacenter" -value "false" -objects $obj
set_property -name "platform.design_intent.embedded" -value "true" -objects $obj
set_property -name "platform.design_intent.external_host" -value "false" -objects $obj
set_property -name "platform.design_intent.server_managed" -value "false" -objects $obj
set_property -name "platform.name" -value "pando_box_sd" -objects $obj
set_property source_mgmt_mode All [current_project]

# Load our source files and design constraints
read_verilog [glob hdl/src/*.v ]
read_xdc [ glob hdl/constraints/*.xdc ]

