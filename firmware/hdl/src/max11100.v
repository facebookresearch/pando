/* max11100.v
 *
 * This module is for coordinating data acquisition from a MAX11100.
 * Intended platform is the UltraZed w/ IO Carrier, with a MAXREFDES11#
 * attached via PMOD port.
 * 
 * Assume a 100MHz clock on clk - adjust CLOCK_PERIOD parameter if other clock is used.
 * This will actually cause the MAX11100 to have a 4.76MHz spi_sclk...
 * 
 * spi_sclk: 4.8MHz @ 50% duty cycle.
 * 
 * To start a conversion, pull CS low.  A falling edge will start the conversion
 * SCLK drives the A/D conversion and shifts the data out on MISO/DOUT.
 * The MSB of the sample will appear on DOUT after the 8th SCLK cycle.
 * 24 SCLK cycles are needed to acquire the full sample.
 * 
 * Reset the ADC by driving CS high
 */

`timescale 1ns / 1ps

module max11100 #(
           parameter CLOCK_PERIOD = 10 // the input clock period, in nanoseconds
       )
       (
           input wire 	      resetn,
           input wire 	      clk,
           input wire 	      trigger,
           output reg [15:0] data,
           output wire       data_ready,

           // Signals to the MAX11100
           input wire 	      spi_miso,
           output wire       spi_sclk,
           output reg 	      spi_cs
       );

localparam integer T_SCLK = $ceil(208.0 / CLOCK_PERIOD);
localparam integer T_CSS = $ceil(100.0 / CLOCK_PERIOD);
localparam integer T_CSW = $ceil(50.0 / CLOCK_PERIOD);
localparam integer TRANSACTION_CYCLES = (24 * T_SCLK) + T_CSS; // The ADC samples at 200ksps

initial
begin
    $display("MAX11100 Calculated Parameters:");
    $display("T_SCLK = %d", T_SCLK);
    $display("T_CSS =  %d", T_CSS);
    $display("T_CSW = %d", T_CSW); // FIXME: Not needed?
end

// Generate the appropriate SCLK when sclk_en is asserted
reg sclk_en;
reg [$clog2(T_SCLK)-1:0] sclk_div_cnt;

// Generate the SPI SCLK signal and a read_data pulse on the rising edge of sclk
reg last_spi_sclk;
assign spi_sclk = sclk_en && (sclk_div_cnt < $rtoi(T_SCLK / 2));
assign read_data = !last_spi_sclk && spi_sclk;

// Implement the counter for the clock divisor
always @(posedge clk)
begin
    // Reset or increment the internal clock divisor counter
    if (!sclk_en || (sclk_div_cnt == T_SCLK - 1))
    begin
        sclk_div_cnt <= 0;
    end
    else
    begin
        sclk_div_cnt <= sclk_div_cnt + 1;
    end

    last_spi_sclk <= spi_sclk;
end


// Counter for controlling the timing of a transaction
reg [$clog2(TRANSACTION_CYCLES)-1:0] trans_timer_cnt;

always @(posedge clk)
begin
    // Reset the counter when spi_cs is deasserted (high)
    if (spi_cs)
    begin
        trans_timer_cnt <= 0;
    end
    else
    begin
        trans_timer_cnt <= trans_timer_cnt + 1;
    end
end

// Generate the proper CS and SCLK_EN signals
always @(posedge clk)
begin
    if (!resetn)
    begin
        sclk_en <= 0;
        spi_cs <= 1;
    end
    else
    begin
        if (trigger && trans_timer_cnt == 0)
        begin
            // Pull CS low to have the ADC start the conversion
            spi_cs <= 0;
        end
        else if (trans_timer_cnt == T_CSS - 1)
        begin
            // Wait T_CSS cycles before starting SCLK
            sclk_en <= 1;
        end
        else if (trans_timer_cnt == TRANSACTION_CYCLES - 1)
        begin
            // Deassert control signals at the end of the transaction
            sclk_en <= 0;
            spi_cs <= 1;
        end
    end
end

// Sample MISO data on the rising spi_sclk edge
reg [4:0] sample_cnt;
wire 	     spi_data_ready;

assign spi_data_ready = sample_cnt == 24;

always @(posedge clk)
begin
    if (spi_cs)
    begin
        // Reset the sample counter when spi_cs is deasserted
        sample_cnt <= 0;
    end
    else
    begin
        // read_data is pulsed on the spi_sclk rising edge
        if (read_data)
        begin
            sample_cnt <= sample_cnt + 1;
            data <= {data[14:0], spi_miso};
        end
    end
end


// Generate a pulse indicating that data is ready and valid
reg last_spi_data_ready;
assign data_ready = !last_spi_data_ready && spi_data_ready;

always @(posedge clk)
begin
    last_spi_data_ready <= spi_data_ready;
end

endmodule
