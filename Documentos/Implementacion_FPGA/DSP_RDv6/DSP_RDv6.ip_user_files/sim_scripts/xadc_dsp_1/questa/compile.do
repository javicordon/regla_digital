vlib questa_lib/work
vlib questa_lib/msim

vlib questa_lib/msim/xil_defaultlib

vmap xil_defaultlib questa_lib/msim/xil_defaultlib

vlog -work xil_defaultlib -64 \
"../../../../DSP_RDv6.srcs/sources_1/ip/xadc_dsp_1/xadc_dsp.v" \


vlog -work xil_defaultlib \
"glbl.v"

