`timescale 1ns / 1ns

module max11100_simple_tb;
   `include "tb_clock.v"

reg         resetn;
reg         clk;
reg 	       trigger;

wire        adc_data_ready;
wire [15:0] adc_data;
wire        spi_miso;


max11100 DUT0(
             .resetn(resetn),
             .clk(clk),
             .trigger(trigger),
             .data(adc_data),
             .data_ready(adc_data_ready),
             .spi_miso(spi_miso),
             .spi_sclk(spi_sclk),
             .spi_cs(spi_cs)
         );

// Model up a data source for the MISO signal by
// shifting a value every spi_sclk posedge
localparam TEST_SAMPLE_VALUE = 24'h001234;
reg [23:0] 		sample_data;

assign spi_miso = sample_data[23];

always @(negedge spi_sclk)
begin
    sample_data <= sample_data << 1;
end

always @(posedge adc_data_ready)
begin
    $display("%t -- ADC Sample Value: 0x%x", $time, adc_data);
end

initial begin
    // Set up the vcd dumps
    $dumpfile("max11100_simple_tb.vcd");
    $dumpvars(0, max11100_simple_tb);

    // Set up initial conditions
    resetn <= 1;
    clk <= 1;

    wait_cycles(2);

    // Assert resetn
    resetn <= 0;

    wait_cycles(2);

    // Deassert resetn
    resetn <= 1;
    sample_data <= TEST_SAMPLE_VALUE;

    wait_cycles(5);

    // Trigger a sample
    trigger <= 1;
    wait_cycles(1);
    trigger <= 0;

    // Wait for the data ready signal to assert
    @(posedge adc_data_ready);

    if (adc_data != TEST_SAMPLE_VALUE)
        $display("FAIL! ADC Sample Value = 0x%x, expected 0x%x", adc_data, TEST_SAMPLE_VALUE);

    wait_cycles(100);
    trigger <= 1;
    sample_data <= TEST_SAMPLE_VALUE;
    wait_cycles(1);
    trigger <= 0;

    // Wait for the data ready signal to assert
    @(posedge adc_data_ready);

    if (adc_data != TEST_SAMPLE_VALUE)
        $display("FAIL! ADC Sample Value = 0x%x, expected 0x%x", adc_data, TEST_SAMPLE_VALUE);

    wait_cycles(5);

    $display("Ending the test...");
    $finish;
end

endmodule
