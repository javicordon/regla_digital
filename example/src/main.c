/*
 *    JCN Template
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <math.h>
#include <stdio.h>

#define _LPC_ADC_ID0 LPC_ADC0
#define _LPC_ADC_ID1 LPC_ADC1
#define _ADC_CHANNLE0 ADC_CH3
#define _ADC_CHANNLE1 ADC_CH1
#define _GPDMA_CONN_ADC GPDMA_CONN_ADC_0
static ADC_CLOCK_SETUP_T ADCSetup;
uint32_t DMAbuffer;
#define PI 3.14159265
#define PI_2 1.570796325
#define PI2 6.2831853

/*****************************************************************************
 * Definiciones/Enumeracion/Variables Privadas para I2C
 ****************************************************************************/
#define ACTIVO_I2C			(1)	//1 es SI, y 0 es NO en caso no se necesite enviar por I2C. Lo que da 42uS disponibles entre muestreo.
#define SPEED_100KHZ        (100000)
#define SPEED_1MHZ          (1000000)
#define I2C_ADDR_7BIT                  (0x64)
static I2CM_XFER_T  i2cmXferRec;

//Union para enviar unicamente los 3 bytes menos significativos por medio del I2C.
typedef union {
  long int    	int_var;
  char  	chx4[4];
} COMBO;

/*****************************************************************************
 * Funciones I2C
 ****************************************************************************/
/* Inicializo el Bus I2C */
static void i2c_app_init(I2C_ID_T id, int speed)
{
	Board_I2C_Init(id);
	/* Iniciar I2C */
	Chip_I2C_Init(id);
	//Debo llamar la siguiente funcion por elegir un clock rate superior a 400kHz de I2C
	Board_I2C_EnableFastPlus(id);
	//Inicio el clock
	Chip_I2C_SetClockRate(id, speed);
}

/* Funcion para configurar y ejecutar tranascion del I2C */
static void SetupXferRecAndExecute(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize)
{
	/* Configurar la transferencia I2C */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.options = 0;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;
	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);
}

/* Funcion para enviar datos procesados en CIAA por medio de I2C */
static void i2c_write(long int writeVal)
{
	COMBO   var1;
	unsigned char tx_buffer[4];
	var1.int_var=writeVal;
	/*Selecciono unicamente los 3 bits menos significativos 2^24 = 16Mn
	 * necesito 1000 mm o 1Mn de um como medida total absoluta que podra medir la regla.
	 * De este modo el valor de salida sera la posicion del sensor de la regla.
	 */
	tx_buffer[2] = var1.chx4[0];
	tx_buffer[1] = var1.chx4[1];
	tx_buffer[0] = var1.chx4[2];
	//Envio los datos.
	SetupXferRecAndExecute(I2C_ADDR_7BIT, tx_buffer, 3, NULL, 0);
}
/*DUMMY Funcion para enviar datos procesados en CIAA por medio de I2C */
static void i2c_dummy(long int writeVal)
{
	COMBO   var1;
	unsigned char tx_buffer[4];
	var1.int_var=writeVal;
	/*Selecciono unicamente los 3 bits menos significativos 2^24 = 16Mn
	 * necesito 1000 mm o 1Mn de um como medida total absoluta que podra medir la regla.
	 * De este modo el valor de salida sera la posicion del sensor de la regla.
	 */
	tx_buffer[2] = var1.chx4[0];
	tx_buffer[1] = var1.chx4[1];
	tx_buffer[0] = var1.chx4[2];
	//Envio los datos. A otra direccion, no quiero leerlo, solo que tenga los tiempos esperados
	SetupXferRecAndExecute(0X33, tx_buffer, 3, NULL, 0);
}

/*****************************************************************************
 * Configuracion de Hardware
 ****************************************************************************/
static void prvSetupHardware(void)
{
    SystemCoreClockUpdate();
    Board_Init();

    //Salidas
    Chip_SCU_PinMux(2,0,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,4); //RGB R
    Chip_SCU_PinMux(2,1,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,4); //RGB G
    Chip_SCU_PinMux(2,2,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,4); //RGB B

    Chip_SCU_PinMux(2,10,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //LED 1
    Chip_SCU_PinMux(2,11,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //LED 2
    Chip_SCU_PinMux(2,13,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //LED 3

    Chip_SCU_PinMux(6,12,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //GPIO 8

    //Entradas
    Chip_SCU_PinMux(1,0,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //TEC1
    Chip_SCU_PinMux(1,1,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //TEC2
    Chip_SCU_PinMux(1,2,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //TEC3
    Chip_SCU_PinMux(1,6,SCU_MODE_INACT|SCU_MODE_INBUFF_EN,0); //TEC4

    //MUX Salida
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,5,0); //RGB R
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,5,1); //RGB G
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,5,2); //RGB B
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0,14); //LED 1
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,1,11); //LED 2
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,1,12); //LED 3

    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,2,8); //GPI0 8

    //MUX Entrada
    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,0,4); //TEC1
    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,0,8); //TEC2
    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,0,9); //TEC3
    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,1,9); //TEC4

    //Start LED and RGB off
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,5,0,FALSE); //RGB R
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,5,1,FALSE); //RGB G
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,5,2,FALSE); //RGB B
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,0,14,FALSE); //LED 1
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,1,11,FALSE); //LED 2
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,1,12,FALSE); //LED 3
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,FALSE); //GPIO 8
}

/*****************************************************************************
 * Funciones ADC
 ****************************************************************************/
/*Configuro el ADC*/
static void prvSetupADC(void)
{
	// Variable para fijar la velocidad de muestreo de los ADC
	uint32_t _bitRate = 400000;

	//ADC 0
	Chip_ADC_Init(_LPC_ADC_ID0, &ADCSetup);
	Chip_ADC_SetSampleRate(_LPC_ADC_ID0, &ADCSetup, _bitRate);
	Chip_ADC_EnableChannel(_LPC_ADC_ID0, _ADC_CHANNLE0, ENABLE);
	//ADC 1
	Chip_ADC_Init(_LPC_ADC_ID1, &ADCSetup);
	Chip_ADC_SetSampleRate(_LPC_ADC_ID1, &ADCSetup, _bitRate);
	Chip_ADC_EnableChannel(_LPC_ADC_ID1, _ADC_CHANNLE1, ENABLE);
}

/*****************************************************************************
 * Funciones Publicas
 ****************************************************************************/

/*
 * Aproximacion polinomial de arcotangente cuando el valor esta dentro de [-1,1]
 * Error maximo < 0.005 (o 0.29 grados).
 * Esta aproximacion unicamente mide angulos en cuadrante I y III
 */
float ApproxAtan(float z)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    return (n1 + n2 * z * z) * z;
}

/*
 * Calculo de Arcotangente utilizando aproximacion polinomial considerando todos los
 * cuadrantes I,II,II,IV. El metodo a implementar es el de arctan2.
 * Fuente: https://en.wikipedia.org/wiki/Atan2
 */
float ApproxAtan2(float y, float x)
{
    if (x != 0.0f)
    {
        if (fabsf(x) > fabsf(y))
        {
            const float z = y / x;
            if (x > 0.0)
            {
                // atan2(y,x) = atan(y/x) if x > 0
                return ApproxAtan(z);
            }
            else if (y >= 0.0)
            {
                // atan2(y,x) = atan(y/x) + PI if x < 0, y >= 0
                return ApproxAtan(z) + PI;
            }
            else
            {
                // atan2(y,x) = atan(y/x) - PI if x < 0, y < 0
                return ApproxAtan(z) - PI;
            }
        }
        else // Uso de propiedad trigonometrica atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
        {
            const float z = x / y;
            if (y > 0.0)
            {
                // atan2(y,x) = PI/2 - atan(x/y) if |y/x| > 1, y > 0
                return -ApproxAtan(z) + PI_2;
            }
            else
            {
                // atan2(y,x) = -PI/2 - atan(x/y) if |y/x| > 1, y < 0
                return -ApproxAtan(z) - PI_2;
            }
        }
    }
    else
    {
        if (y > 0.0f) // x = 0, y > 0
        {
            return PI_2;
        }
        else if (y < 0.0f) // x = 0, y < 0
        {
            return -PI_2;
        }
    }
    return 0.0f; // x,y = 0
}

static void arctan(uint16_t si, uint16_t co, uint16_t* pos_1b,int32_t* pos_2,uint16_t* promed,int32_t* med5f,int32_t* med5)
{
    float y, x, res;
    int32_t pos_1;
    y=si-*promed;
    x=co-*promed;

//Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,TRUE); //GPIO 8
    res = ApproxAtan2(y,x);
    //res= atan2(y,x);
    //En vez de cuantificar el arcTangente entre +/- PI, se cuantifica de 0 a 2PI
    (res<0)?res+=PI2:res;
    res = res*180000/PI; //Conversion de radianes a grados por mil para tener mejor resolucion

//Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,FALSE); //GPIO 8

    /*
     * Identifico en que cuadrante me encuentro I,II,III, IV
     * segun el valor de ADC (Seno y Coseno)
     */
    if(si>*promed&&co>*promed) {
        pos_1=0;
    }
    if(si>*promed&&co<=*promed) {
        pos_1=1;
    }
    if(si<=*promed&&co<=*promed) {
        pos_1=2;
    }
    if(si<=*promed&&co>*promed) {
        pos_1=3;
    }

    /*
     * Verifico si realizo un cambio de cuadrante de IV a I o de IV a I
     * para poder sumar un periodo a la medicion (5mm de periodo entre polos
     * magneticos de la regla digital como periodo)
     * pos_2 registra el cuadrante actual acumulado y pos_1b el cuadrante del ciclo anterior.
     * 	Es decir, pos_2 mide el periodo de 5mm en cuatro, teniendo una resolucion de 1.25mm
     * 	sin necesidad del calculo del arc-tan. Arc-tan nos podra dar una resolucion de 8um sin
     * 	el uso de i2c o de 50um con uso de i2c. El error estadistico maximo de la regla es 100um.
     */
    if((*pos_1b==3)&&(pos_1==0)) {
        *pos_2=*pos_2+1;
        *pos_1b=pos_1;
    };
    if((*pos_1b==0)&&(pos_1==3)) {
        *pos_2=*pos_2-1;
        *pos_1b=pos_1;
    };
    *med5f=res*5000/360000;

    if(*pos_2>=0) *med5=*pos_2*5000+*med5f;
        else *med5=*pos_2*5000+*med5f-5000;

    //printf(" Sin:%i\r\n Cos:%i\r\n ArcTan:%i\r\n",seno,coseno,res);
    //printf("%f\t%f\t%f\t%i\t%i\t%i\t%i\t%i\r\n",y,x,res,pos_1,*pos_2,*promed,*med5f,*med5);
#ifdef ACTIVO_I2C
    //Envio por I2C el valor de la posicion calculado. A un SLAVE con direccion 0X64
    //i2c_write(si);
    i2c_write(*med5);
    //i2c_dummy(*med5);
    //vTaskDelay(1/portTICK_RATE_MS);
#endif

    *pos_1b=pos_1; //Registro de posicion del cuadrante anterior

    /* Delay */
    //vTaskDelay(10/portTICK_RATE_MS);
}
#define BUFF (512)
/*
 * Tarea principal para muestreo de dos senales por dos ADC de CIAA,
 *  posterior calculo del arco-tangente de las senales y eventual envio
 *  por I2C de ser necesario.
 */
static void adc_task(void *pvParameters) {
    int max=511,min=511;
    uint16_t dataADC0,dataADC1;
    int32_t dummy=0;
    int32_t* med5 = (int32_t*)calloc(1, sizeof(int32_t));
    int32_t* pos_abs = (int32_t*)calloc(BUFF, sizeof(int32_t));
    int32_t* sin_mem = (int32_t*)calloc(BUFF, sizeof(int32_t));
    int32_t* cos_mem = (int32_t*)calloc(BUFF, sizeof(int32_t));
    int32_t* med5f = (int32_t*)calloc(1, sizeof(int32_t));
    int32_t* medq5 = (int32_t*)calloc(1, sizeof(int32_t));
    uint16_t* pos_1b = (uint16_t*)calloc(1, sizeof(uint16_t));
    uint16_t* promed = (uint16_t*)calloc(1, sizeof(uint16_t));
    //Init I2C
	SystemCoreClockUpdate();
	Board_Init();
	//i2c_app_init(I2C0, SPEED_100KHZ);
	i2c_app_init(I2C0, SPEED_1MHZ);
	int cortar = 0;

        while (1) {
            /* Empiezo conversion A/D solo si no hay modo Burst */
			Chip_ADC_SetStartMode(_LPC_ADC_ID0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
			Chip_ADC_SetStartMode(_LPC_ADC_ID1, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

			//Empiezo conversion A/D
Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,TRUE); //GPIO 8 Para medir Tiempo efectivo entre muestras (frecuencia de muestreo efectiva)
			/* Espera de conversion A/D */
			while (Chip_ADC_ReadStatus(_LPC_ADC_ID0, _ADC_CHANNLE0, ADC_DR_DONE_STAT) != SET &&Chip_ADC_ReadStatus(_LPC_ADC_ID1, _ADC_CHANNLE1, ADC_DR_DONE_STAT) != SET) {}
			/* Leo valor ADC 0*/
			Chip_ADC_ReadValue(_LPC_ADC_ID0, _ADC_CHANNLE0, &dataADC0);
			/* Leo valor ADC 1*/
			Chip_ADC_ReadValue(_LPC_ADC_ID1, _ADC_CHANNLE1, &dataADC1);

			/* Calculo el valor medio de la senal como autocalibracion y calculo de arctan de las senales */
			if(dataADC0>max) max=dataADC0;
			if(dataADC0<min) min=dataADC0;
			*promed=max+min;
			*promed=(*promed)/2;
			arctan(dataADC0,dataADC1, pos_1b,medq5,promed,med5f,med5);
			pos_abs[cortar]=*med5;
			sin_mem[cortar]=dataADC0;
			cos_mem[cortar]=dataADC1;
Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,FALSE); //GPIO 8
            //vTaskDelay(100/portTICK_RATE_MS);

			//Imprimir datos guardados en memoria
/*
			if(cortar++>=BUFF){
				for(long int i = 0; i<BUFF;i++){
					//printf("%i\n",pos_abs[i]);
					i2c_write(pos_abs[i]);
					//i2c_write(i);
					//printf("%i ",i);
					vTaskDelay(1/portTICK_RATE_MS);
				}
				vTaskDelay(1000/portTICK_RATE_MS);
				for(int i = 0; i<BUFF;i++){
					//printf("%i\n",cos_mem[i]);
					//vTaskDelay(10/portTICK_RATE_MS);
					i2c_write(cos_mem[i]);
					//i2c_write(i);
					vTaskDelay(1/portTICK_RATE_MS);
				}
				vTaskDelay(1000/portTICK_RATE_MS);
				for(int i = 0; i<BUFF;i++){
					//printf("%i\n",sin_mem[i]);
					//vTaskDelay(10/portTICK_RATE_MS);
					i2c_write(sin_mem[i]);
					//i2c_write(i);
					vTaskDelay(1/portTICK_RATE_MS);
				}


				//printf("\n\n");
				vTaskDelay(1000/portTICK_RATE_MS);
				cortar=0;
				//vTaskDelete(NULL);
			}
*/

        }
}

/*****************************************************************************
 * Main principal
 ****************************************************************************/
int main(void)
{
	prvSetupHardware();
	prvSetupADC();

    xTaskCreate(adc_task, "adc_task", 1024, NULL, (tskIDLE_PRIORITY + 4UL), (TaskHandle_t *) NULL);
    /* Inicio scheduler */
    vTaskStartScheduler();

    return 1;
}
