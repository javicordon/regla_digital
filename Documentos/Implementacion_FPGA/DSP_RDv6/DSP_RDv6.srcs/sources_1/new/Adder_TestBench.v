`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/09/2018 12:22:26 AM
// Design Name: 
// Module Name: Adder_TestBench
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


module Adder_TestBench();
  reg [31:0] A_t, B_t;
  wire [31:0] S_t;
  
  Adder32 Adder32_0(A_t,B_t,S_t);
  
  //Test vectors *A good testbench would have many more)
  initial  begin
    A_t <= 5;
    B_t <= 10;
    #1;
    
    A_t <= 50;
    B_t <= 7;
    #1;
        
    A_t <= 0;
    B_t <= 0;
    #1;
    
    A_t <= 1000000;
    B_t <= 34991;
    #1;
  end
endmodule
