/* mock_imu.v
 *
 * Virtual IMU device that responds to register reads to sensor registers.
 *
 * Register values update on assertions of sample_trigger. Register values are
 * sequences of six incrementing numbers. The first number increases by one each
 * time the values update. Ex. (1,2,3,4,5,6), (2,3,4,5,6,7), ...
 *
 * When the registers update, an interrupt will fire (instructing the SPI master
 * to begin reading the new values). The interrupt is cleared on a register read
 * or after 50us, whichever is soonest. The values will not update if a trans-
 * action is in progress (cs asserted).
 *
 * input sys_clk        System clock.
 * input resetn         Reset signal.
 * input sample_trigger 1 sys_clk period pulse instructing the module to update the sensor registers.
 * output imu_int       Interrupt signal from IMU. Fires when sensor registers are updated.
 *                      Clears after 50us or on a read. Whichever is soonest.
 * input imu_mosi       SPI MISO signal.
 * output imu_miso      SPI MISO signal.
 * input imu_clk        SPI clock signal.
 * input imu_cs         SPI CS signal.
 *
*/

module mock_imu(
           // system signals
           input sys_clk,
           input resetn,
           input sample_trigger,

           //IMU spi wires
           output reg imu_int,
           input imu_mosi,
           output reg imu_miso,
           input imu_clk,
           input imu_cs
       );

/* register addresses of sensor registers on real device. */
localparam ADDR_AXH = 7'h3b;
localparam ADDR_AXL = 7'h3c;
localparam ADDR_AYH = 7'h3d;
localparam ADDR_AYL = 7'h3e;
localparam ADDR_AZH = 7'h3f;
localparam ADDR_AZL = 7'h40;
localparam ADDR_TH = 7'h41;
localparam ADDR_TL = 7'h42;
localparam ADDR_GXH = 7'h43;
localparam ADDR_GXL = 7'h44;
localparam ADDR_GYH = 7'h45;
localparam ADDR_GYL = 7'h46;
localparam ADDR_GZH = 7'h47;
localparam ADDR_GZL = 7'h48;

// commands
localparam CMD_WRITE = 0;
localparam CMD_READ = 1;

// states
localparam S_RESET = 0;
localparam S_IDLE = 1;
localparam S_RX_RW = 2;
localparam S_RX_ADDR = 3;
localparam S_TX_DATA = 4;
localparam S_RX_DATA = 5;

reg [2:0] state;

reg [15:0] gx, gy, gz, ax, ay, az; /* sensor registers. */

// helper registers
reg delayed_sample_trigger;
reg [3:0] bit_counter;
reg last_imu_clk;

// spi registers
reg read_write;
reg [6:0] address;
reg [7:0] byte_out;


initial begin
    gx = 0;
    gy = 0;
    gz = 0;
    ax = 0;
    ay = 0;
    az = 0;

    state = S_RESET;

    delayed_sample_trigger = 0;
    bit_counter = 0;
    last_imu_clk = 0;
    read_write = 0;
    address = 0;
    byte_out = 0;
    imu_miso = 0;
    imu_int = 0;
end

reg [7:0] next_addr;


localparam IMU_TIMER_MAX = 50000;
localparam IMU_TIMER_NBITS = $clog2(IMU_TIMER_MAX + 1);
reg [IMU_TIMER_NBITS-1:0] imu_int_timer;

always @(posedge sys_clk)
begin
    if (!resetn) // reset sensor registers when module resets
    begin
        gx <= 0;
        gy <= 0;
        gz <= 0;
        ax <= 0;
        ay <= 0;
        az <= 0;
        imu_int <= 0;
    end
    else if ((sample_trigger || delayed_sample_trigger) && imu_cs)
    begin
        /* set sensor registers. */
        delayed_sample_trigger <= 0;
        gx <= gx + 1;
        gy <= gx + 2;
        gz <= gx + 3;
        ax <= gx + 4;
        ay <= gx + 5;
        az <= gx + 6;
        imu_int <= 1; // throw interrupt
        imu_int_timer <= IMU_TIMER_MAX; // 50 us timeout
    end
    else if (sample_trigger && !imu_cs)
    begin
        /* Set delayed trigger (to be updated when cs clears).
           This mimics the behavior of the real IMU which implies
           that it won't update the sensor registers during a continuous read,
           which I interpret to be holding cs low thru multiple transactions. */
        delayed_sample_trigger <= 1;
    end

    // control clearing of imu_int. Clears on any read or if a 50us timer lapses.
    if (imu_int && (state == S_TX_DATA || imu_int_timer == 0)) imu_int <= 0;
    else if (imu_int) imu_int_timer <= imu_int_timer - 1;

    /* Reset if reset asserted or if cs line is deasserted at an incorrect spot in the trx. */
    if (!resetn || (!(state == S_RESET || state == S_IDLE) && imu_cs))
    begin
        state <= S_RESET;
    end
    else
    begin
        case (state)
            S_RESET:
            begin
                bit_counter <= 7;
                address <= 0;
                byte_out <= 0;
                read_write <= 0;
                state <= S_IDLE;
            end
            S_IDLE:
            begin
                imu_miso <= imu_cs ? 0 : imu_miso; /* Hold miso low. Shouldn't really matter, but means it will always be low when a trx isn't happening, */
                last_imu_clk <= imu_clk;
                if (!imu_cs && !imu_clk && last_imu_clk) // Wait for cs to be asserted and a falling edge on spi_clk
                begin
                    imu_miso <= 0;
                    bit_counter <= 7;
                    state <= S_RX_RW;
                end
                next_addr <= address + 1; // in case we need a burst read
                // if we see a rising edge in this state and CS is still asserted, then doing a burst read.
                if (!imu_cs && imu_clk && !last_imu_clk)
                begin
                    bit_counter <= 7;
                    address <= next_addr;
                    case(next_addr)
                        ADDR_AXH: byte_out <= ax[15:8];
                        ADDR_AXL: byte_out <= ax[7:0];
                        ADDR_AYH: byte_out <= ay[15:8];
                        ADDR_AYL: byte_out <= ay[7:0];
                        ADDR_AZH: byte_out <= az[15:8];
                        ADDR_AZL: byte_out <= az[7:0];
                        ADDR_TH: byte_out <= 8'b0;
                        ADDR_TL: byte_out <= 8'b0;
                        ADDR_GXH: byte_out <= gx[15:8];
                        ADDR_GXL: byte_out <= gx[7:0];
                        ADDR_GYH: byte_out <= gy[15:8];
                        ADDR_GYL: byte_out <= gy[7:0];
                        ADDR_GZH: byte_out <= gz[15:8];
                        ADDR_GZL: byte_out <= gz[7:0];
                        default:  byte_out <= 8'b0; //TODO reasonable behavior?
                    endcase
                    state <= S_TX_DATA;
                end
            end
            S_RX_RW:
            begin
                last_imu_clk <= imu_clk;
                if (imu_clk && !last_imu_clk) // latch in data on rising edge
                begin
                    read_write <= imu_mosi;
                    bit_counter <= 6;
                    state <= S_RX_ADDR;
                end
            end
            S_RX_ADDR:
            begin
                last_imu_clk <= imu_clk;
                if (imu_clk && !last_imu_clk) // latch in data on rising edge
                begin
                    address[bit_counter] <= imu_mosi;
                    if (bit_counter != 0) bit_counter <= bit_counter - 1;
                    else // done receiving address.
                    begin
                        bit_counter <= 7;
                        if (read_write == CMD_WRITE) state <= S_RX_DATA;
                        else if (read_write == CMD_READ)
                        begin
                            imu_int <= 0; // clear interrupt on read.
                            case({address[6:1], imu_mosi})
                                ADDR_AXH: byte_out <= ax[15:8];
                                ADDR_AXL: byte_out <= ax[7:0];
                                ADDR_AYH: byte_out <= ay[15:8];
                                ADDR_AYL: byte_out <= ay[7:0];
                                ADDR_AZH: byte_out <= az[15:8];
                                ADDR_AZL: byte_out <= az[7:0];
                                ADDR_TH: byte_out <= {8{1'b0}};
                                ADDR_TL: byte_out <= {8{1'b0}};
                                ADDR_GXH: byte_out <= gx[15:8];
                                ADDR_GXL: byte_out <= gx[7:0];
                                ADDR_GYH: byte_out <= gy[15:8];
                                ADDR_GYL: byte_out <= gy[7:0];
                                ADDR_GZH: byte_out <= gz[15:8];
                                ADDR_GZL: byte_out <= gz[7:0];
                                default:  byte_out <= {8{1'b0}}; //TODO reasonable behavior?
                            endcase
                            state <= S_TX_DATA;
                        end
                    end
                end
            end
            S_TX_DATA:
            begin
                last_imu_clk <= imu_clk;
                if (!imu_clk && last_imu_clk) // transition outgoing data on falling edge.
                begin
                    imu_miso <= byte_out[bit_counter];
                    if (bit_counter == 0) state <= S_IDLE;
                    else bit_counter <= bit_counter - 1;
                end
            end
            // Not doing anything on writes since this is a mock device.
            // Just waits an appropriate amount of time before changing state to idle.
            S_RX_DATA:
            begin
                last_imu_clk <= imu_clk;
                if (imu_clk && !last_imu_clk)
                begin
                    if (bit_counter == 0) state <= S_IDLE;
                    else bit_counter <= bit_counter - 1;
                end
            end
        endcase
    end
end

endmodule
