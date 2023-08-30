`timescale 1ns / 1ns

module example_tb;

initial
begin
    // Drive some stimulus
    $display("Starting the test...");

    #(100); // Just kill a little time...
    $display("Ending the test...");
end

endmodule
