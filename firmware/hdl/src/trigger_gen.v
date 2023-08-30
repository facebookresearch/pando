/* trigger_gen.v
 *
 * This peripheral generates trigger signal with configurable period and pulse width.
 *
 */

`timescale 1 ns / 1 ps

module trigger_gen #(
           parameter integer CONFIG_REG_DATA_WIDTH = 32,
           parameter integer CONFIG_REG_ADDR_WIDTH = 4,

           parameter integer DEVICE_ID = 0
       )
       (
           input wire resetn,
           input wire clk,
           input wire run,

           output wire trigger,

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

// Instantiation of module axi_registers_ps_pl
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_flags_0;
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_trigger_period;
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_trigger_width;


axi_registers_ps_pl #(
                        .N_REGISTERS    (3),
                        .C_S_AXI_DATA_WIDTH(CONFIG_REG_DATA_WIDTH),
                        .C_S_AXI_ADDR_WIDTH(CONFIG_REG_ADDR_WIDTH))
                    CONFIG_REGS (
                        .data_out       ({config_reg_trigger_width, config_reg_trigger_period, config_reg_flags_0}),
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

// Instantiation of module pulse_gen
pulse_gen #(
              .COUNT_WIDTH(CONFIG_REG_DATA_WIDTH))
          FRAME_TRIG_GEN (
              .resetn(resetn),
              .run(run && enable),
              .clk(clk),
              .period(config_reg_trigger_period),
              .width(config_reg_trigger_width),
              .pulse_out(trigger)
          );


endmodule
