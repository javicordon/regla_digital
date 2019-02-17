onbreak {quit -f}
onerror {quit -f}

vsim -t 1ps -lib xil_defaultlib arctan_0_opt

do {wave.do}

view wave
view structure
view signals

do {arctan_0.udo}

run -all

quit -force
