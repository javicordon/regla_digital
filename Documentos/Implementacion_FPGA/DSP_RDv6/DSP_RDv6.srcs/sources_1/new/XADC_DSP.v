`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/26/2018 04:37:53 PM
// Design Name: 
// Module Name: XADC_DSP
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


module XADC_DSP(DCLK, RESET, VAUXP, VAUXN,vaux1_p,vaux1_n,vaux9_p,vaux9_n, VP,VN, MEASURED_A, MEASURED_B, CHANNEL, XADC_EOC,XADC_EOS, FIN_ADC, LED    );
input DCLK;     //Clock de entrada para DRP
input RESET;
input [15:0] VAUXP, VAUXN; //Entrada auxiliar del canal analogico a muestrear
input vaux1_p, vaux1_n, vaux9_p, vaux9_n;
input VP, VN;   // Para de entrada de ardware analogico y dedicado

output reg[15:0] MEASURED_A, MEASURED_B;

output wire [4:0] CHANNEL;
output wire XADC_EOC;
output wire XADC_EOS;
output wire FIN_ADC;
output reg LED;

//DEFINICIONES INTERNAS
wire busy;
wire [0:5] channel;
wire drdy;
reg [6:0] daddr;
reg [15:0] di_drp;
wire [15:0] do_drp;
wire [15:0] vauxp_active;
wire [15:0] vauxn_active;
wire EOC,EOS;

reg [1:0] den_reg;
reg [1:0] dwe_reg;
reg done;

/*
wire vauxp0 = vaux0_p; //Pin A0
wire vauxn0 = vaux0_p; //Pin A0
wire vauxp1 = vaux1_p; //Pin A1
wire vauxn1 = vaux1_p; //Pin A2
*/

assign XADC_EOC = EOC;
assign XADC_EOS = EOS;
assign FIN_ADC = done;

parameter   init_read        = 8'h00,
            read_waitdrdy    = 8'h01,
            write_waitdrdy   = 8'h02,
            read_reg00       = 8'h03,
            reg00_waitdrdy   = 8'h04,
            read_reg01       = 8'h05,
            reg01_waitdrdy   = 8'h06;

reg [7:0] state = init_read;

always @(posedge DCLK) begin
    LED <= 1'b0;
    if (RESET) begin
      state   <= init_read;
      den_reg <= 2'h0;
      dwe_reg <= 2'h0;
      di_drp  <= 16'h0000;
      LED <= 1'b1;
    end
    else
      case (state)
      init_read : begin
        daddr = 7'h40;
         den_reg = 2'h2; // performing read
         if (busy == 0 ) state <= read_waitdrdy;
         end
      read_waitdrdy :
         if (drdy ==1)  begin
            //di_drp = do_drp  & 16'h03_FF; //Clearing AVG bits for Configreg0
            di_drp = 16'h8000; //Disable AVG
            daddr = 7'h40;
            den_reg = 2'h2;
            dwe_reg = 2'h2; // performing write
            state = write_waitdrdy;
         end
         else begin
            den_reg = { 1'b0, den_reg[1] } ;
            dwe_reg = { 1'b0, dwe_reg[1] } ;
            state = state;
         end
      write_waitdrdy :
        if (drdy ==1) begin
            state = read_reg00;
              end
           else begin
              den_reg = { 1'b0, den_reg[1] } ;
              dwe_reg = { 1'b0, dwe_reg[1] } ;
              state = state;
           end
        read_reg00 : begin
           daddr   = 7'h10; //10
           den_reg = 2'h2; // performing read
           done=0;
           if (EOS == 1) state   <=reg00_waitdrdy;
           end
        reg00_waitdrdy :
           if (drdy ==1)  begin
              MEASURED_A = do_drp;
              state <=read_reg01;
              end
           else begin
              den_reg = { 1'b0, den_reg[1] } ;
              dwe_reg = { 1'b0, dwe_reg[1] } ;
              state = state;
           end
        read_reg01 : begin
           daddr   = 7'h18; //18
           den_reg = 2'h2; // performing read
           state   <=reg01_waitdrdy;
           end
           reg01_waitdrdy :
           if (drdy ==1)  begin
              MEASURED_B = do_drp;
              state <=read_reg00;
              daddr   = 7'h10;
              done = 1;
              end
           else begin
              den_reg = { 1'b0, den_reg[1] } ;
              dwe_reg = { 1'b0, dwe_reg[1] } ;
              state = state;
              done = 0;
      end endcase
end
    
xadc_dsp xadc_dual (
  .di_in(di_drp),              // input wire [15 : 0] di_in
  .daddr_in(daddr),        // input wire [6 : 0] daddr_in
  .den_in(den_reg[0]),            // input wire den_in
  .dwe_in(dwe_reg[0]),            // input wire dwe_in
  .drdy_out(drdy),        // output wire drdy_out
  .do_out(do_drp),            // output wire [15 : 0] do_out
  .dclk_in(DCLK),          // input wire dclk_in
  .reset_in(RESET),        // input wire reset_in
  .vp_in(VP),              // input wire vp_in
  .vn_in(VN),              // input wire vn_in
  .vauxp1(vaux1_p),            // input wire vauxp0
  .vauxn1(vaux1_n),            // input wire vauxn0
  .vauxp9(vaux9_p),            // input wire vauxp8
  .vauxn9(vaux9_n),            // input wire vauxn8
  .channel_out(),  // output wire [4 : 0] channel_out
  .eoc_out(EOC),          // output wire eoc_out
  .alarm_out(),      // output wire alarm_out
  .eos_out(EOS),          // output wire eos_out
  .busy_out(busy)        // output wire busy_out
);





endmodule
