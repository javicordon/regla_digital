`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/09/2018 12:57:16 AM
// Design Name: 
// Module Name: blinky_TestBench
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


module blinky_TestBench();
    reg clk_t;
    wire led_t;
    wire [24:0] SS_t;
    
    reg [31:0] A_t,B_t;
    wire [31:0] copy_t;
    wire [31:0] S_t;
    
    /*
    //Sine definitions
    reg phase_tvalid;
    wire phase_tready;
    reg [15:0] tdata;
    wire dout_tvalid;
    wire [31:0] dout_tdata;
    wire [15:0] x_out;
    wire [15:0] y_out;
    assign x_out = dout_tdata[15:0];
    assign y_out = dout_tdata[31:16];
    */
    
    //ADC Simulation
    reg [11:0] sin_in;  //Senal muestreada Seno 12bits
    reg [11:0] cos_in;  //Senal muestreada Coseno 12 bits unsigned
    
    //DSP Definitions
    wire [15:0] arc_out_t;
    wire o_ready_t;
    wire i_ready_t;
    reg dsp_enable;
    
    blinky blinky_0(
    .clk    (clk_t),
    .led    (led_t),
    .S      (SS_t)
    );
    //Same as before but needs to be in modules declaration order
    //blinky blinky_0(clk_t,led_t,SS_t);
    Adder32 Adder32_0(A_t,B_t,S_t);
    
    wire [1:0] count_t;
    wire [31:0] position_t;
    wire [15:0] position_phase_t;
    wire [11:0] measure_a_t;
    wire [11:0] measure_b_t;
    
    DSP_RD1 my_dsp_rd1(
        .clk (clk_t), 
        .dsp_sin_in (sin_in), //sin_in
        .dsp_cos_in (cos_in), //cos_in
        .enable (dsp_enable), 
        .arctan_out (arc_out_t), 
        .i_ready (i_ready_t), 
        .o_ready (o_ready_t),
        .count (count_t),
        .position (position_t),
        .position_phase (position_phase_t)
    );
    
    //Start XADC test bench
    reg [15:0]   VAUXP, VAUXN;
    reg         RESET;
    //reg         DCLK; clk_t
    wire [15:0] MEASURED_A, MEASURED_B;
    wire DONE_T;
    
    assign measure_a_t =  MEASURED_A [15:4];
    assign measure_b_t =  MEASURED_B [15:4];
    
    //Unidad bajo prueb (Unit Under Test: UUT)
    XADC_DSP my_xadc_dsp(
        .VAUXP  (VAUXP),
        .VAUXN  (VAUXN),
        .RESET  (RESET),
        .DCLK   (clk_t),
        .MEASURED_A    (MEASURED_A),
        .MEASURED_B     (MEASURED_B),
        .FIN_ADC (DONE_T)     
    );
    
    initial begin
        clk_t=0;
        
        A_t <= 0;
        B_t <= 0;
        dsp_enable=0;
        
                RESET = 1;
        #100    RESET = 0;
        $display("Hello");
        
        
        /*
        #10;
        //dsp_enable=1;
      
        #350 sin_in <= 12'd2048; cos_in <= 12'd4095; 
        #400 sin_in <= 12'd2054; cos_in <= 12'd4095;$display("%f", $itor(arc_out_t)*(2.0**-13));
        #400 sin_in <= 12'd2061; cos_in <= 12'd4095;$display("%f", $itor(arc_out_t)*(2.0**-13));
        #400 sin_in <= 12'd2067; cos_in <= 12'd4095;$display("%f", $itor(arc_out_t)*(2.0**-13));
        
        #400 sin_in <= 12'd2073; cos_in <= 12'd4095;$display("%f", $itor(arc_out_t)*(2.0**-13));
        
        */
        //#200;
        //$display("Sum is hex: %h, dec: %d, bin: %b", S_t, S_t, S_t);
        
    end
    
    always begin
     #5 clk_t = clk_t+1;
    end   
    
    always @ (posedge(clk_t)) begin
    A_t <= A_t + 1;
    B_t <= 0;
    dsp_enable=0;
    
       if (S_t == 32'd40) begin
        //sin_in = sin_in+1;
        //cos_in = cos_in+1;
        A_t <= 32'd0;
        //dsp_enable=1;
       end
       
       if (DONE_T == 1) begin
        sin_in <= MEASURED_A [15:4]; 
        cos_in <= MEASURED_B [15:4];
        dsp_enable=1;
       end     
       
    end
    
endmodule
