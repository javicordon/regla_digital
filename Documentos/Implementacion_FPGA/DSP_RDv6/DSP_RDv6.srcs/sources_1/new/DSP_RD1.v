`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/12/2018 04:00:26 PM
// Design Name: 
// Module Name: DSP_RD1
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


module DSP_RD1(clk, dsp_sin_in, dsp_cos_in, enable, arctan_out, i_ready, o_ready, count,position,position_phase);
    input clk;
    input [11:0] dsp_sin_in;
    input [11:0] dsp_cos_in;
    input enable;
    output wire [15:0] arctan_out;
    output o_ready;
    output i_ready;
    output wire [1:0] count;
    output wire [31:0] position;
    output wire [15:0] position_phase;
    
    
   reg [15:0] dsp_phi_s;    //Sin - 2048 (signed [-2048,2047]) 16bit signed
   reg [15:0] dsp_phi_c;    //Cos - 2048 (signed [-2048,2047]) 16bit signed
   wire [31:0] dsp_phi;    //Concatena Sin y Cos para calculo de ArcTan
   
   reg [1:0] position_p;
   reg [15:0] position_f = 16'b0;
   
   reg [1:0] buff_pos_p;
   reg [15:0] buff_pos_f;
   reg [15:0] buff_pos_f2;
   reg done_assign = 0;
   
   //wire [15:0] dsp_phi_y;   //Divide por 2048 para ArcTan(Y/X) normalizado
   //wire [15:0] dsp_phi_x;   //Divide por 2048 para ArcTan(Y/X) normalizado
    
   assign dsp_phi [31:19]= dsp_phi_s [12:0];    //Concatena Seno y hace el shift equivalente a division
   assign dsp_phi [15:3]= dsp_phi_c [12:0];     //Concatena Coseno y hace el shift equivalente a division
   assign count [1:0]= position_p [1:0];     //
   assign position [31:16]= position_f [15:0];     //
   assign position [15:0]= buff_pos_f2 [15:0];     //
   assign position_phase [15:0] = buff_pos_f [15:0];
   
   //assign dsp_phi_y [15:0]= dsp_phi [31:16];    //Division por 2048 es shift de 3 bits a la izquierda
   //assign dsp_phi_x [15:0]= dsp_phi [15:0];     //Division por 2048 es shift de 3 bits a la izquierda
   
   //Arctan definitions
   wire [31:0] dsp_cartesian_tdata;
   wire arctan_dout_tvalid;
   assign dsp_cartesian_tdata[31:0] = dsp_phi [31:0];
    
     arctan_0 dsp_arctan (
     .aclk(clk),                                            // input wire aclk
     .s_axis_cartesian_tvalid(enable),                      // input wire s_axis_cartesian_tvalid
     .s_axis_cartesian_tready(i_ready),                     // output wire s_axis_cartesian_tready
     .s_axis_cartesian_tdata(dsp_cartesian_tdata),          // input wire [31 : 0] s_axis_cartesian_tdata
     .m_axis_dout_tvalid(o_ready),                          // output wire m_axis_dout_tvalid
     .m_axis_dout_tdata(arctan_out)                         // output wire [15 : 0] m_axis_dout_tdata
    );
    
    always @ (posedge(clk)) begin
        dsp_phi_s <= dsp_sin_in - 16'd2048;
        dsp_phi_c <= dsp_cos_in - 16'd2048;
        
        if (o_ready == 1'b1) begin
            buff_pos_p <= position_p; // Guarda posicion anterior
            if (dsp_cos_in >= 16'd2048 && dsp_sin_in >= 16'd2048) begin
                 if(arctan_out>16'd52000) begin
                   position_p <= 2'd0;
                   buff_pos_f <= arctan_out+16'h2e;
                 end else begin
                    position_p <= 2'd0;
                    buff_pos_f <= arctan_out+16'b0;
                 end
            end else if (dsp_cos_in < 16'd2048 && dsp_sin_in >= 16'd2048) begin
                if(arctan_out>16'd25732) begin
                    position_p <= 2'd1;
                    //buff_pos_f <= arctan_out+32'b1111_1111_1111_1111_0110_0100_1000_0010; //+3.140869140625 para que sea cero
                    buff_pos_f <= arctan_out+16'b1100_1001_0000_0100; //3.140869140625*2
                end else begin
                    position_p <= 2'd1;
                    buff_pos_f <= arctan_out+16'b0;
                end
            end else if (dsp_cos_in < 16'd2048 && dsp_sin_in < 16'd2048) begin
                if(arctan_out>16'd25732) begin
                    position_p <= 2'd2;
                    //buff_pos_f <= arctan_out+32'b1111_1111_1111_1111_0110_0100_1000_0010; //+3.140869140625 para que sea cero
                    buff_pos_f <= arctan_out+16'b1100_1001_0000_0100; //3.140869140625*2
                end else begin
                    position_p <= 2'd2;
                    buff_pos_f <= arctan_out+16'b0;
                end
            end else if (dsp_cos_in >= 16'd2048 && dsp_sin_in < 16'd2048) begin
                position_p <= 2'd3;
                buff_pos_f <= arctan_out+16'b1100_1001_0000_0100; //3.140869140625*2
            end
            done_assign <= 1;
        end
        
        if (done_assign==1) begin
            buff_pos_f2 <=buff_pos_f;
            if (buff_pos_p == 2'd3 && position_p == 2'd0) begin
               position_f[15:0]<=position_f[15:0]+1;
            end
            
            if (buff_pos_p == 2'd0 && position_p == 2'd3) begin
               position_f[15:0]<=position_f[15:0]-1;        
            end
            done_assign=0;
        end
         
    end
endmodule
