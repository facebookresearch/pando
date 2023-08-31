`timescale 1ns / 1ns

module adc_mock_tb;
`include "tb_clock.v"

// Set an overall test timeout
initial
begin
    wait_cycles(50000);
    $display("FAIL! Test did not complete!");
    $finish;
end

reg resetn;
reg clk;
reg run;

localparam TDATA_WIDTH = 32;
localparam TEST_DEVICE_ID = 1;

wire m_axis_tvalid;
wire [TDATA_WIDTH-1:0] m_axis_tdata;
wire m_axis_tlast;
reg 	m_axis_tready;

wire m_spi_clk;
wire m_spi_cs;
wire m_spi_miso;


// Instantiate the adc module
adc #(.TIMESTAMP_WIDTH(64),
      .C_M_AXIS_TDATA_WIDTH(TDATA_WIDTH),
      .CONFIG_REG_DATA_WIDTH(32),
      .CONFIG_REG_ADDR_WIDTH(3),
      .DEVICE_ID(TEST_DEVICE_ID)) DUT0(
        .resetn(resetn),
        .clk(clk),
        .run(run),
        .m_axis_tvalid(m_axis_tvalid),
        .m_axis_tdata(m_axis_tdata),
        .m_axis_tlast(m_axis_tlast),
        .m_axis_tready(m_axis_tready),
        .m_spi_clk(m_spi_clk),
        .m_spi_cs(m_spi_cs),
        .m_spi_miso(m_spi_miso)
    );

// MISO not used in mock mode
assign m_spi_miso = 0;


integer mock_sample_cnt = 0;
always @(posedge clk)
begin
    if (m_axis_tvalid)
    begin
        $display("t:%4d run=%d tlast=%d tdata=0x%x", $time, run, m_axis_tlast, m_axis_tdata);

        // When the last word is emitted, check the value and increment the sample counter
        if (m_axis_tlast)
        begin
            mock_sample_cnt <= mock_sample_cnt + 1;

            if (m_axis_tlast && (m_axis_tdata[31:16] != mock_sample_cnt) && (m_axis_tdata[15:0] != TEST_DEVICE_ID[15:0]))
            begin
                $display("FAIL! Got 0x%x, expected 0x%x", m_axis_tdata, {TEST_DEVICE_ID[15:0], mock_sample_cnt});
            end
        end
    end
end

initial
begin
    // Set up the vcd dumps
    $dumpfile("adc_overflow_fault_tb.vcd");
    $dumpvars(0, adc_mock_tb);
    // $monitor("t:%4d run=%d tlast=%d tdata=0x%x", $time, run, m_axis_tlast, m_axis_tdata);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    m_axis_tready <= 1;

    // Set the register values
    force DUT0.config_reg_samp_rate_div = 1000;
    force DUT0.enable = 1;
    force DUT0.mock = 1;

    // Wait a few clock cycles before asserting the reset line
    wait_cycles(3);

    // Assert the reset signal for a few clock cycles
    resetn <= 0;
    wait_cycles(3);

    // Deassert reset
    resetn <= 1;
    wait_cycles(3);

    // Drive some stimulus
    $display("Starting the test...");

    // Assert Run
    run <= 1;

    // Wait for valid data
    repeat(3) @(posedge m_axis_tvalid);

    // De-assert t_ready
    m_axis_tready <= 0;

    // Wait for one sample period
    wait_cycles(1000);

    if(DUT0.state_r != DUT0.S_FAULT)
        $display("FAIL! Did not enter fault state as expected");

    // Wait for one sample period
    wait_cycles(1000);

    $display("Ending the test...");
    $finish;
end

endmodule
