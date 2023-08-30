`timescale 1ns / 1ns

module traffic_generator_simple_tb;
`include "tb_clock.v"

reg resetn;
reg clk;
reg run;

integer packet_count = 0;

localparam TDATA_WIDTH = 32;

wire m_axis_tvalid;
wire [TDATA_WIDTH-1:0] m_axis_tdata;
wire m_axis_tlast;
reg 	m_axis_tready;

// Instantiate the timestamp generator
traffic_generator #(.TIMESTAMP_WIDTH(64),
                    .C_M_AXIS_TDATA_WIDTH(TDATA_WIDTH),
                    .CONFIG_REG_DATA_WIDTH(32),
                    .CONFIG_REG_ADDR_WIDTH(3)) DUT0(
                      .resetn(resetn),
                      .clk(clk),
                      .run(run),
                      .m_axis_tvalid(m_axis_tvalid),
                      .m_axis_tdata(m_axis_tdata),
                      .m_axis_tlast(m_axis_tlast),
                      .m_axis_tready(m_axis_tready)
                  );

// Examine AXI stream transactions
always @(posedge clk)
begin
    if (m_axis_tvalid && m_axis_tready)
    begin
        // TODO: Compare to a test vector
        $display("t:%4d tdata=0x%x tlast=%d", $time, m_axis_tdata, m_axis_tlast);

        if (m_axis_tlast)
            packet_count = packet_count + 1;
    end
end

initial
begin
    // Set up the vcd dumps
    $dumpfile("traffic_generator_simple_tb.vcd");
    $dumpvars(0, traffic_generator_simple_tb);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    m_axis_tready <= 1;

    // Force the configuration register that controls the traffic rate
    force DUT0.config_reg_clk_div = 100;

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

    wait_cycles(508);
    if (packet_count != 6)
        $display("FAIL! Total Packet Count = %d, expected 6", packet_count);

    $display("Ending the test...");
    $finish;
end

endmodule
