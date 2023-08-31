`timescale 1ns / 1ns

module imu_simple_tb;
`include "tb_clock.v"

reg resetn;
reg clk;
reg run;

localparam TDATA_WIDTH = 32;

wire m_axis_tvalid;
wire [TDATA_WIDTH-1:0] m_axis_tdata;
wire m_axis_tlast;
reg 	m_axis_tready;

reg imu_int;
reg imu_spi_miso;
wire imu_spi_clk;
wire imu_spi_mosi;
wire imu_spi_cs;

// Instantiate the imu module
imu #(.TIMESTAMP_WIDTH(64),
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
        .imu_int(imu_int),
        .m_spi_miso(imu_spi_miso),
        .m_spi_mosi(imu_spi_mosi),
        .m_spi_cs(imu_spi_cs),
        .m_spi_clk(imu_spi_clk)
    );

always @(posedge imu_spi_clk)
begin
    $display("MOSI: %d MISO: %d", imu_spi_mosi, imu_spi_miso);
end

initial
begin
    // Set up the vcd dumps
    $dumpfile("imu_simple_tb.vcd");
    $dumpvars(0, imu_simple_tb);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    m_axis_tready <= 1;

    // Force the configuration register bit that ungates the run signal
    force DUT0.enable = 1'b1;

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

    wait_cycles(500);
    // FIXME: Fill in the rest of the test
    //$display("FAIL! Test needs to be implemented");

    $display("Ending the test...");
    $finish;
end

endmodule
