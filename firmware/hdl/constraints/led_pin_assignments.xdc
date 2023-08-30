# User LEDs
set_property PACKAGE_PIN R7  [get_ports {digital_inputs_0_fault}];  # JX1_HP_DP_25_P
set_property PACKAGE_PIN T5  [get_ports {imu_0_fault}];  # JX1_HP_DP_24_P
set_property PACKAGE_PIN T7  [get_ports {adc_0_fault}];  # JX1_HP_DP_25_N
set_property PACKAGE_PIN T4  [get_ports {adc_1_fault}];  # JX1_HP_DP_24_N
set_property PACKAGE_PIN T3  [get_ports {adc_2_fault}];  # JX1_HP_DP_27_P
set_property PACKAGE_PIN U2  [get_ports {adc_3_fault}];  # JX1_HP_DP_27_N
set_property PACKAGE_PIN U6  [get_ports {traffic_generator_0_fault}];  # JX1_HP_DP_26_P
# set_property PACKAGE_PIN W7   [get_ports {PLPMOD6_D7}];  # JX1_HP_DP_23_N

set_property IOSTANDARD LVCMOS18 [get_ports {digital_inputs_0_fault}]
set_property IOSTANDARD LVCMOS18 [get_ports {imu_0_fault}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_0_fault}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_1_fault}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_2_fault}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_3_fault}]
set_property IOSTANDARD LVCMOS18 [get_ports {traffic_generator_0_fault}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD6_D7}]
