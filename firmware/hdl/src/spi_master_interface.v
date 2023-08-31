/* spi_master_interface.v
 *
 * Abstracts SPI-based register read/write interface.
 *
 * Use (write operation):
 *    - Set read_write=0
 *    - Set address and data_in to desired values
 *    - assert trx_ready for 1 sys_clk cycle.
 *    - wait for trx_done to be asserted (for 1 sys_clk cycle).
 *
 * Use (single read operation):
 *   - Set read_write=1
 *   - Set address to desired value
 *   - assert trx_ready for 1 sys_clk cycle.
 *   - when trx_done is asserted, read transmitted value from data_out.
 *
 * Use (burst read operation):
 *   - Set read_write=1
 *   - Set burst_read=1 and burst_length to desired number of bytes to read
 *   - Set address to desired starting address (the SPI device will auto-increment it)
 *   - assert trx_ready for 1 sys_clk cycle.
 *   - each time trx_done is asserted, read transmitted value from data_out
 *      - trx_done will assert N=burst_length times, and the SPI master will not accept new
 *        transactions until it is finished
 *      - the user is responsible for keeping track of the number of times trx_done is asserted and
 *        which address is currently being read.
 *
 * A "transaction" is considered one complete read or write of a register. It normally consists of 16 bits
 * being transmissed across the SPI channel. 8 from the master (R/W bit + 7 address bits) and then either
 * another 8 from the master (for a write) or 8 from the SPI device (for a read). For a burst read, the first
 * transaction is like the above, but following it are (N-1) addition reads (where N=burst_length) with no addition
 * bytes transmitted from the master to the SPI device.
 *
 * This module generates a SPI clock at 1/100th the frequency of the module's system clock.
 *
 * input sys_clk       System clock. SPI clock will be 1/100th of this clock's frequency.
                       User must ensure that this clock speed is in spec for the SPI device.
 * input resetn        Module reset signal.
 * input read_write    Bit indicating whether to read or write a register. Write=0, Read=1.
 * input address [7]   Register address.
 * input data_in [8]   Byte to write to register. Can be left unset if not writing a register.
 * input burst_read    1 if a burst read is being initiated.
 * input burst_length  Number of bytes to read from device if doing a burst read.
 * output data_out [8] Byte read from register. Can be ignored if not reading a register.
 * input trx_ready     1 sys_clk cycle pulse instructing the module to begin a new transaction.
 *                     read_write, address, data_in (if writing), and burst settings (if doing a
 *                     burst read) must be set to their desired values before/when this signal is asserted.
 * output trx_done     1 sys_clk cycle pulse indicating that a transaction is completed.
 * output spi_clk      Clock used by SPI device. Is one-tenth the frequency of the system clock.
 * output spi_cs       CS line for SPI device.
 * output spi_mosi     MOSI line for SPI device.
 * input spi_miso      MISO line for SPI device.
 **/

module spi_master_interface(
           // system
           input sys_clk,
           input resetn,
           // signals with controlling module
           input read_write,
           input [6:0] address,
           input [7:0] data_in,
           input burst_read,
           input [5:0] burst_length,
           output [7:0] data_out,
           input trx_ready,
           output reg trx_done,
           // signals with spi slave
           output reg spi_clk,
           output reg spi_cs,
           output reg spi_mosi,
           input spi_miso
       );

localparam CMD_WRITE = 0;
localparam CMD_READ = 1;

reg [3:0] state;
// states
localparam S_RESET = 0;
localparam S_IDLE = 1;
localparam S_TX_ADDR = 2;
localparam S_WAIT_FOR_RX = 3;
localparam S_RX_DATA = 4;
localparam S_TX_DATA = 5;
localparam S_CS_HOLDOFF = 6;

// input/output registers
reg read_write_r;
reg [6:0] address_r;
reg [7:0] data_r;
reg [5:0] burst_length_r;
reg burst_read_r;

// helper registers
reg [6:0] spi_clk_counter; // creates spi clock with 1/100th of the system clock frequency (expecting a 100Mhz clock signal)
reg run_spi_clk; // enables or disables the spi clock.
reg [2:0] bit_counter; // keeps track of number of bits tx'd/rx'd
reg [5:0] burst_read_counter; // keeps track of burst reads
reg have_new_inputs; // tracks whether new inputs have been latched for the next transaction

localparam SPI_CLK_HIGH_EDGE = 50;
localparam SPI_CLK_LOW_EDGE = 0;
localparam SPI_CLK_CNT_MAX = 99;
localparam SPI_CLK_CS_HOLDOFF = 79;

// sets data_out to the byte tx'd or rx'd from spi slave on trx completion.
assign data_out = (trx_done) ? data_r : 8'b0;

initial begin
    trx_done = 0;
    spi_clk = 1;
    spi_cs = 1;
    spi_mosi = 0;

    read_write_r = 0;
    address_r = 0;
    data_r = 0;

    spi_clk_counter = 0;
    bit_counter = 0;

    state = S_RESET;

    have_new_inputs = 0;
    burst_read_r = 0;
    burst_read_counter = 0;
    burst_length_r = 0;
end

always @(posedge sys_clk)
begin
    // SPI clock generation and control.
    if (run_spi_clk)
    begin
        if (spi_clk_counter >= SPI_CLK_HIGH_EDGE) spi_clk <= 1;
        else spi_clk <= 0;
        if (spi_clk_counter == SPI_CLK_CNT_MAX) spi_clk_counter <= 0; // rollover
        else spi_clk_counter <= spi_clk_counter + 1;
    end
    else
    begin
        spi_clk_counter <= SPI_CLK_HIGH_EDGE;
        spi_clk <= 1; // Clock is high when not in use.
    end

    if (!resetn)
    begin
        state <= S_RESET;
    end
    else
    begin
        case (state)
            S_RESET: // Reset behavior.
            begin
                spi_cs <= 1;
                run_spi_clk <= 0;

                data_r <= 0;
                trx_done <= 0;
                state <= S_IDLE;
            end
            S_IDLE:
            begin
                trx_done <= 0;
                bit_counter <= 7;
                // In the middle of a burst read, continue.
                if (burst_read_r && !have_new_inputs && burst_read_counter != burst_length_r)
                begin
                    state <= S_RX_DATA;
                end
                // if there are new inputs to latch, latch them.
                else if (trx_ready && !have_new_inputs)
                begin
                    //latch inputs
                    read_write_r <= read_write;
                    address_r <= address;
                    burst_read_r <= burst_read;
                    burst_length_r <= burst_length;

                    if (read_write == CMD_WRITE) data_r <= data_in;
                    else data_r <= 0;

                    have_new_inputs <= 1;
                end
                // Disable clock at the end of its high period
                if (spi_clk_counter == SPI_CLK_CNT_MAX)
                begin
                    spi_mosi <= 0;
                    run_spi_clk <= 0;
                end
                // Once clock is disabled, re-assert cs.
                if (!run_spi_clk) spi_cs <= 1;
                // If there is new data, and this trx has finished totally, proceed.
                if (!run_spi_clk && have_new_inputs)
                begin
                    state <= S_CS_HOLDOFF;
                    run_spi_clk <= 1;
                    have_new_inputs <= 0;
                    // reset burst counter if a burst read is being initiated
                    if (burst_read_r) burst_read_counter <= 0;
                end
            end
            // Wait a little while, since CS needs to be held high for a little bit after a transcation.
            S_CS_HOLDOFF:
            begin
                if (spi_clk_counter == SPI_CLK_CS_HOLDOFF)
                begin
                    spi_cs <= 0;
                    state <= S_TX_ADDR;
                end
            end
            S_TX_ADDR:
            begin
                if (spi_clk_counter == SPI_CLK_LOW_EDGE) // transition new bit on falling edge
                begin
                    if (bit_counter == 7) spi_mosi <= read_write_r; // tx r/w bit.
                    else spi_mosi <= address_r[bit_counter]; // tx address, MSB first
                    if (bit_counter == 0)
                    begin
                        if (read_write_r == CMD_READ) state <= S_WAIT_FOR_RX;
                        else state <= S_TX_DATA;
                        bit_counter <= 7;
                    end
                    else bit_counter <= bit_counter - 1;
                end
            end
            S_TX_DATA:
            begin
                if (spi_clk_counter == SPI_CLK_LOW_EDGE) // transition new bit on falling edge
                begin
                    spi_mosi <= data_r[bit_counter];
                    if (bit_counter == 0)
                    begin
                        trx_done <= 1;
                        state <= S_IDLE;
                    end
                    else bit_counter <= bit_counter - 1;
                end
            end
            S_WAIT_FOR_RX:
            begin
                // the imu will shift the first data bit out on the next falling edge.
                // The spi master will need to latch the first data bit on the rising edge after that edge,
                // So wait for the next falling edge before expecting the data to come in.
                if (spi_clk_counter == SPI_CLK_LOW_EDGE) state <= S_RX_DATA;
            end
            S_RX_DATA:
            begin
                spi_mosi <= 0;
                if (spi_clk_counter == SPI_CLK_HIGH_EDGE) // latch new bit on rising edge
                begin
                    data_r[bit_counter] <= spi_miso;
                    if (bit_counter == 0)
                    begin
                        // deincrement burst_read_counter if doing a burst read.
                        if (burst_read_r) burst_read_counter <= burst_read_counter + 1;
                        trx_done <= 1;
                        state <= S_IDLE;
                    end
                    else bit_counter <= bit_counter - 1;
                end
            end

        endcase
    end
end

endmodule
