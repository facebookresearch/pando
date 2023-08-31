`timescale 1ns / 1ns

module adc_simple_tb;
`include "tb_clock.v"

// Set an overall test timeout
initial
begin
    wait_cycles(500000);
    $display("FAIL! Test did not complete!");
    $finish;
end

reg resetn;
reg clk;
reg run;

localparam TDATA_WIDTH = 32;

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
      .CONFIG_REG_ADDR_WIDTH(3)) DUT0(
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


// Model up a data source for the MISO signal by
// shifting a value every spi_sclk posedge
localparam TEST_SAMPLE_VALUE = 24'h001EAF;
reg [23:0] 		sample_data;

assign m_spi_miso = sample_data[23];

always @(negedge m_spi_clk or posedge m_spi_cs)
begin
    sample_data <= sample_data << 1;

    if (m_spi_cs == 1)
    begin
        sample_data <= TEST_SAMPLE_VALUE;
    end
end

always @(posedge clk)
begin
    if (m_axis_tvalid)
    begin
        $display("t:%4d run=%d tlast=%d tdata=0x%x", $time, run, m_axis_tlast, m_axis_tdata);

        if (m_axis_tlast && m_axis_tdata[31:16] != TEST_SAMPLE_VALUE[15:0])
        begin
            $display("FAIL! m_axis_tdata[31:16] = 0%x, expected 0x%x", m_axis_tdata[31:16], TEST_SAMPLE_VALUE[15:0]);
        end
    end
end

initial
begin
    // Set up the vcd dumps
    $dumpfile("adc_simple_tb.vcd");
    $dumpvars(0, adc_simple_tb);
    // $monitor("t:%4d run=%d tlast=%d tdata=0x%x", $time, run, m_axis_tlast, m_axis_tdata);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    m_axis_tready <= 1;

    // Set the register values
    force DUT0.config_reg_samp_rate_div = 1000;
    force DUT0.enable = 1;

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
    sample_data <= TEST_SAMPLE_VALUE;

    // Wait for valid data
    repeat(3) @(posedge m_axis_tvalid);

    wait_cycles(10);

    $display("Ending the test...");
    $finish;
end

endmodule
