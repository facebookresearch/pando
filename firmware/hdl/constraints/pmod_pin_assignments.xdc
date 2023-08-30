# JX1 JA PMOD  (Digital Inputs)
set_property PACKAGE_PIN AB8  [get_ports {digital_input_0}];  # JX1_HP_DP_00_P
set_property PACKAGE_PIN AB7  [get_ports {digital_input_1}];  # JX1_HP_DP_00_N
set_property PACKAGE_PIN Y7   [get_ports {digital_input_2}];  # JX1_HP_DP_01_P
set_property PACKAGE_PIN AA7  [get_ports {digital_input_3}];  # JX1_HP_DP_01_N
set_property PACKAGE_PIN AA9  [get_ports {digital_input_4}];  # JX1_HP_DP_02_P
set_property PACKAGE_PIN AA8  [get_ports {digital_input_5}];  # JX1_HP_DP_02_N
set_property PACKAGE_PIN AC9  [get_ports {digital_input_6}];  # JX1_HP_DP_03_P
set_property PACKAGE_PIN AC8  [get_ports {digital_input_7}];  # JX1_HP_DP_03_N

set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_0}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_1}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_2}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_3}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_4}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_5}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_6}]
set_property IOSTANDARD LVCMOS18 [get_ports {digital_input_7}]

# JX1 JB PMOD (Dual PPS)
set_property PACKAGE_PIN AD6  [get_ports {PPS_0}];  # JX1_HP_DP_04_P
set_property PACKAGE_PIN AD5  [get_ports {PPS_1}];  # JX1_HP_DP_04_N
set_property PACKAGE_PIN AD9  [get_ports {clk_10M}];  # JX1_HP_DP_05_P
# set_property PACKAGE_PIN AE9  [get_ports {PLPMOD2_D3}];  # JX1_HP_DP_05_N
# set_property PACKAGE_PIN AE7  [get_ports {PLPMOD2_D4}];  # JX1_HP_DP_06_P
# set_property PACKAGE_PIN AE6  [get_ports {PLPMOD2_D5}];  # JX1_HP_DP_06_N
# set_property PACKAGE_PIN AD8  [get_ports {PLPMOD2_D6}];  # JX1_HP_DP_07_P
# set_property PACKAGE_PIN AD7  [get_ports {PLPMOD2_D7}];  # JX1_HP_DP_07_N

set_property IOSTANDARD LVCMOS18 [get_ports {PPS_0}]
set_property IOSTANDARD LVCMOS18 [get_ports {PPS_1}]
set_property IOSTANDARD LVCMOS18 [get_ports {clk_10M}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD2_D3}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD2_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD2_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD2_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD2_D7}]

# JX1 JC PMOD (ADC 0)
set_property PACKAGE_PIN AA4  [get_ports {adc_0_spi_cs}];  # JX1_HP_DP_08_P
# set_property PACKAGE_PIN AA3  [get_ports {PLPMOD3_D1}];  # JX1_HP_DP_08_N
set_property PACKAGE_PIN W1   [get_ports {adc_0_spi_miso}];  # JX1_HP_DP_09_P
set_property PACKAGE_PIN Y1   [get_ports {adc_0_spi_clk}];  # JX1_HP_DP_09_N
# set_property PACKAGE_PIN W4   [get_ports {PLPMOD3_D4}];  # JX1_HP_DP_10_P
# set_property PACKAGE_PIN Y4   [get_ports {PLPMOD3_D5}];  # JX1_HP_DP_10_N
# set_property PACKAGE_PIN AB2  [get_ports {PLPMOD3_D6}];  # JX1_HP_DP_11_P
# set_property PACKAGE_PIN AB1  [get_ports {PLPMOD3_D7}];  # JX1_HP_DP_11_N

set_property IOSTANDARD LVCMOS18 [get_ports {adc_0_spi_cs}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD3_D1}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_0_spi_miso}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_0_spi_clk}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD3_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD3_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD3_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD3_D7}]

# JX1 JD PMOD (ADC 1)
set_property PACKAGE_PIN W3   [get_ports {adc_1_spi_cs}];  # JX1_HP_DP_12_P
# set_property PACKAGE_PIN W2   [get_ports {PLPMOD4_D1}];  # JX1_HP_DP_12_N
set_property PACKAGE_PIN AB6  [get_ports {adc_1_spi_miso}];  # JX1_HP_DP_13_P
set_property PACKAGE_PIN AC6  [get_ports {adc_1_spi_clk}];  # JX1_HP_DP_13_N
# set_property PACKAGE_PIN Y2   [get_ports {PLPMOD4_D4}];  # JX1_HP_DP_14_P
# set_property PACKAGE_PIN AA2  [get_ports {PLPMOD4_D5}];  # JX1_HP_DP_14_N
# set_property PACKAGE_PIN AE4  [get_ports {PLPMOD4_D6}];  # JX1_HP_DP_15_P
# set_property PACKAGE_PIN AE3  [get_ports {PLPMOD4_D7}];  # JX1_HP_DP_15_N

set_property IOSTANDARD LVCMOS18 [get_ports {adc_1_spi_cs}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD4_D1}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_1_spi_miso}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_1_spi_clk}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD4_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD4_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD4_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD4_D7}]

# JX1 JE PMOD (ADC 2)
set_property PACKAGE_PIN AD1  [get_ports {adc_2_spi_cs}];  # JX1_HP_DP_16_P
# set_property PACKAGE_PIN AE1  [get_ports {PLPMOD5_D1}];  # JX1_HP_DP_16_N
set_property PACKAGE_PIN AD4  [get_ports {adc_2_spi_miso}];  # JX1_HP_DP_17_P
set_property PACKAGE_PIN AD3  [get_ports {adc_2_spi_clk}];  # JX1_HP_DP_17_N
# set_property PACKAGE_PIN AB5  [get_ports {PLPMOD5_D4}];  # JX1_HP_DP_18_GC_P
# set_property PACKAGE_PIN AC4  [get_ports {PLPMOD5_D5}];  # JX1_HP_DP_18_GC_N
# set_property PACKAGE_PIN AB3  [get_ports {PLPMOD5_D6}];  # JX1_HP_DP_19_GC_P
# set_property PACKAGE_PIN AC3  [get_ports {PLPMOD5_D7}];  # JX1_HP_DP_19_GC_N

set_property IOSTANDARD LVCMOS18 [get_ports {adc_2_spi_cs}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD5_D1}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_2_spi_miso}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_2_spi_clk}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD5_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD5_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD5_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD5_D7}]


# JX1 JF PMOD (ADC 3)
set_property PACKAGE_PIN W6   [get_ports {adc_3_spi_cs}];  # JX1_HP_DP_20_GC_P
# set_property PACKAGE_PIN Y6   [get_ports {PLPMOD6_D1}];  # JX1_HP_DP_20_GC_N
set_property PACKAGE_PIN Y5   [get_ports {adc_3_spi_miso}];  # JX1_HP_DP_21_GC_P
set_property PACKAGE_PIN AA5  [get_ports {adc_3_spi_clk}];  # JX1_HP_DP_21_GC_N
# set_property PACKAGE_PIN AD2  [get_ports {PLPMOD6_D4}];  # JX1_HP_DP_22_P
# set_property PACKAGE_PIN AE2  [get_ports {PLPMOD6_D5}];  # JX1_HP_DP_22_N
# set_property PACKAGE_PIN W8   [get_ports {PLPMOD6_D6}];  # JX1_HP_DP_23_P
# set_property PACKAGE_PIN W7   [get_ports {PLPMOD6_D7}];  # JX1_HP_DP_23_N

set_property IOSTANDARD LVCMOS18 [get_ports {adc_3_spi_cs}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD6_D1}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_3_spi_miso}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_3_spi_clk}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD6_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD6_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD6_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD6_D7}]

# JX2 JA PMOD (Dual frame trigger)
set_property PACKAGE_PIN D2   [get_ports {frame_trig_0}];  # JX2_HP_DP_00_P
set_property PACKAGE_PIN D1   [get_ports {frame_trig_1}];  # JX2_HP_DP_00_N
# set_property PACKAGE_PIN A3   [get_ports {PLPMOD7_D2}];  # JX2_HP_DP_01_P
# set_property PACKAGE_PIN A2   [get_ports {PLPMOD7_D3}];  # JX2_HP_DP_01_N
# set_property PACKAGE_PIN C1   [get_ports {PLPMOD7_D4}];  # JX2_HP_DP_02_P
# set_property PACKAGE_PIN B1   [get_ports {PLPMOD7_D5}];  # JX2_HP_DP_02_N
# set_property PACKAGE_PIN H4   [get_ports {PLPMOD7_D6}];  # JX2_HP_DP_03_P
# set_property PACKAGE_PIN H3   [get_ports {PLPMOD7_D7}];  # JX2_HP_DP_03_N

set_property IOSTANDARD LVCMOS18 [get_ports {frame_trig_0}]
set_property IOSTANDARD LVCMOS18 [get_ports {frame_trig_1}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD7_D2}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD7_D3}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD7_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD7_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD7_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD7_D7}]

# JX2 JB PMOD
# set_property PACKAGE_PIN G2   [get_ports {PLPMOD8_D0}];  # JX2_HP_DP_04_P
# set_property PACKAGE_PIN F2   [get_ports {PLPMOD8_D1}];  # JX2_HP_DP_04_N
# set_property PACKAGE_PIN G3   [get_ports {PLPMOD8_D2}];  # JX2_HP_DP_05_P
# set_property PACKAGE_PIN F3   [get_ports {PLPMOD8_D3}];  # JX2_HP_DP_05_N
# set_property PACKAGE_PIN E2   [get_ports {PLPMOD8_D4}];  # JX2_HP_DP_06_P
# set_property PACKAGE_PIN E1   [get_ports {PLPMOD8_D5}];  # JX2_HP_DP_06_N
# set_property PACKAGE_PIN F4   [get_ports {PLPMOD8_D6}];  # JX2_HP_DP_07_P
# set_property PACKAGE_PIN E4   [get_ports {PLPMOD8_D7}];  # JX2_HP_DP_07_N

# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D0}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D1}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D2}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D3}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD8_D7}]

# JX2 JC PMOD
# set_property PACKAGE_PIN B7   [get_ports {PLPMOD9_D0}];  # JX2_HP_DP_08_P
# set_property PACKAGE_PIN A7   [get_ports {PLPMOD9_D1}];  # JX2_HP_DP_08_N
# set_property PACKAGE_PIN C6   [get_ports {PLPMOD9_D2}];  # JX2_HP_DP_09_P
# set_property PACKAGE_PIN B6   [get_ports {PLPMOD9_D3}];  # JX2_HP_DP_09_N
# set_property PACKAGE_PIN D7   [get_ports {PLPMOD9_D4}];  # JX2_HP_DP_10_GC_P
# set_property PACKAGE_PIN D6   [get_ports {PLPMOD9_D5}];  # JX2_HP_DP_10_GC_N
# set_property PACKAGE_PIN E6   [get_ports {PLPMOD9_D6}];  # JX2_HP_DP_11_GC_P
# set_property PACKAGE_PIN E5   [get_ports {PLPMOD9_D7}];  # JX2_HP_DP_11_GC_N

# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D0}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D1}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D2}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D3}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD9_D7}]

# JX2 JD PMOD
# set_property PACKAGE_PIN C4   [get_ports {PLPMOD10_D0}];  # JX2_HP_DP_12_GC_P
# set_property PACKAGE_PIN C3   [get_ports {PLPMOD10_D1}];  # JX2_HP_DP_12_GC_N
# set_property PACKAGE_PIN D4   [get_ports {PLPMOD10_D2}];  # JX2_HP_DP_13_GC_P
# set_property PACKAGE_PIN D3   [get_ports {PLPMOD10_D3}];  # JX2_HP_DP_13_GC_N
# set_property PACKAGE_PIN A9   [get_ports {PLPMOD10_D4}];  # JX2_HP_DP_14_P
# set_property PACKAGE_PIN A8   [get_ports {PLPMOD10_D5}];  # JX2_HP_DP_14_N
# set_property PACKAGE_PIN C5   [get_ports {PLPMOD10_D6}];  # JX2_HP_DP_15_P
# set_property PACKAGE_PIN B5   [get_ports {PLPMOD10_D7}];  # JX2_HP_DP_15_N

# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D0}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D1}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D2}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D3}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD10_D7}]

# JX2 JE PMOD
# set_property PACKAGE_PIN C8   [get_ports {PLPMOD11_D0}];  # JX2_HP_DP_16_P
# set_property PACKAGE_PIN B8   [get_ports {PLPMOD11_D1}];  # JX2_HP_DP_16_N
# set_property PACKAGE_PIN H1   [get_ports {PLPMOD11_D2}];  # JX2_HP_DP_17_P
# set_property PACKAGE_PIN G1   [get_ports {PLPMOD11_D3}];  # JX2_HP_DP_17_N
# set_property PACKAGE_PIN A6   [get_ports {PLPMOD11_D4}];  # JX2_HP_DP_18_P
# set_property PACKAGE_PIN A5   [get_ports {PLPMOD11_D5}];  # JX2_HP_DP_18_N
# set_property PACKAGE_PIN G7   [get_ports {PLPMOD11_D6}];  # JX2_HP_DP_19_P
# set_property PACKAGE_PIN F7   [get_ports {PLPMOD11_D7}];  # JX2_HP_DP_19_N

# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D0}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D1}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D2}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D3}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D4}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D5}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D6}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD11_D7}]

# JX2 JF PMOD (IMU 0)
set_property PACKAGE_PIN H6   [get_ports {imu_0_spi_cs}];  # JX2_HP_DP_20_P
set_property PACKAGE_PIN G6   [get_ports {imu_0_spi_mosi}];  # JX2_HP_DP_20_N
set_property PACKAGE_PIN G8   [get_ports {imu_0_spi_miso}];  # JX2_HP_DP_21_P
set_property PACKAGE_PIN F8   [get_ports {imu_0_spi_clk}];  # JX2_HP_DP_21_N
set_property PACKAGE_PIN G5   [get_ports {imu_0_imu_int}];  # JX2_HP_DP_22_P
# set_property PACKAGE_PIN F5   [get_ports {PLPMOD12_D5}];  # JX2_HP_DP_22_N
set_property PACKAGE_PIN B3   [get_ports {clk_19p2M}];  # JX2_HP_DP_23_P
# set_property PACKAGE_PIN B2   [get_ports {PLPMOD12_D7}];  # JX2_HP_DP_23_N

set_property IOSTANDARD LVCMOS18 [get_ports {imu_0_spi_cs}]
set_property IOSTANDARD LVCMOS18 [get_ports {imu_0_spi_mosi}]
set_property IOSTANDARD LVCMOS18 [get_ports {imu_0_spi_miso}]
set_property IOSTANDARD LVCMOS18 [get_ports {imu_0_spi_clk}]
set_property IOSTANDARD LVCMOS18 [get_ports {imu_0_imu_int}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD12_D5}]
set_property IOSTANDARD LVCMOS18 [get_ports {clk_19p2M}]
# set_property IOSTANDARD LVCMOS18 [get_ports {PLPMOD12_D7}]
