`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/08/2018 10:50:12 PM
// Design Name: 
// Module Name: blinky
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

module blinky(clk, led, S);
    input clk;
    output reg led;
    //CLK100MHZ
    output reg [24:0] S = 0;
    
    always @ (posedge(clk)) begin
        S <= S + 1;
        led <= S[24];
    end
endmodule
