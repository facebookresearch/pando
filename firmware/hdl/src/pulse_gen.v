/* pulse_gen.v
 *
 * Generate a pulse train with configurable period and pulse width.
 */

`timescale 1 ns / 1 ps

module pulse_gen #(
           parameter COUNT_WIDTH = 32
       )(
           input wire resetn,
           input wire run,
           input wire clk,

           input wire [COUNT_WIDTH-1 : 0] period,
           input wire [COUNT_WIDTH-1 : 0] width,

           output reg pulse_out
       );

reg [COUNT_WIDTH-1 : 0] count;

// Continuously calculate (count + 1)
wire [COUNT_WIDTH-1 : 0] count_plus_one;
assign count_plus_one = count + 1;

// Generate pulses
always @(posedge clk)
begin
    if (!resetn || !run)
    begin
        pulse_out <= 1'b0;
        count <= 0;
    end
    else
    begin
        pulse_out <= (count < width) ? 1'b1 : 1'b0;
        count <= (count_plus_one < period) ? count_plus_one : {COUNT_WIDTH{1'b0}};
    end
end

endmodule
