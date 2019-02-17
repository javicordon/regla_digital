# regla_digital_FPGA_

Proyecto Final - UNSAM
Creado por: Javier Cordon
Hardware utilizado: Arty S7 / DIGILENT / XILINX
Hardware destinado: Spartan - 7

Descripción:
Implementación de adquisición de dos señales en paralelo y sincronizadas (al mismo tiempo)
con los dos ADC disponibles en ARTY S7, adquiriendo la señal seno y coseno, para luego
ser procesada y obtener el arco-tangente entre las dos, logrando un procesamiento total
cercano a 1Mhz de muestreo efectivo (adquisición y proceso).

Pendiente a implementar un protocolo para transimitir la señal a un micro-controlador.
