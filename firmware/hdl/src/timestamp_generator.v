
/* timestamp_generator.v
 *
 * Counts clock cycles. To be used for local generation of timestamps.
 */

`timescale 1 ns / 1 ps

module timestamp_generator #(
           parameter TIMESTAMP_WIDTH = 64
       )(
           input wire resetn,
           input wire clk,
           input wire run,

           output reg[TIMESTAMP_WIDTH-1 : 0] timestamp
       );

always @(posedge clk)
begin
    if (!resetn || !run)
        timestamp <= {TIMESTAMP_WIDTH{1'b0}};
    else
        timestamp <= timestamp + 1;
end

endmodule
