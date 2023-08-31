`timescale 1ns / 1ns

module timestamp_generator_simple_tb;
`include "tb_clock.v"

reg resetn;
reg clk;
reg run;
wire [63:0] timestamp;

// Instantiate the timestamp generator
timestamp_generator DUT0(
                        .resetn(resetn),
                        .clk(clk),
                        .run(run),
                        .timestamp(timestamp)
                    );

initial
begin
    // Set up the vcd dumps
    $dumpfile("timestamp_generator_simple_tb.vcd");
    $dumpvars(0, timestamp_generator_simple_tb);
    $monitor("t:%4d timestamp=%d", $time, timestamp);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;

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

    wait_cycles(50); // Just kill a little time...

    if (timestamp != 50)
        $display("FAIL!");

    $display("Ending the test...");
    $finish;
end

endmodule
