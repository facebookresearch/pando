/* imu.v
 *
 * This peripheral acquires data from a MPU6050 SPI IMU.
 *
 * FIXME: This is just a stub.
 */

`timescale 1 ns / 1 ps

module imu #(
           parameter TIMESTAMP_WIDTH = 64,

           parameter integer C_M_AXIS_TDATA_WIDTH    = 32,
           parameter integer CONFIG_REG_DATA_WIDTH = 32,
           parameter integer CONFIG_REG_ADDR_WIDTH = 3,

           parameter integer DEVICE_ID = 0
       )
       (
           input wire resetn,
           input wire clk,
           input wire run,

           // INT signal from MPU6050
           input wire imu_int,

           // Sample Trigger (for mock IMU)
           input wire sample_trigger,

           // SPI Master interface
           output wire m_spi_clk,
           input wire m_spi_miso,
           output wire m_spi_mosi,
           output wire m_spi_cs,

	   // Fault LED
           output wire fault,

           // AXI stream master port
           output wire  m_axis_tvalid,
           output wire [C_M_AXIS_TDATA_WIDTH-1 : 0] m_axis_tdata,
           output wire [(C_M_AXIS_TDATA_WIDTH/8)-1 : 0] m_axis_tstrb,
           output wire  m_axis_tlast,
           input wire  m_axis_tready,

           // AXI4-lite slave port for config registers
           input wire [CONFIG_REG_ADDR_WIDTH-1 : 0] s_axi_awaddr,
           input wire [2 : 0] s_axi_awprot,
           input wire  s_axi_awvalid,
           output wire  s_axi_awready,
           input wire [CONFIG_REG_DATA_WIDTH-1 : 0] s_axi_wdata,
           input wire [(CONFIG_REG_DATA_WIDTH/8)-1 : 0] s_axi_wstrb,
           input wire  s_axi_wvalid,
           output wire  s_axi_wready,
           output wire [1 : 0] s_axi_bresp,
           output wire  s_axi_bvalid,
           input wire  s_axi_bready,
           input wire [CONFIG_REG_ADDR_WIDTH-1 : 0] s_axi_araddr,
           input wire [2 : 0] s_axi_arprot,
           input wire  s_axi_arvalid,
           output wire  s_axi_arready,
           output wire [CONFIG_REG_DATA_WIDTH-1 : 0] s_axi_rdata,
           output wire [1 : 0] s_axi_rresp,
           output wire  s_axi_rvalid,
           input wire  s_axi_rready
       );

// MPU-6050 Reset Registers
localparam N_IMU_RST_REGS = 2;
localparam RST_REG_INDEX_NBITS = $clog2(N_IMU_RST_REGS + 1); // this index must be able to equal N_IMU_RST_REGS
// so adding one ensures this even when N_IMU_RST_REGS
// is a power of two.

localparam [N_IMU_RST_REGS*7-1:0] IMU_RST_REG_ADDRS = {
               7'h68, // SIGNAL_PATH_RESET
               7'h6b  // PWR_MGMT_1
           };

localparam [N_IMU_RST_REGS*8-1:0] IMU_RST_REG_VALS = {
               8'h03, // resets all sensor signal paths
               8'h80  // general device reset
           };

// Timing delay setup between reset register writes
localparam IMU_RST_DELAY = 10000000; // clock cycles = 100ms @ clk=100Mhz
localparam RST_COUNTER_NBITS = $clog2(IMU_RST_DELAY + 1);

// MPU-6050 Configuration Registers
localparam N_IMU_CONFIG_REGS = 6;
localparam CONFIG_REG_INDEX_NBITS = $clog2(N_IMU_CONFIG_REGS + 1); // # of bits in the index register

localparam [N_IMU_CONFIG_REGS*7-1:0] IMU_CONFIG_REG_ADDRS = {
               7'h38, //INT_ENABLE
               7'h37, //INT_PIN_CFG
               7'h19, //SMPLRT_DIV
               7'h1a, //CONFIG
               7'h6a, //USER_CTRL
               7'h6b  //PWR_MGMT_1
           };

localparam [N_IMU_CONFIG_REGS*8-1:0] IMU_CONFIG_REG_VALS = {
               8'h01, // enables interrupt on data ready
               8'h10, // configures interrupt settings
               8'h07, // sets gyro sample rate to 1khz
               8'h08, // disables the DLPF, sets FSYNC bit to TEMP_LOW[0]
               8'h10, // disables I2C. Can be used to reset.
               8'h05  // selects external 19.2Mhz ref clock
           };

// Timing delay setup between config register writes.
localparam IMU_CFG_DELAY = 10000; // clock cycles = 100us @ clk=100Mhz
localparam CFG_COUNTER_NBITS = $clog2(IMU_CFG_DELAY + 1);

// MPU-6050 WHOAMI Register
// Used to sanity-check after configuration
// And insure the IMU is readable before starting experiment
localparam WHOAMI_ADDR = 7'h75;
localparam WHOAMI_VAL = 8'h68;

// Instantiation of module timestamp_generator
wire [TIMESTAMP_WIDTH-1 : 0] timestamp_gen_out;
timestamp_generator TIMESTAMP_GENERATOR (
                        .resetn     (resetn),
                        .clk        (clk),
                        .run        (run),
                        .timestamp  (timestamp_gen_out));


// Instantiation of module spi_master_interface
wire m_spi_clk_mxd, m_spi_cs_mxd, m_spi_mosi_mxd, m_spi_miso_mxd;
wire [6:0] spi_trx_addr;
wire spi_trx_rw;
wire [7:0] spi_trx_dout, spi_trx_din;
wire spi_trx_ready;
wire spi_trx_done;
wire spi_trx_burst_read;
wire [5:0] spi_trx_burst_length;

assign spi_trx_addr = spi_trx_addr_r;
assign spi_trx_din = spi_trx_din_r;
assign spi_trx_rw = spi_trx_rw_r;
assign spi_trx_ready = spi_trx_ready_r;
assign spi_trx_burst_read = spi_trx_burst_read_r;
assign spi_trx_burst_length = spi_trx_burst_length_r;

spi_master_interface SPI_MASTER_INTERFACE(
                         .sys_clk      (clk),
                         .resetn       (resetn),
                         .read_write   (spi_trx_rw),
                         .address      (spi_trx_addr),
                         .data_in      (spi_trx_din),
                         .data_out     (spi_trx_dout),
                         .burst_read   (spi_trx_burst_read),
                         .burst_length (spi_trx_burst_length),
                         .trx_ready    (spi_trx_ready),
                         .trx_done     (spi_trx_done),
                         .spi_clk      (m_spi_clk_mxd),
                         .spi_cs       (m_spi_cs_mxd),
                         .spi_mosi     (m_spi_mosi_mxd),
                         .spi_miso     (m_spi_miso_mxd));

// Instantiation of module mock_imu
wire mock_m_spi_mosi, mock_m_spi_miso, mock_m_spi_clk, mock_m_spi_cs;
wire mock_imu_int;
mock_imu MOCK_IMU(
             .sys_clk        (clk),
             .resetn         (resetn),
             .sample_trigger (sample_trigger),
             .imu_int        (mock_imu_int),
             .imu_mosi       (mock_m_spi_mosi),
             .imu_miso       (mock_m_spi_miso),
             .imu_clk        (mock_m_spi_clk),
             .imu_cs         (mock_m_spi_cs));


// Multiplex SPI signals between real and mocked IMU.
assign m_spi_clk = (mock) ? 1 : m_spi_clk_mxd;
assign m_spi_cs = (mock) ? 1 : m_spi_cs_mxd;
assign m_spi_mosi = (mock) ? 0 : m_spi_mosi_mxd;

assign mock_m_spi_clk = (mock) ? m_spi_clk_mxd : 1;
assign mock_m_spi_cs = (mock) ? m_spi_cs_mxd : 1;
assign mock_m_spi_mosi = (mock) ? m_spi_mosi_mxd : 0;

assign m_spi_miso_mxd = (mock) ? mock_m_spi_miso : m_spi_miso;

// Multiplex interrupt signal.
wire imu_int_mxd;
assign imu_int_mxd = (mock) ? mock_imu_int : imu_int;

// Constants
localparam SAMPLE_TYPE = 32'h5350_0001;
localparam SAMPLE_SIZE = 32'h20;

localparam SENSOR_DATA_WIDTH = 16; // MPU-6050 sensors are 16 bits wide.

// IMU Register addresses
// Note that only IMU_ADDR_AXH is used in this implementation
// since we are using burst reads. The additional register
// addresses have been left for reference only.
localparam IMU_ADDR_AXH = 7'h3b;
localparam IMU_ADDR_AXL = 7'h3c;
localparam IMU_ADDR_AYH = 7'h3d;
localparam IMU_ADDR_AYL = 7'h3e;
localparam IMU_ADDR_AZH = 7'h3f;
localparam IMU_ADDR_AZL = 7'h40;

localparam IMU_ADDR_TH = 7'h41;
localparam IMU_ADDR_TL = 7'h42;

localparam IMU_ADDR_GXH = 7'h43;
localparam IMU_ADDR_GXL = 7'h44;
localparam IMU_ADDR_GYH = 7'h45;
localparam IMU_ADDR_GYL = 7'h46;
localparam IMU_ADDR_GZH = 7'h47;
localparam IMU_ADDR_GZL = 7'h48;

// IMU Command bit definitons.
localparam IMU_REG_READ = 1'b1;
localparam IMU_REG_WRITE = 1'b0;

// Location of each axis in sensor_data_arr.
localparam AX_AXIS_INDEX = 0;
localparam AY_AXIS_INDEX = 1;
localparam AZ_AXIS_INDEX = 2;
localparam T_AXIS_INDEX = 3;
localparam GX_AXIS_INDEX = 4;
localparam GY_AXIS_INDEX = 5;
localparam GZ_AXIS_INDEX = 6;
localparam N_SENSOR_AXES = 7;
localparam SENSOR_AXIS_INDEX_NBITS = $clog2(N_SENSOR_AXES + 1);

localparam SENSOR_LOW_BYTE = 0;
localparam SENSOR_HIGH_BYTE = 1;

// Enumeration of FSM states
localparam S_RESET = 0;
localparam S_FAULT = 1;
localparam S_DISABLED = 2;
localparam S_RST_BEGIN = 3;
localparam S_RST_FINISH = 4;
localparam S_RST_WAIT = 5;
localparam S_CFG_BEGIN = 6;
localparam S_CFG_FINISH = 7;
localparam S_CFG_WAIT = 8;
localparam S_WHOAMI_BEGIN = 9;
localparam S_WHOAMI_FINISH = 10;
localparam S_IDLE = 11;
localparam S_START_BURST_RX = 12;
localparam S_RX_SENSOR_DATA = 13;
localparam S_XMIT_WORD_0 = 14;
localparam S_XMIT_WORD_1 = 15;
localparam S_XMIT_WORD_2 = 16;
localparam S_XMIT_WORD_3 = 17;
localparam S_XMIT_WORD_4 = 18;
localparam S_XMIT_WORD_5 = 19;
localparam S_XMIT_WORD_6 = 20;
localparam S_XMIT_WORD_7 = 21;
localparam S_XMIT_DONE = 22;
localparam STATE_COUNT = 23;

localparam STATE_NBITS = $clog2(STATE_COUNT); // # of bits in the state register

// Instantiation of module axi_registers_ps_pl
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_flags_0;
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_unused_0;
axi_registers_ps_pl #(
                        .N_REGISTERS    (2),
                        .C_S_AXI_DATA_WIDTH(CONFIG_REG_DATA_WIDTH),
                        .C_S_AXI_ADDR_WIDTH(CONFIG_REG_ADDR_WIDTH))
                    CONFIG_REGS (
                        .data_out       ({config_reg_unused_0, config_reg_flags_0}),
                        .s_axi_aclk     (clk),
                        .s_axi_aresetn  (resetn),
                        .s_axi_awaddr   (s_axi_awaddr),
                        .s_axi_awprot   (s_axi_awprot),
                        .s_axi_awvalid  (s_axi_awvalid),
                        .s_axi_awready  (s_axi_awready),
                        .s_axi_wdata    (s_axi_wdata),
                        .s_axi_wstrb    (s_axi_wstrb),
                        .s_axi_wvalid   (s_axi_wvalid),
                        .s_axi_wready   (s_axi_wready),
                        .s_axi_bresp    (s_axi_bresp),
                        .s_axi_bvalid   (s_axi_bvalid),
                        .s_axi_bready   (s_axi_bready),
                        .s_axi_araddr   (s_axi_araddr),
                        .s_axi_arprot   (s_axi_arprot),
                        .s_axi_arvalid  (s_axi_arvalid),
                        .s_axi_arready  (s_axi_arready),
                        .s_axi_rdata    (s_axi_rdata),
                        .s_axi_rresp    (s_axi_rresp),
                        .s_axi_rvalid   (s_axi_rvalid),
                        .s_axi_rready   (s_axi_rready)
                    );

// Extract flag bits from config registers
wire mock, enable;
assign mock = config_reg_flags_0[1];
assign enable = config_reg_flags_0[0];

// FSM registers:
reg [STATE_NBITS-1 : 0] state_r, state_n;
reg [TIMESTAMP_WIDTH-1 : 0] timestamp_r, timestamp_n;

// AXI registers
reg [C_M_AXIS_TDATA_WIDTH - 1 : 0] tdata_r, tdata_n;
reg tvalid_r, tvalid_n;
reg tlast_r, tlast_n;

// SPI registers
reg spi_trx_rw_r, spi_trx_rw_n;
reg [6:0] spi_trx_addr_r, spi_trx_addr_n;
reg [7:0] spi_trx_din_r, spi_trx_din_n;
reg spi_trx_ready_r, spi_trx_ready_n;
reg spi_trx_burst_read_r, spi_trx_burst_read_n;
reg [5:0] spi_trx_burst_length_r, spi_trx_burst_length_n;

// Index registers for IMU register maps
reg [RST_REG_INDEX_NBITS-1:0] rst_reg_i_r, rst_reg_i_n;
reg [CONFIG_REG_INDEX_NBITS-1:0] cfg_reg_i_r, cfg_reg_i_n;

// Interrupt latching registers for synchronization and edge-detection
(* ASYNC_REG = "TRUE" *) reg imu_int_last_r, imu_int_last_2_r, imu_int_last_3_r;

// Sensor data registers
reg [SENSOR_DATA_WIDTH-1:0] sensor_data_arr_r [N_SENSOR_AXES-1:0],
    sensor_data_arr_n [N_SENSOR_AXES-1:0];
reg sensor_data_byte_r, sensor_data_byte_n;
reg [SENSOR_AXIS_INDEX_NBITS-1:0] sensor_axis_index_r, sensor_axis_index_n;

// Counter registers for implementing timing delays
reg [RST_COUNTER_NBITS-1:0] rst_delay_counter_r, rst_delay_counter_n;
reg [CFG_COUNTER_NBITS-1:0] cfg_delay_counter_r, cfg_delay_counter_n;

// In a fault condition, we'll want to set the fault LED
reg fault_r, fault_n;
assign fault = fault_r;

// FSM Combinational Process
always @(*)
begin

    // Registers default to previous value
    state_n = state_r;
    timestamp_n = timestamp_r;
    tdata_n = tdata_r;
    fault_n = fault_r;

    // Pulsed registers default to 0
    tvalid_n = 1'b0;
    tlast_n = 1'b0;

    // SPI transaction registers default to 0
    spi_trx_ready_n = 0;
    spi_trx_rw_n = 0;
    spi_trx_addr_n = 0;
    spi_trx_din_n = 0;
    spi_trx_burst_read_n = 0;
    spi_trx_burst_length_n = 0;

    // Counters and indices default to their previous value.
    cfg_reg_i_n = cfg_reg_i_r;
    rst_reg_i_n = rst_reg_i_r;
    rst_delay_counter_n = rst_delay_counter_r;
    cfg_delay_counter_n = cfg_delay_counter_r;
    sensor_data_byte_n = sensor_data_byte_r;
    sensor_axis_index_n = sensor_axis_index_r;
    for (i=0; i<N_SENSOR_AXES; i=i+1)
    begin
        sensor_data_arr_n[i] = sensor_data_arr_r[i];
    end

    case (state_r)
        S_RESET:
        begin
            tdata_n  = {C_M_AXIS_TDATA_WIDTH{1'b0}};
            tvalid_n = 1'b0;
            tlast_n = 1'b0;
            timestamp_n = {TIMESTAMP_WIDTH{1'b0}};

            spi_trx_rw_n = 1'b0;
            spi_trx_addr_n = 7'b0;
            spi_trx_ready_n = 1'b0;
            spi_trx_din_n = 8'b0;
            spi_trx_burst_read_n = 1'b0;
            spi_trx_burst_length_n = 6'b0;

            rst_delay_counter_n = {RST_COUNTER_NBITS{1'b0}};
            cfg_delay_counter_n =  {CFG_COUNTER_NBITS{1'b0}};

            sensor_axis_index_n = {SENSOR_AXIS_INDEX_NBITS{1'b0}};
            sensor_data_byte_n = 1'b0;

            state_n = S_DISABLED;
            fault_n = 1'b0;
        end
        // Wait for enable to be asserted
        S_DISABLED:
        begin
            rst_reg_i_n = 0; // reset counters for register maps
            cfg_reg_i_n = 0;

            if (enable && !mock) state_n = S_RST_BEGIN;
            // Don't reset or configure if using the mock IMU
            if (enable && mock) state_n = S_IDLE;
        end
        // Reset IMU.
        S_RST_BEGIN:
        begin
            spi_trx_ready_n = 1;
            spi_trx_rw_n = IMU_REG_WRITE;
            spi_trx_addr_n = IMU_RST_REG_ADDRS[7*rst_reg_i_r +: 7];
            spi_trx_din_n = IMU_RST_REG_VALS[8*rst_reg_i_r +: 8];
            rst_reg_i_n = rst_reg_i_r + 1;
            state_n = S_RST_FINISH;
        end
        S_RST_FINISH:
        begin
            if (spi_trx_done) state_n = S_RST_WAIT;
            rst_delay_counter_n = 0;
        end
        S_RST_WAIT:
        begin
            if (rst_delay_counter_r >= IMU_RST_DELAY)
            begin
                if (rst_reg_i_r == N_IMU_RST_REGS) state_n = S_CFG_BEGIN;
                else state_n = S_RST_BEGIN;
            end
            rst_delay_counter_n = rst_delay_counter_r + 1;
        end
        // Configure IMU.
        S_CFG_BEGIN:
        begin
            spi_trx_ready_n = 1;
            spi_trx_rw_n = IMU_REG_WRITE;
            spi_trx_addr_n = IMU_CONFIG_REG_ADDRS[7*cfg_reg_i_r +: 7];
            spi_trx_din_n = IMU_CONFIG_REG_VALS[8*cfg_reg_i_r +: 8];
            cfg_reg_i_n = cfg_reg_i_r + 1;
            state_n = S_CFG_FINISH;
        end
        S_CFG_FINISH:
        begin
            if (spi_trx_done) state_n = S_CFG_WAIT;
            cfg_delay_counter_n = 0;
        end
        S_CFG_WAIT:
        begin
            if (cfg_delay_counter_r >= IMU_CFG_DELAY)
            begin
                if (cfg_reg_i_r == N_IMU_CONFIG_REGS) state_n = S_WHOAMI_BEGIN;
                else state_n = S_CFG_BEGIN;
            end
            cfg_delay_counter_n = cfg_delay_counter_r + 1;
        end
        // Receive correct whoami from IMU as proof of readiness.
        S_WHOAMI_BEGIN:
        begin
            spi_trx_ready_n = 1;
            spi_trx_rw_n = IMU_REG_READ;
            spi_trx_addr_n = WHOAMI_ADDR;
            state_n = S_WHOAMI_FINISH;
        end
        S_WHOAMI_FINISH:
        begin
            if (spi_trx_done)
            begin
                if (spi_trx_dout != WHOAMI_VAL) state_n = S_FAULT;
                else state_n = S_IDLE;
            end
        end
        // Wait for the IMU to interrupt with new data
        S_IDLE:
        begin
            if (run && enable) // Wait for run and enable to be asserted.
            begin
                // Wait for a rising edge on the interrupt pin
                if (imu_int_last_2_r && !imu_int_last_3_r)
                begin
                    timestamp_n = timestamp_gen_out;
                    state_n = S_START_BURST_RX;
                end
            end
            else if (!enable) state_n = S_DISABLED;
        end
        // Set up sensor register burst read.
        // Only need to set up the first address, then
        // consecutive addresses are read as long as CS is asserted.
        S_START_BURST_RX:
        begin
            spi_trx_addr_n = IMU_ADDR_AXH;
            spi_trx_rw_n = IMU_REG_READ;
            spi_trx_ready_n = 1;
            spi_trx_burst_read_n = 1;
            spi_trx_burst_length_n = 14;

            sensor_axis_index_n = 0;
            sensor_data_byte_n = SENSOR_HIGH_BYTE;

            state_n = S_RX_SENSOR_DATA;
        end
        S_RX_SENSOR_DATA:
        begin
            if (spi_trx_done)
            begin
                sensor_data_arr_n[sensor_axis_index_r][8*sensor_data_byte_r +: 8] = spi_trx_dout;
                sensor_data_byte_n = (sensor_data_byte_r == SENSOR_HIGH_BYTE) ? SENSOR_LOW_BYTE : SENSOR_HIGH_BYTE;
                if (sensor_axis_index_r == (N_SENSOR_AXES-1) && sensor_data_byte_r == SENSOR_LOW_BYTE) state_n = S_XMIT_WORD_0;
                else if (sensor_data_byte_r == SENSOR_LOW_BYTE) sensor_axis_index_n = sensor_axis_index_r + 1;
            end
        end
        // Transmit SAMPLE_TYPE field
        S_XMIT_WORD_0:
        begin
            tdata_n = SAMPLE_TYPE;
            tvalid_n = 1'b1;

            state_n = S_XMIT_WORD_1;
        end
        // Transmit SAMPLE_SIZE field
        S_XMIT_WORD_1:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if(m_axis_tready)
            begin
                tdata_n = SAMPLE_SIZE;
                state_n = S_XMIT_WORD_2;
            end

        end
        // Transmit lower 32 bits of timestamp
        S_XMIT_WORD_2:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if(m_axis_tready)
            begin
                tdata_n = timestamp_r[31:0];

                state_n = S_XMIT_WORD_3;
            end
        end
        // Transmit upper 32 bits of timestamp
        S_XMIT_WORD_3:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if(m_axis_tready)
            begin
                tdata_n = timestamp_r[63:32];

                state_n = S_XMIT_WORD_4;
            end
        end
        // Transmit DEVICE_ID and FSYNC fields
        S_XMIT_WORD_4:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if (m_axis_tready)
            begin
                // Pad tdata to 32 bits. The fsync bit is the LSB of the temperature register.
                tdata_n = {15'b0, sensor_data_arr_r[T_AXIS_INDEX][0] , DEVICE_ID[15:0]};

                state_n = S_XMIT_WORD_5;
            end
        end
        // Transmit accelerometer x-axis and acceleromter y-axis
        S_XMIT_WORD_5:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if (m_axis_tready)
            begin
                // Concatenate ax and ay data
                tdata_n = {sensor_data_arr_r[AY_AXIS_INDEX], sensor_data_arr_r[AX_AXIS_INDEX]};

                state_n = S_XMIT_WORD_6;
            end
        end
        // Transmit accelerometer z-axis and gyroscope x-axis
        S_XMIT_WORD_6:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if (m_axis_tready)
            begin
                // Concatenate az and gx data
                tdata_n = {sensor_data_arr_r[GX_AXIS_INDEX], sensor_data_arr_r[AZ_AXIS_INDEX]};

                state_n = S_XMIT_WORD_7;
            end
        end
        // Transmit gyroscope y-axis and gyroscope z-axis
        S_XMIT_WORD_7:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if (m_axis_tready)
            begin
                // Concatenate gy and gz data
                tdata_n = {sensor_data_arr_r[GZ_AXIS_INDEX], sensor_data_arr_r[GY_AXIS_INDEX]};
                tlast_n = 1'b1;

                state_n = S_XMIT_DONE;
            end
        end
        S_XMIT_DONE:
        begin
            if (m_axis_tready)
            begin
                state_n = S_IDLE;
            end
            else
            begin
                tvalid_n = 1'b1;
                tlast_n = 1'b1;
            end
        end
        S_FAULT:
        begin
            state_n = enable ? S_FAULT : S_RESET;
            fault_n = 1'b1;
        end
        default:
        begin
            state_n = S_FAULT;
        end
    endcase

end

// FSM Sync process
integer i;
always @(posedge clk)
begin
    if (!resetn)
    begin
        state_r <= S_RESET;
        tdata_r    <= {C_M_AXIS_TDATA_WIDTH{1'b0}};
        tvalid_r <= 1'b0;
        tlast_r <= 1'b0;
        timestamp_r <= {TIMESTAMP_WIDTH{1'b0}};

        spi_trx_rw_r <= 1'b0;
        spi_trx_addr_r <= 7'b0;
        spi_trx_ready_r <= 1'b0;
        spi_trx_din_r <= 8'b0;
        spi_trx_burst_read_r <= 1'b0;
        spi_trx_burst_length_r <= 6'b0;

        imu_int_last_r <= 1'b0;
        imu_int_last_2_r <= 1'b0;
        imu_int_last_3_r <= 1'b0;

        rst_delay_counter_r <= {RST_COUNTER_NBITS{1'b0}};
        cfg_delay_counter_r <=  {CFG_COUNTER_NBITS{1'b0}};

        sensor_axis_index_r <= {SENSOR_AXIS_INDEX_NBITS{1'b0}};
        sensor_data_byte_r <= 1'b0;
        fault_r <= 1'b0;
    end
    else
    begin
        state_r <= state_n;
        tdata_r <= tdata_n;
        tvalid_r <= tvalid_n;
        tlast_r <= tlast_n;
        timestamp_r <= timestamp_n;

        cfg_reg_i_r <= cfg_reg_i_n;
        rst_reg_i_r <= rst_reg_i_n;

        spi_trx_rw_r <= spi_trx_rw_n;
        spi_trx_addr_r <= spi_trx_addr_n;
        spi_trx_ready_r <= spi_trx_ready_n;
        spi_trx_din_r <= spi_trx_din_n;
        spi_trx_burst_read_r <= spi_trx_burst_read_n;
        spi_trx_burst_length_r <= spi_trx_burst_length_n;

        imu_int_last_r <= imu_int_mxd;
        imu_int_last_2_r <= imu_int_last_r;
        imu_int_last_3_r <= imu_int_last_2_r;

        for (i=0; i<N_SENSOR_AXES; i=i+1)
        begin
            sensor_data_arr_r[i] <= sensor_data_arr_n[i];
        end

        rst_delay_counter_r <= rst_delay_counter_n;
        cfg_delay_counter_r <= cfg_delay_counter_n;

        sensor_axis_index_r <= sensor_axis_index_n;
        sensor_data_byte_r <= sensor_data_byte_n;
        fault_r <= fault_n;
    end
end

// Output assignments
assign m_axis_tdata = tdata_r;
assign m_axis_tvalid = tvalid_r;
assign m_axis_tlast = tlast_r;
assign m_axis_tstrb    = {(C_M_AXIS_TDATA_WIDTH/8){1'b1}};

endmodule
