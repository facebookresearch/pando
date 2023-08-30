/* digital_input_ch.v
 *
 * This is a single channel input for the digital_inputs module.
 */

`timescale 1 ns / 1 ps

module digital_input_ch #(
           parameter TIMESTAMP_WIDTH = 64
       )
       (
           input wire resetn,
           input wire clk,

           // Digital input
           input wire d_in,

           // Timestamp from shared timestamp_generator instance
           input wire [TIMESTAMP_WIDTH-1 : 0] time_in,

           // AXI stream master port
           output reg [TIMESTAMP_WIDTH-1 : 0] edge_timestamp,
           output reg  edge_stored,
           output reg edge_direction,
           input wire  dst_ready
       );


always @(posedge clk)
begin
    if (!resetn)
    begin
        edge_timestamp    <= {TIMESTAMP_WIDTH{1'b0}};
        edge_stored <= 1'b0;
        edge_direction <= 1'b0;

    end
    else
    begin
        // Transmit stored edges
        if(edge_stored && dst_ready)
        begin
            edge_stored <= 1'b0;
        end

        // Store edges
        if (d_in != edge_direction)
        begin
            edge_timestamp <= time_in;
            edge_direction <= d_in;

            // If an edge (of the opposite polarity) is already stored but has
            // not been transmitted, don't transmit that edge or the current
            // edge. Else, signal availability of the current edge.
            if(edge_stored && ! dst_ready)
                edge_stored <= 1'b0;
            else
                edge_stored <= 1'b1;
        end

    end
end

endmodule
