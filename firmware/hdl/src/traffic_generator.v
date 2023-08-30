/* traffic_generator.v
 *
 * This is a simple peripheral that periodically emits 64 bit wide timestamps
 * at a configurable rate. It is meant to be used as an early development aid.
 */

`timescale 1 ns / 1 ps

module traffic_generator #(
           parameter DEVICE_ID = 0,

           parameter TIMESTAMP_WIDTH = 64,

           parameter integer C_M_AXIS_TDATA_WIDTH	= 32,
           parameter integer CONFIG_REG_DATA_WIDTH = 32,
           parameter integer CONFIG_REG_ADDR_WIDTH = 3
       )
       (
           input wire resetn,
           input wire clk,
           input wire run,

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
localparam S_IDLE = 1;
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

// Constants
localparam SAMPLE_TYPE = 32'h5350_0003;
localparam SAMPLE_SIZE = 32'h14;

// Instantiation of module axi_registers_ps_pl
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_flags_0;
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_clk_div;
axi_registers_ps_pl #(
                        .N_REGISTERS    (2),
                        .C_S_AXI_DATA_WIDTH(CONFIG_REG_DATA_WIDTH),
                        .C_S_AXI_ADDR_WIDTH(CONFIG_REG_ADDR_WIDTH))
                    CONFIG_REGS (
                        .data_out       ({config_reg_clk_div, config_reg_flags_0}),
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
wire enable;
assign enable = config_reg_flags_0[0];

// Instantiation of module pulse_gen (generates sample_trig)
wire sample_trig;
pulse_gen #(
              .COUNT_WIDTH(CONFIG_REG_DATA_WIDTH))
          SAMPLE_TRIG_GEN (
              .resetn(resetn),
              .run(run),
              .clk(clk),
              .period(config_reg_clk_div),
              .width(1),
              .pulse_out(sample_trig)
          );

// FSM registers
reg [STATE_NBITS-1 : 0] state_r, state_n;
reg [C_M_AXIS_TDATA_WIDTH - 1 : 0] tdata_r, tdata_n;
reg tvalid_r, tvalid_n;
reg tlast_r, tlast_n;
reg [TIMESTAMP_WIDTH-1 : 0] timestamp_r, timestamp_n;

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
    fault_n = fault_r;

    // Pulsed registers default to 0
    tvalid_n = 1'b0;
    tlast_n = 1'b0;

    case (state_r)
        S_RESET:
        begin
            tdata_n = {C_M_AXIS_TDATA_WIDTH{1'b0}};
            tvalid_n = 1'b0;
            tlast_n = 1'b0;

            state_n = S_IDLE;
            timestamp_n = {TIMESTAMP_WIDTH{1'b0}};
            fault_n = 1'b0;
        end

        // Wait for sample_trig pulse
        S_IDLE:
        begin
            if (sample_trig && run && enable)
            begin
                timestamp_n = timestamp_gen_out;
                state_n = S_XMIT_WORD_0;
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

        // Transmit DEVICE_ID & DIRECTION fields
        S_XMIT_WORD_4:
        begin
            tvalid_n = 1'b1;

            // Stall until previous transaction completes
            if(m_axis_tready)
            begin
                tdata_n = {16'b0, DEVICE_ID};
                tlast_n = 1'b1;

                state_n = S_XMIT_DONE;
            end
        end

        S_XMIT_DONE:
        begin
            // Stall until previous transaction completes
            if(m_axis_tready)
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
always @(posedge clk)
begin
    if (!resetn)
    begin
        state_r <= S_RESET;
        tdata_r    <= {C_M_AXIS_TDATA_WIDTH{1'b0}};
        tvalid_r <= 1'b0;
        tlast_r <= 1'b0;
        timestamp_r <= {TIMESTAMP_WIDTH{1'b0}};
        fault_r <= 1'b0;
    end
    else
    begin
        state_r <= state_n;
        tdata_r <= tdata_n;
        tvalid_r <= tvalid_n;
        tlast_r <= tlast_n;
        timestamp_r <= timestamp_n;
        fault_r <= fault_n;
    end
end

// Output assignments
assign m_axis_tdata = tdata_r;
assign m_axis_tvalid = tvalid_r;
assign m_axis_tlast = tlast_r;
assign m_axis_tstrb	= {(C_M_AXIS_TDATA_WIDTH/8){1'b1}};

endmodule
