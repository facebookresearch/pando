reset_runs impl_1
launch_runs impl_1 -to_step write_bitstream -verbose
wait_on_run impl_1 -timeout 60 -verbose

set_property platform.name                           pando_box_sd  [current_project]
set_property platform.design_intent.embedded         true      [current_project]
set_property platform.design_intent.server_managed   false     [current_project]
set_property platform.design_intent.external_host    false     [current_project]
set_property platform.design_intent.datacenter       false     [current_project]
set_property platform.default_output_type            "sd_card" [current_project]

write_hw_platform -fixed -force -include_bit pando_box_vivado_project_working_copy/pando_box_bd.xsa
