onbreak {quit -f}
onerror {quit -f}

vsim -t 1ps -lib xil_defaultlib xadc_dsp_opt

do {wave.do}

view wave
view structure
view signals

do {xadc_dsp.udo}

run -all

quit -force
