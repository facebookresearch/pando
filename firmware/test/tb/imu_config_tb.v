`timescale 1ns / 1ns

module imu_config_tb;
`include "tb_clock.v"

reg resetn;
reg clk;
reg run;

localparam TDATA_WIDTH = 32;
localparam SAMPLE_TYPE = 32'h53500001;
localparam SAMPLE_SIZE = 32'h20;
localparam N_SENSOR_TRIGGERS = 5;
localparam DEVICE_ID =16'b1;

wire m_axis_tvalid;
wire [TDATA_WIDTH-1:0] m_axis_tdata;
wire m_axis_tlast;
reg m_axis_tready;

reg imu_int;
reg imu_spi_miso;
wire imu_spi_clk;
wire imu_spi_mosi;
wire imu_spi_cs;

reg sample_trigger;

// Instantiate the imu module
imu #(.TIMESTAMP_WIDTH(64),
      .C_M_AXIS_TDATA_WIDTH(TDATA_WIDTH),
      .CONFIG_REG_DATA_WIDTH(32),
      .CONFIG_REG_ADDR_WIDTH(3),
      .DEVICE_ID(DEVICE_ID)) DUT0(
        .resetn(resetn),
        .clk(clk),
        .run(run),
        .sample_trigger(sample_trigger),
        .m_axis_tvalid(m_axis_tvalid),
        .m_axis_tdata(m_axis_tdata),
        .m_axis_tlast(m_axis_tlast),
        .m_axis_tready(m_axis_tready),
        .imu_int(imu_int),
        .m_spi_miso(imu_spi_miso),
        .m_spi_mosi(imu_spi_mosi),
        .m_spi_cs(imu_spi_cs),
        .m_spi_clk(imu_spi_clk)
    );


reg [15:0] txd_val;
reg [3:0] bit_counter;
reg imu_spi_clk_prev;
reg display;
reg [3:0] tx_count;

always @(posedge clk)
begin
    imu_spi_clk_prev <= imu_spi_clk;
    if (DUT0.enable && !imu_spi_cs && imu_spi_clk && !imu_spi_clk_prev)
    begin
        txd_val <= (txd_val << 1) + imu_spi_mosi;
        bit_counter <= bit_counter + 1;
        display <= 1;
    end
    else if (bit_counter == 0 && display)
    begin
        display <= 0;
        tx_count <= tx_count + 1;
        if (tx_count >= 2 && tx_count < DUT0.N_IMU_CONFIG_REGS+2)
        begin
            if (txd_val[15:8] != {1'b0,DUT0.IMU_CONFIG_REG_ADDRS[(tx_count-2)*7 +: 7]} ||
                    txd_val[7:0] != DUT0.IMU_CONFIG_REG_VALS[(tx_count-2)*8 +: 8])
            begin
                $display("t:%4d FAIL actual addr: %x expected addr: %x   actual value %x expected value %x",
                         $time,
                         txd_val[15:8],
                         {1'b0,DUT0.IMU_CONFIG_REG_ADDRS[(tx_count-2)*7 +: 7]},
                         txd_val[7:0],
                         DUT0.IMU_CONFIG_REG_VALS[(tx_count-2)*8 +: 8]);
            end
            else $display("t:%4d Register %x configured correctly!", $time, txd_val[15:8]);
        end
        else if (tx_count < 2)
        begin
            if (txd_val[15:8] != {1'b0,DUT0.IMU_RST_REG_ADDRS[(tx_count)*7 +: 7]} ||
                    txd_val[7:0] != DUT0.IMU_RST_REG_VALS[(tx_count)*8 +: 8])
            begin
                $display("t:%4d FAIL actual addr: %x expected addr: %x   actual value %x expected value %x",
                         $time,
                         txd_val[15:8],
                         {1'b0,DUT0.IMU_RST_REG_ADDRS[(tx_count)*7 +: 7]},
                         txd_val[7:0],
                         DUT0.IMU_RST_REG_VALS[(tx_count)*8 +: 8]);
            end
            else $display("t:%4d Register %x reset correctly!", $time, txd_val[15:8]);
        end
        else $display("t:%4d Received %x from IMU", $time, txd_val[31:0]);
    end
end

initial
begin
    // Set up the vcd dumps
    $dumpfile("imu_config_tb.vcd");
    $dumpvars(0, imu_config_tb);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    m_axis_tready <= 1;
    txd_val <= 0;
    bit_counter <= 0;
    tx_count <= 0;

    force DUT0.enable = 1'b0;
    force DUT0.mock = 1'b0;
    // Wait a few clock cycles before asserting the reset line
    wait_cycles(3);

    // Assert the reset signal for a few clock cycles
    resetn <= 0;
    wait_cycles(3);

    // Deassert reset
    resetn <= 1;
    wait_cycles(3);

    // Enable the IMU to trigger configuration.
    force DUT0.enable = 1'b1;

    $display("Starting the test...");

    wait_cycles(20080000);
    $display("Configuration done.");
    $display("Ending the test...");
    $finish;
end

endmodule
