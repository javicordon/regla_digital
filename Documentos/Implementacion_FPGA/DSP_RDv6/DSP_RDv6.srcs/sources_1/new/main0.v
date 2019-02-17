`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/09/2018 01:53:53 AM
// Design Name: 
// Module Name: main0
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module main0(input CLK100MHZ, output led0, output led1, input btn0,input vp_in, input vn_in,  input vaux1_p, input vaux1_n, input vaux9_p, input vaux9_n);
    wire [24:0] S_m;
    blinky blinky_m(
    .clk    (CLK100MHZ),
    .led    (led0),
    .S      (S_m)
    );
    
    wire [15:0] VAUXP, VAUXN;
    wire [15:0] MEASURED_A;
    wire [15:0] MEASURED_B;
    wire DONE_T;
    
    XADC_DSP xadc_dsp_m(
    //.VAUXP (VAUXP),
    //.VAUXN (VAUXN),
    .VP  (vp_in),
    .VN  (vn_in),
    .vaux1_p (vaux1_p),
    .vaux1_n (vaux1_n),
    .vaux9_p (vaux9_p),
    .vaux9_n (vaux9_n),
    .RESET  (btn0),
    .DCLK   (CLK100MHZ),
    .MEASURED_A    (MEASURED_A),
    .MEASURED_B     (MEASURED_B),
    .FIN_ADC (DONE_T),
    .LED (led1)     
    );
    
    //always @(posedge(CLK100MHZ)) ;
    
endmodule
