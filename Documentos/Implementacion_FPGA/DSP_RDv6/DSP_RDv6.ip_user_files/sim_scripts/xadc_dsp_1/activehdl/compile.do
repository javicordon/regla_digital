vlib work
vlib activehdl

vlib activehdl/xil_defaultlib

vmap xil_defaultlib activehdl/xil_defaultlib

vlog -work xil_defaultlib  -v2k5 \
"../../../../DSP_RDv6.srcs/sources_1/ip/xadc_dsp_1/xadc_dsp.v" \


vlog -work xil_defaultlib \
"glbl.v"

