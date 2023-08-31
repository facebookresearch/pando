`timescale 1ns / 1ns

module imu_mock_tb;
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


reg [2:0] sample_index;

/* The mock IMU updates its sensor values each time it gets a sample_trigger.
 * The value of gx increases by one each sample_trigger and the other values
 * gy, gz, ax, ay, az are set to the value of gx plus one, two, three, four
 * and five respectively. So, for example, the first sample trigger should yield:
 * gx=1, gy=2, gz=3, ax=4, ax=5, ax=6 and the second sample trigger should yield:
 * gx=2, gy=3, gz=4, ax=5, ay=6, az=7. 
 */
reg [15:0] gx_expected_value;

always @(posedge clk)
begin
    if (m_axis_tvalid)
    begin
        $display("t:%4d run=%d tlast=%d tdata=0x%x", $time, run, m_axis_tlast, m_axis_tdata);

        sample_index <= sample_index + 1;
        case (sample_index)
            0: // sample type
            begin
                if (m_axis_tdata != SAMPLE_TYPE)
                    $display("\tFAIL! Got sample_type=0x%x, exampled sample_type=0x%x.", m_axis_tdata, SAMPLE_TYPE);
                else
                    $display("\tReceived correct sample_type.");
            end
            1: // sample size
            begin
                if (m_axis_tdata != SAMPLE_SIZE)
                    $display("\tFAIL! Got sample_size=0x%x, exampled sample_size=0x%x.", m_axis_tdata, SAMPLE_SIZE);
                else
                    $display("\tReceived correct sample_size.");
            end
            2: // timestamp low
            begin
                $display("\tReceived timestamp low bits.");
            end
            3: // timestamp high
            begin
                $display("\tReceived timestamp high bits.");
            end
            4: // device id
            begin
                if (m_axis_tdata[31:16] != 0 || m_axis_tdata[15:0] != DEVICE_ID)
                    $display("\tFAIL! Device ID incorrect. Got 0x%x, expected 0x%x.", m_axis_tdata, {16'b0, DEVICE_ID});
                else
                    $display("\tReceived correct device ID.");
            end
            5: // accel x / accel y
            begin
                if (m_axis_tdata[15:0] != gx_expected_value + 3)
                    $display("\tFAIL! Got accel_x=0x%x, expected accel_x=0x%x.",m_axis_tdata[15:0], 4);
                else
                    $display("\tReceived correct accel_x.");
                if (m_axis_tdata[31:16] != gx_expected_value + 4)
                    $display("\tFAIL! Got accel_y=0x%x, expected accel_y=0x%x.",m_axis_tdata[31:16], 5);
                else
                    $display("\tReceived correct accel_y.");
            end
            6: // accel z / gyro x
            begin
                if (m_axis_tdata[15:0] != gx_expected_value + 5)
                    $display("\tFAIL! Got accel_z=0x%x, expected accel_z=0x%x.",m_axis_tdata[15:0], 6);
                else
                    $display("\tReceived correct accel_z.");
                if (m_axis_tdata[31:16] != gx_expected_value)
                    $display("\tFAIL! Got gyro_x=0x%x, expected gyro_x=0x%x.",m_axis_tdata[31:16], 1);
                else
                    $display("\tReceived correct gyro_x.");
            end
            7: // gyro y / gyro z
            begin
                if (m_axis_tdata[15:0] != gx_expected_value + 1)
                    $display("\tFAIL! Got gyro_y=0x%x, expected gyro_y=0x%x.",m_axis_tdata[15:0], 2);
                else
                    $display("\tReceived correct gyro_y.");
                if (m_axis_tdata[31:16] != gx_expected_value + 2)
                    $display("\tFAIL! Got gyro_z=0x%x, expected gyro_z=0x%x.",m_axis_tdata[31:16], 3);
                else
                    $display("\tReceived correct gyro_z.");
            end
            default:
                $display("\tFAIL! Shouldn't hit default case.");
        endcase
    end
end


initial
begin
    // Set up the vcd dumps
    $dumpfile("imu_mock_tb.vcd");
    $dumpvars(0, imu_mock_tb);

    // Set up initial conditions
    resetn <= 1;
    run <= 0;
    clk <= 0;
    m_axis_tready <= 1;
    gx_expected_value <= 0;

    //sample_index_high <= 31;
    //sample_index_low <= 0;
    sample_index <= 0;

    // Force the configuration register bit that ungates the run signal
    force DUT0.enable = 1'b1;
    force DUT0.mock = 1'b1;
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
    wait_cycles(10);

    repeat(N_SENSOR_TRIGGERS)
    begin
        // trigger an update to the mock IMU sensor registers.
        // this should cause the IMU module to start receiving sensor data.
        gx_expected_value <= gx_expected_value + 1;
        $display("Triggering new sensor sample...");
        sample_trigger <= 1;
        wait_cycles(1);
        sample_trigger <= 0;
        wait_cycles(25000);
    end

    $display("Ending the test...");
    $finish;
end

endmodule
