/* axi_registers_ps_pl.v
 * 
 * This is an axi slave peripheral that implements a configurable number of
 * memory mapped registers. The register values are concatenated together and
 * made available for reading on the data_out port. The name of the module
 * refers to the fact that the registers are written by the PS and read by the
 * PL.
 */


`timescale 1 ns / 1 ps

module axi_registers_ps_pl #
       (
           // Users to add parameters here

           // Number of registers
           parameter unsigned N_REGISTERS	= 16,

           // User parameters ends
           // Do not modify the parameters beyond this line

           // Width of S_AXI data bus
           parameter integer C_S_AXI_DATA_WIDTH	= 32,
           // Width of S_AXI address bus
           parameter integer C_S_AXI_ADDR_WIDTH	= 6
       )(
           // Users to add ports here
           output wire [(C_S_AXI_DATA_WIDTH * N_REGISTERS)-1 : 0] data_out,

           // User ports ends
           // Do not modify the ports beyond this line

           // Ports of Axi Slave Bus Interface
           input wire  s_axi_aclk,
           input wire  s_axi_aresetn,
           input wire [C_S_AXI_ADDR_WIDTH-1 : 0] s_axi_awaddr,
           input wire [2 : 0] s_axi_awprot,
           input wire  s_axi_awvalid,
           output wire  s_axi_awready,
           input wire [C_S_AXI_DATA_WIDTH-1 : 0] s_axi_wdata,
           input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] s_axi_wstrb,
           input wire  s_axi_wvalid,
           output wire  s_axi_wready,
           output wire [1 : 0] s_axi_bresp,
           output wire  s_axi_bvalid,
           input wire  s_axi_bready,
           input wire [C_S_AXI_ADDR_WIDTH-1 : 0] s_axi_araddr,
           input wire [2 : 0] s_axi_arprot,
           input wire  s_axi_arvalid,
           output wire  s_axi_arready,
           output wire [C_S_AXI_DATA_WIDTH-1 : 0] s_axi_rdata,
           output wire [1 : 0] s_axi_rresp,
           output wire  s_axi_rvalid,
           input wire  s_axi_rready
	);
// Instantiation of axi_registers module
	axi_registers # ( 
        .N_REGISTERS(N_REGISTERS),
		.C_S_AXI_DATA_WIDTH(C_S_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S_AXI_ADDR_WIDTH)
	) axi_registers_inst (
        .data_in(data_out),  // Note: feedback of data_out into data_in
        .data_out(data_out),
		.s_axi_aclk(s_axi_aclk),
		.s_axi_aresetn(s_axi_aresetn),
		.s_axi_awaddr(s_axi_awaddr),
		.s_axi_awprot(s_axi_awprot),
		.s_axi_awvalid(s_axi_awvalid),
		.s_axi_awready(s_axi_awready),
		.s_axi_wdata(s_axi_wdata),
		.s_axi_wstrb(s_axi_wstrb),
		.s_axi_wvalid(s_axi_wvalid),
		.s_axi_wready(s_axi_wready),
		.s_axi_bresp(s_axi_bresp),
		.s_axi_bvalid(s_axi_bvalid),
		.s_axi_bready(s_axi_bready),
		.s_axi_araddr(s_axi_araddr),
		.s_axi_arprot(s_axi_arprot),
		.s_axi_arvalid(s_axi_arvalid),
		.s_axi_arready(s_axi_arready),
		.s_axi_rdata(s_axi_rdata),
		.s_axi_rresp(s_axi_rresp),
		.s_axi_rvalid(s_axi_rvalid),
		.s_axi_rready(s_axi_rready)
	);

// Add user logic here

// User logic ends

endmodule
