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
de polos cada 5mm. Con 10 bits de resolución, se logró una resolución menor a 5um.

Los instrumentos utilizados en este proyecto tienen un error estadístico conjunto
de 109 um. La velocidad máxima admisible de desplazamiento del encoder es de 10m/s.

El error estadístico alcanzado en la medición es menor al de los instrumentos y,
la velocidad máxima del encoder en el proyect está limitada a 1m/s. En ambos casos,
se cumple con los requerimientos solicitados.

La presente solución es capaz de obtener la muestra, procesarla y transmitirla por el
protocolo I2C cada 50ms.

Si se utiliza la solución propuesta en el mismo hardware, a modo de no ser necesaria
 la comunicación I2C. El tiempo de muestreo y procesamiento es de 8ms.

Se emplea una aproximación polinómica de tercer orden para el cálculo de la función
arco-tangente de doble argumento que, permite obtener una mejora en tiempos de procesamiento
de 36.8 veces, con respecto a la función nativa de C, para el arco-tangente.
