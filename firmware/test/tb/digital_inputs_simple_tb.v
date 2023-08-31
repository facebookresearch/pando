`timescale 1ns / 1ns

module digital_inputs_simple_tb;
`include "tb_clock.v"

// Generate 10MHz input_clk
always
begin
    #(100 / 2) input_clk <= !input_clk;
end

reg resetn;
reg clk;
reg input_clk;
reg run;

integer packet_count = 0;

localparam TDATA_WIDTH = 32;

wire m_axis_tvalid;
wire [TDATA_WIDTH-1:0] m_axis_tdata;
wire m_axis_tlast;
reg m_axis_tready;
reg [7 : 0] inputs;

// Instantiate digital_inputs module
digital_inputs #(.TIMESTAMP_WIDTH(64),
                 .C_M_AXIS_TDATA_WIDTH(TDATA_WIDTH),
                 .CONFIG_REG_DATA_WIDTH(32),
                 .CONFIG_REG_ADDR_WIDTH(3)) DUT0(
                   .resetn(resetn),
                   .clk(clk),
                   .input_clk(input_clk),
                   .run(run),
                   .inputs(inputs),
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
    $dumpfile("digital_inputs_simple_tb.vcd");
    $dumpvars(0, digital_inputs_simple_tb);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    input_clk <= 0;
    m_axis_tready <= 1;
    inputs <= 8'b0;

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

    // Generate some pulses on the inputs and observe the resulting AXI stream traffic...

    // Generate 1us pulse on input 0
    // Should receive timestamps 0 & 100, since run is asserted simultaneously.
    inputs <= 8'h01;
    wait_cycles(100);
    inputs <= 8'h00;

    wait_cycles(200);

    // Generate 1us pulse on input 2
    // Should receive timestamps 300 & 400
    inputs <= 8'h02;
    wait_cycles(100);
    inputs <= 8'h00;

    wait_cycles(200);

    // Generate simultaneous 1us pulse on all inputs
    // All should receive timestamps 600 & 700
    inputs <= 8'hFF;
    wait_cycles(100);
    inputs <= 8'h00;

    wait_cycles(200);

    // Should have 20 packets (10 pulses total, packet generated on each rise/fall)
    if (packet_count != 20)
        $display("FAIL! Total Packet Count = %d, expected 20", packet_count);

    $display("Ending the test...");
    $finish;
end

endmodule
