# regla_digital

Proyecto Final - UNSAM
Creado por: Javier Cordon
Hardware utilizado: EDU-CIAA
Hardware destinado: CIAA

Descripción:
El firmware del proyecto está destinado para un microcontorlador LPC 4337 de
una EDU-CIAA o CIAA.
Se utilizan dos de sus entradas analógicas para ser procesadas por la función
arco-tangente y así poder determinar la posición en que se encuentra.

Las dos señales pueden ser provistas por dos generadores de funciones sinusoidales
desplazada 90 grados una de otra. Al procesar el arco-tangente entre ambas señales,
se obtiene una función lineal de mismo período que la señal de entrada.
De esta manera se puede determinar con alta precisión la posición en que se
encuentra entre cada período.

La resolución del ADC es de 10 bits, por lo que dependerá de la amplitud de la
señal de entrada, para determinar la verdadera resolución obtenida.

Para este proyecto se utilza una regla digital, la cual tiene colocado cada par
de polos cada 5mm. Con 10 bits de resolución, puede alcanzar 5mm/1024 o aprox.
4.88um.

La señal origen a muestrear debe tener una frecuencia menor a la frecuencia de
muestreo efectiva de la CIAA.

En nuestro experimento con la regla digital se encuentra lejos de obtener esta
resolución teórica. La velocidad de desplazamiento máximo de la regla es de 1m/s.

debido al muestreo de dos entradas no síncronas (primero muestrea señal A y luego
  señal B) Además de los tiempos que le toma al programa en procesar la señal adquirida.
  
La presente solución es capaz de obtener la muestra, procesarla y enviarla por el 
protocolo I2C cada 50ms. 

Si se utiliza la solución propuesta en el mismo hardware a modo de no ser necesaria
 la comunicación I2C. El tiempo de muestreo y procesamiento es de 8ms.
