/* adc.v
 *
 * This peripheral acquires data from a MAXREFDES11 SPI ADC board.
 *
 * FIXME: This is just a stub.
 */

`timescale 1 ns / 1 ps

module adc #(
           parameter TIMESTAMP_WIDTH = 64,

           parameter integer C_M_AXIS_TDATA_WIDTH	= 32,
           parameter integer CONFIG_REG_DATA_WIDTH = 32,
           parameter integer CONFIG_REG_ADDR_WIDTH = 3,

           parameter integer DEVICE_ID = 0
       )
       (
           input wire resetn,
           input wire clk,
           input wire run,

           // SPI Master interface
           output wire m_spi_clk,
           input wire m_spi_miso,
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



// Instantiation of module timestamp_generator
wire [TIMESTAMP_WIDTH-1 : 0] timestamp_gen_out;
timestamp_generator TIMESTAMP_GENERATOR (
                        .resetn     (resetn),
                        .clk        (clk),
                        .run        (run),
                        .timestamp  (timestamp_gen_out));

// Enumeration of FSM states
localparam S_RESET = 0;
localparam S_WAIT = 1;
localparam S_XMIT_WORD_0 = 2;
localparam S_XMIT_WORD_1 = 3;
localparam S_XMIT_WORD_2 = 4;
localparam S_XMIT_WORD_3 = 5;
localparam S_XMIT_WORD_4 = 6;
localparam S_XMIT_DONE = 7;
localparam S_FAULT = 8;
localparam STATE_COUNT = 9;


// Derived params
localparam STATE_NBITS = $clog2(STATE_COUNT); // # of bits in the state register

// Instantiation of module axi_registers_ps_pl
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_flags_0;
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_samp_rate_div;

axi_registers_ps_pl #(
                        .N_REGISTERS    (2),
                        .C_S_AXI_DATA_WIDTH(CONFIG_REG_DATA_WIDTH),
                        .C_S_AXI_ADDR_WIDTH(CONFIG_REG_ADDR_WIDTH))
                    CONFIG_REGS (
                        .data_out       ({config_reg_samp_rate_div, config_reg_flags_0}),
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

// ADC data and control signals
reg trigger;
wire [15:0] adc_data_out;
wire        adc_data_ready;
reg [CONFIG_REG_DATA_WIDTH-1:0] adc_div_cnt;
reg [15:0] mock_data_cnt_r, mock_data_cnt_n;


// FSM registers
reg [STATE_NBITS-1 : 0] state_r, state_n;
reg [C_M_AXIS_TDATA_WIDTH - 1 : 0] tdata_r, tdata_n;
reg tvalid_r, tvalid_n;
reg tlast_r, tlast_n;
reg [TIMESTAMP_WIDTH-1 : 0] timestamp_r, timestamp_n;
reg [15:0] adc_data_r, adc_data_n;

// In a fault condition, we'll want to set the fault LED
reg fault_r, fault_n;
assign fault = fault_r;

// FSM Combinational Process
always @(*)
begin

    // Registers default to previous value
    state_n = state_r;
    tdata_n = tdata_r;
    timestamp_n = timestamp_r;
    adc_data_n = adc_data_r;
    mock_data_cnt_n = mock_data_cnt_r;
    fault_n = fault_r;

    // Pulsed registers default to 0
    tvalid_n = 1'b0;
    tlast_n = 1'b0;

    case (state_r)
        S_RESET:
        begin
            timestamp_n = 0;
            tdata_n = {C_M_AXIS_TDATA_WIDTH{1'b0}};
            tvalid_n = 1'b0;
            tlast_n = 1'b0;
            adc_data_n = 16'b0;
            mock_data_cnt_n = 0;
            state_n = S_WAIT;
	    fault_n = 1'b0;
        end

        S_WAIT:
        begin
            if (run && enable && adc_data_ready)
            begin
                timestamp_n = timestamp_gen_out;
                adc_data_n = adc_data_out;
                state_n = S_XMIT_WORD_0;
            end
        end

        // Transmit the Sample Type field
        S_XMIT_WORD_0:
        begin
            tvalid_n = 1;
            tdata_n = 32'h5350_0002;
            state_n = S_XMIT_WORD_1;
        end

        // Transmit the Sample Size Field
        S_XMIT_WORD_1:
        begin
            tvalid_n = 1;

            if (m_axis_tready)
            begin
                tdata_n = 32'h0000_0014;
                state_n = S_XMIT_WORD_2;
            end
        end

        // Transmit the lower 32 bits of timestamp
        S_XMIT_WORD_2:
        begin
            tvalid_n = 1;

            if (m_axis_tready)
            begin
                tdata_n = timestamp_r[31:0];
                state_n = S_XMIT_WORD_3;
            end
        end

        // Transmit upper 32 bits of timestamp
        S_XMIT_WORD_3:
        begin
            tvalid_n = 1;

            if (m_axis_tready)
            begin
                tdata_n = timestamp_r[63:32];
                state_n = S_XMIT_WORD_4;
            end
        end

        // Transmit device ID and the sample value
        S_XMIT_WORD_4:
        begin
            tvalid_n = 1;

            if (m_axis_tready)
            begin
                tlast_n = 1;

                if (mock)
                begin
                    tdata_n = {mock_data_cnt_r, DEVICE_ID[15:0]};
                    mock_data_cnt_n = mock_data_cnt_r + 1;
                end
                else
                begin
                    tdata_n = {adc_data_r, DEVICE_ID[15:0]};
                end

                state_n = S_XMIT_DONE;
            end
        end

        S_XMIT_DONE:
        begin
            if(m_axis_tready)
            begin
                state_n = S_WAIT;
            end
            else
            begin
                tvalid_n = 1;
                tlast_n = 1;
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

    // Detect dropped samples (can happen if m_axis_tready is deasserted)
    if (adc_data_ready && (state_r != S_WAIT))
        state_n = S_FAULT;

end

// FSM Sync process
always @(posedge clk)
begin
    if (!resetn)
    begin
        state_r <= S_RESET;
        tdata_r <= {C_M_AXIS_TDATA_WIDTH{1'b0}};
        tvalid_r <= 1'b0;
        tlast_r <= 1'b0;
        timestamp_r <= {TIMESTAMP_WIDTH{1'b0}};
        adc_data_r <= 16'b0;
        mock_data_cnt_r <= 0;
        fault_r <= 1'b0;
    end
    else
    begin
        state_r <= state_n;
        fault_r <= fault_n;
        tdata_r <= tdata_n;
        tvalid_r <= tvalid_n;
        tlast_r <= tlast_n;
        timestamp_r <= timestamp_n;
        adc_data_r <= adc_data_n;
        mock_data_cnt_r <= mock_data_cnt_n;
    end
end

// Output assignments
assign m_axis_tdata = tdata_r;
assign m_axis_tvalid = tvalid_r;
assign m_axis_tlast = tlast_r;
assign m_axis_tstrb	= {(C_M_AXIS_TDATA_WIDTH/8){1'b1}};


// MAX11100 Instantiation
max11100 max11100_0(
             .resetn(resetn),
             .clk(clk),
             .trigger(trigger),
             .data(adc_data_out),
             .data_ready(adc_data_ready),
             .spi_miso(m_spi_miso),
             .spi_sclk(m_spi_clk),
             .spi_cs(m_spi_cs)
         );

// Generate the ADC sample trigger pulses
always @(posedge clk)
begin
    if (!resetn || !enable)
    begin
        adc_div_cnt <= 0;
        trigger <= 0;
    end
    else
    begin
        if (enable && run && (config_reg_samp_rate_div >= 1000))
        begin
            if (adc_div_cnt == 0)
            begin
                adc_div_cnt <= config_reg_samp_rate_div - 1;
                trigger <= 1;
            end
            else
            begin
                adc_div_cnt <= adc_div_cnt - 1;
                trigger <= 0;
            end
        end
    end
end


endmodule
