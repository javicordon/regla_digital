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
static volatile uint8_t Burst_Mode_Flag = 0, Sample_Time_sec = 10;
static volatile uint8_t channelTC, dmaChannelNum;
uint32_t DMAbuffer;
#define PI 3.14159265
#define PI_2 1.570796325
#define PI2 6.2831853


/*****************************************************************************
 * Semaphores Definition
 ****************************************************************************/
//xSemaphoreHandle SEM1, SEM2;

/*****************************************************************************
 * Queue Definition
 ****************************************************************************/
xQueueHandle Time;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
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
 * Public functions
 ****************************************************************************/
/* Print ADC value and delay */
static void App_print_ADC_value(uint16_t si, uint16_t co, int cont,uint16_t* pos_1b,int32_t* pos_2,uint16_t* promed,int32_t* med5f,int32_t* med5)
{
    //Imprime valor ADC de 0 a 1023 niveles por los 10 bit
    //DEBUGOUT("%04d\r\n", data);
    //Realiza la conversion de niveles a 3.3V

    float_t tan_yx;
    float_t y, x, resf;
    int32_t res,seno,coseno,tantan, pos_1;
    y=si-*promed;
    seno=y;
    //y=y/512;
    x=co-*promed;
    coseno=x;
    //x=co/512;
    //if(x==0) x=1;
    tan_yx=y/x;

    resf= atan(tan_yx)*180000/PI;//Uso 180 000, en vez de 180, para incluir los decimales en entero
    tantan=tan_yx*1000;
    res=resf;



    if(si>*promed&&co>*promed) {
        pos_1=0;
        *med5f=res;
        if(res<-1) *med5f=res+180000;
    }
    if(si>*promed&&co<=*promed) {
        pos_1=1;
        if(res<-1) *med5f=res+180000;
    }
    if(si<=*promed&&co<=*promed) {
        pos_1=2;
        *med5f=res+180000;
        if(res<-1) *med5f=res+360000;
    }
    if(si<=*promed&&co>*promed) {
        pos_1=3;
        *med5f=res+360000;
    }

    if((*pos_1b==3)&&(pos_1==0)) {
        *pos_2=*pos_2+1;
        *pos_1b=pos_1;
    };
    if((*pos_1b==0)&&(pos_1==3)) {
        *pos_2=*pos_2-1;
        *pos_1b=pos_1;
    };

    *med5f=(*med5f)*5000/360000;

    if(*pos_2>=0) *med5=*pos_2*5000+*med5f;
        else *med5=*pos_2*5000+*med5f-5000;


    //result = 2*3.1415;
    //result = cos(1.5708);
    //result=data*3.3/1023;
    //DEBUGOUT("%04d\r\n", result);
    //DEBUGOUT("%f\r\n", result);

    //printf(" Sin:%i\r\n Cos:%i\r\n ArcTan:%i\r\n",seno,coseno,res);
    printf("%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\r\n",seno,coseno,res,tantan,pos_1,*pos_2,*promed,*med5f,*med5);
    *pos_1b=pos_1;

    /* Delay */
    vTaskDelay((cont)/portTICK_RATE_MS);
}


// Aproximacion polinomial de arcotangente entre -1,1.
// Error maximo < 0.005 (o 0.29 grados)
float ApproxAtan(float z)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    return (n1 + n2 * z * z) * z;
}

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
        else // Use property atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
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
    return 0.0f; // x,y = 0. Could return NaN instead.
}

static void arctan(uint16_t si, uint16_t co, int cont,uint16_t* pos_1b,int32_t* pos_2,uint16_t* promed,int32_t* med5f,int32_t* med5)
{
    //Imprime valor ADC de 0 a 1023 niveles por los 10 bit
    //DEBUGOUT("%04d\r\n", data);
    //Realiza la conversion de niveles a 3.3V

    float tan_yx;
    float y, x, res, resf,tantan;
    int32_t seno,coseno, pos_1;
    y=si-*promed;
    seno=y;
    //y=y/512;
    x=co-*promed;
    coseno=x;
    //x=co/512;
    //if(x==0) x=1;
    tan_yx=y/x;

//Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,TRUE); //GPIO 8
    //res= atan(tan_yx)*180000/PI;//Uso 180 000, en vez de 180, para incluir los decimales en entero
	//res= atan2(y,x)*180000/PI;//Uso 180 000, en vez de 180, para incluir los decimales en entero
	//res = 0.9724*tan_yx-0.1919*tan_yx*tan_yx*tan_yx;
    res = ApproxAtan2(y,x);
    (res<0)?res+=PI2:res;
    res = res*180000/PI;

//Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,FALSE); //GPIO 8

    //tantan=tan_yx*1000;
    //res=resf;

    if(si>*promed&&co>*promed) {
        pos_1=0;
        //*med5f=res;
        //if(res<-1) *med5f=res+180000;
    }
    if(si>*promed&&co<=*promed) {
        pos_1=1;
        //if(res<-1) *med5f=res+180000;
    }
    if(si<=*promed&&co<=*promed) {
        pos_1=2;
        //*med5f=res+180000;
        //if(res<-1) *med5f=res+360000;
    }
    if(si<=*promed&&co>*promed) {
        pos_1=3;
        //*med5f=res+360000;
    }


    if((*pos_1b==3)&&(pos_1==0)) {
        *pos_2=*pos_2+1;
        *pos_1b=pos_1;
    };
    if((*pos_1b==0)&&(pos_1==3)) {
        *pos_2=*pos_2-1;
        *pos_1b=pos_1;
    };

    //*med5f=(*med5f)*5000/360000;
    *med5f=res*5000/360000;

    if(*pos_2>=0) *med5=*pos_2*5000+*med5f;
        else *med5=*pos_2*5000+*med5f-5000;

    //result = 2*3.1415;
    //result = cos(1.5708);
    //result=data*3.3/1023;
    //DEBUGOUT("%04d\r\n", result);
    //DEBUGOUT("%f\r\n", result);

    //printf(" Sin:%i\r\n Cos:%i\r\n ArcTan:%i\r\n",seno,coseno,res);
    //printf("%f\t%f\t%f\t%i\t%i\t%i\t%i\t%i\r\n",y,x,res,pos_1,*pos_2,*promed,*med5f,*med5);
    *pos_1b=pos_1;

    /* Delay */
    //vTaskDelay((cont)/portTICK_RATE_MS);
}


static void vLectorTEC1(void *pvParameters) {
    int count=0,cont=500;

    while (1) {
        //Verifica si la tecla 1 es presionada
        if (Chip_GPIO_GetPinState(LPC_GPIO_PORT,0,4)==FALSE){
            if(count==1){
                //Al terminar el tiempo de muestreo apaga el LED 3 Verde
             Chip_GPIO_SetPinState(LPC_GPIO_PORT,1,12,FALSE); //LED 3
             count=0;

            } else {
            //Al presionar la tecla 1 se preden LED 3 Verde
             Chip_GPIO_SetPinState(LPC_GPIO_PORT,1,12,TRUE); //LED 3
             count+=1;
            }
             vTaskDelay(300/portTICK_RATE_MS);
        }
           xQueueSendToBack(Time,&count,portMAX_DELAY);
           vTaskDelay(100/portTICK_RATE_MS);
           //Libera cola Time
           xQueueReceive (Time,&count,portMAX_DELAY);
    }
}
static void adc_tec(void *pvParameters) {
    int cont=5, count=0, max=511,min=511;
    uint16_t dataADC0,dataADC1;
    int32_t* med5 = (int32_t*)calloc(1, sizeof(int32_t));
    int32_t* med5f = (int32_t*)calloc(1, sizeof(int32_t));
    uint32_t* medq5 = (uint32_t*)calloc(1, sizeof(uint32_t));
    uint16_t* pos_1b = (uint16_t*)calloc(1, sizeof(uint16_t));
    uint16_t* promed = (uint16_t*)calloc(1, sizeof(uint16_t));
        while (1) {
            //Verifico si hay algo en cola de Time pero no lo libero
            xQueuePeek (Time,&count,portMAX_DELAY);
            //Si envie 1 fue porque active TEC1
            /* Empiezo conversion A/D solo si no hay modo Burst */
            if (!Burst_Mode_Flag) {
                Chip_ADC_SetStartMode(_LPC_ADC_ID0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
                Chip_ADC_SetStartMode(_LPC_ADC_ID1, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
            }
            if (count==1){
                //Empiezo conversion A/D mientras estoy con TEC 1 Activa

                    /* Espera de conversion A/D */
                    while (Chip_ADC_ReadStatus(_LPC_ADC_ID0, _ADC_CHANNLE0, ADC_DR_DONE_STAT) != SET &&Chip_ADC_ReadStatus(_LPC_ADC_ID1, _ADC_CHANNLE1, ADC_DR_DONE_STAT) != SET) {}
                    /* Leo valor ADC */
                    Chip_ADC_ReadValue(_LPC_ADC_ID0, _ADC_CHANNLE0, &dataADC0);
                    /* Leo valor ADC */
                    Chip_ADC_ReadValue(_LPC_ADC_ID1, _ADC_CHANNLE1, &dataADC1);
                    /* Imprimo valor ADC */
                    if(dataADC0>max) max=dataADC0;
                    if(dataADC0<min) min=dataADC0;
                    *promed=max+min;
                    *promed=(*promed)/2;
                    App_print_ADC_value(dataADC0,dataADC1, 5,pos_1b,medq5,promed,med5f,med5);

            }
            //Si envie 0 no he presionado TEC1, no hago nada.
            if (count==0){
            	vTaskDelay(10/portTICK_RATE_MS);
            }

            //vTaskDelay(10/portTICK_RATE_MS);
        }
}

static void adc_task(void *pvParameters) {
    int cont=5, count=0, max=511,min=511;
    uint16_t dataADC0,dataADC1;
    int32_t* med5 = (int32_t*)calloc(1, sizeof(int32_t));
    int32_t* med5f = (int32_t*)calloc(1, sizeof(int32_t));
    uint32_t* medq5 = (uint32_t*)calloc(1, sizeof(uint32_t));
    uint16_t* pos_1b = (uint16_t*)calloc(1, sizeof(uint16_t));
    uint16_t* promed = (uint16_t*)calloc(1, sizeof(uint16_t));
        while (1) {
            /* Empiezo conversion A/D solo si no hay modo Burst */
			Chip_ADC_SetStartMode(_LPC_ADC_ID0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
			Chip_ADC_SetStartMode(_LPC_ADC_ID1, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

			//Empiezo conversion A/D
Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,TRUE); //GPIO 8
			/* Espera de conversion A/D */
			while (Chip_ADC_ReadStatus(_LPC_ADC_ID0, _ADC_CHANNLE0, ADC_DR_DONE_STAT) != SET &&Chip_ADC_ReadStatus(_LPC_ADC_ID1, _ADC_CHANNLE1, ADC_DR_DONE_STAT) != SET) {}
			/* Leo valor ADC */
			Chip_ADC_ReadValue(_LPC_ADC_ID0, _ADC_CHANNLE0, &dataADC0);
			/* Leo valor ADC */
			Chip_ADC_ReadValue(_LPC_ADC_ID1, _ADC_CHANNLE1, &dataADC1);


			/* Imprimo valor ADC */
			if(dataADC0>max) max=dataADC0;
			if(dataADC0<min) min=dataADC0;
			*promed=max+min;
			*promed=(*promed)/2;
			arctan(dataADC0,dataADC1, 5,pos_1b,medq5,promed,med5f,med5);
Chip_GPIO_SetPinState(LPC_GPIO_PORT,2,8,FALSE); //GPIO 8
            //vTaskDelay(10/portTICK_RATE_MS);
        }
}



/**
 * @
 */
int main(void)
{
    uint32_t _bitRate = 400000; // Variable para fijar la velocidad de muestreo


    prvSetupHardware();
    Chip_ADC_Init(_LPC_ADC_ID0, &ADCSetup);
    Chip_ADC_SetSampleRate(_LPC_ADC_ID0, &ADCSetup, _bitRate);
    Chip_ADC_EnableChannel(_LPC_ADC_ID0, _ADC_CHANNLE0, ENABLE);

    Chip_ADC_Init(_LPC_ADC_ID1, &ADCSetup);
    Chip_ADC_SetSampleRate(_LPC_ADC_ID1, &ADCSetup, _bitRate);
    Chip_ADC_EnableChannel(_LPC_ADC_ID1, _ADC_CHANNLE1, ENABLE);

    /*Queue creation*/
    Time = xQueueCreate (1,sizeof (unsigned int));


    /* TASK creation xTaskCreate (Puntero, String descriptivo, Tamaño del Stack, parámetros de la función void* es puntero) */
    //de cualquier cosa que hay que castear, prioridad.

    //xTaskCreate(vLectorTEC1, "vLectorTEC1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL), (TaskHandle_t *) NULL);
    //xTaskCreate(adc_tec, "adc_tec", 1024, NULL, (tskIDLE_PRIORITY + 4UL), (TaskHandle_t *) NULL);

    xTaskCreate(adc_task, "adc_task", 1024, NULL, (tskIDLE_PRIORITY + 4UL), (TaskHandle_t *) NULL);

    /* Start the scheduler */
    vTaskStartScheduler();

    /* Should never arrive here */
    return 1;
}
