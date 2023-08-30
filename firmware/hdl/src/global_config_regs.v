
`timescale 1 ns / 1 ps

`include "version.v"

module global_config_regs #(
           parameter integer CONFIG_REG_DATA_WIDTH = 32,
           parameter integer CONFIG_REG_ADDR_WIDTH = 4
       )
       (
           // AXI Slave port
           input wire s_axi_aclk,
           input wire s_axi_aresetn,
           input wire [CONFIG_REG_ADDR_WIDTH-1 : 0] s_axi_awaddr,
           input wire [2 : 0] s_axi_awprot,
           input wire s_axi_awvalid,
           output wire s_axi_awready,
           input wire [CONFIG_REG_DATA_WIDTH-1 : 0] s_axi_wdata,
           input wire [(CONFIG_REG_DATA_WIDTH/8)-1 : 0] s_axi_wstrb,
           input wire s_axi_wvalid,
           output wire s_axi_wready,
           output wire [1 : 0] s_axi_bresp,
           output wire s_axi_bvalid,
           input wire s_axi_bready,
           input wire [CONFIG_REG_ADDR_WIDTH-1 : 0] s_axi_araddr,
           input wire [2 : 0] s_axi_arprot,
           input wire s_axi_arvalid,
           output wire s_axi_arready,
           output wire [CONFIG_REG_DATA_WIDTH-1 : 0] s_axi_rdata,
           output wire [1 : 0] s_axi_rresp,
           output wire s_axi_rvalid,
           input wire s_axi_rready,

           // Config outputs
           output wire run
       );

localparam unsigned N_REGISTERS	= 3;

// Instantiation of module axi_registers_ps_pl
wire [(CONFIG_REG_DATA_WIDTH * N_REGISTERS)-1 : 0] data_in;
wire [(CONFIG_REG_DATA_WIDTH * N_REGISTERS)-1 : 0] data_out;
axi_registers #(
                        .N_REGISTERS(N_REGISTERS),
                        .C_S_AXI_DATA_WIDTH(CONFIG_REG_DATA_WIDTH),
                        .C_S_AXI_ADDR_WIDTH(CONFIG_REG_ADDR_WIDTH)
                    ) CONFIG_REGS (
                        .data_in        (data_in),
                        .data_out       (data_out),
                        .s_axi_aclk     (s_axi_aclk),
                        .s_axi_aresetn  (s_axi_aresetn),
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
                        .s_axi_rready   (s_axi_rready));

wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_flags_0;
wire [CONFIG_REG_DATA_WIDTH-1 : 0] config_reg_flags_1;

// Mux register values into data_in
assign data_in[1*CONFIG_REG_DATA_WIDTH - 1 -: CONFIG_REG_DATA_WIDTH] = config_reg_flags_0;
assign data_in[2*CONFIG_REG_DATA_WIDTH - 1 -: CONFIG_REG_DATA_WIDTH] = config_reg_flags_1;
assign data_in[3*CONFIG_REG_DATA_WIDTH - 1 -: CONFIG_REG_DATA_WIDTH] = `VERSION;

// Demux register values from data_out
assign config_reg_flags_0 = data_out[1*CONFIG_REG_DATA_WIDTH - 1 -: CONFIG_REG_DATA_WIDTH];
assign config_reg_flags_1 = data_out[2*CONFIG_REG_DATA_WIDTH - 1 -: CONFIG_REG_DATA_WIDTH];

// Output assignments
assign run	 = config_reg_flags_0[0];

endmodule
