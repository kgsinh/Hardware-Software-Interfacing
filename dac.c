/***** dac.c*****/

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include "common.h"

#define ADC_VREF           3.3
#define ADC_V_PER_COUNT     (ADC_VREF/4095.0) //12-bit resolution

static DAC_HandleTypeDef hdac;
HAL_StatusTypeDef rc;
uint32_t val;
float voltage;
int16_t DACValue;


/* GPIO init function */
void GPIOInit()
{
GPIO_InitTypeDef  GPIO_InitStruct;  
/* GPIO Ports Clock Enable */
__HAL_RCC_GPIOA_CLK_ENABLE();
/*Configure these DAC pins in analog*/
GPIO_InitStruct.Pin = GPIO_PIN_4;    //Configure PA4
GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* DAC init function */
void DAC_Init()
{
DAC_ChannelConfTypeDef DacConfig;
/*DAC Clock Enable*/
__HAL_RCC_DAC1_CLK_ENABLE();
/*Initialize DAC */
hdac.Instance = DAC1;
rc = HAL_DAC_Init(&hdac);
if(rc != HAL_OK)
{
printf("Unable to initialize " "DAC, rc=%d\n",rc);
return;
}
DacConfig.DAC_Trigger = DAC_TRIGGER_NONE;
DacConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
rc = HAL_DAC_ConfigChannel(&hdac, &DacConfig,DAC_CHANNEL_1);
if(rc != HAL_OK)
{
printf("Unable to configure DAC " "channel 1, rc=%d\n",rc);
return;
}
/* Enable the output */ 
__HAL_DAC_ENABLE(&hdac,DAC_CHANNEL_1);
}


/*DAC Write Function */
unsigned int DACWrite(int value)
{
/* Set initial values */
rc = HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,
DAC_ALIGN_12B_R,value);
if(rc != HAL_OK) {
printf("Unable to initial value on DAC " "channel 1, rc=%d\n",rc);
}
return value;
}


ParserReturnVal_t CmdDAC(int mode)
{
if(mode != CMD_INTERACTIVE) return CmdReturnOk;
fetch_uint32_arg(&val);
GPIOInit();  
DAC_Init();
DACValue = DACWrite(val);
voltage = DACValue * ADC_V_PER_COUNT;
if(val >= 0 && val <= 4095)
{
printf("Voltage = %0.2fV\n",voltage);
}
else
{
printf("Enter the value between 0 to 4095\n");
}
return CmdReturnOk;
}
ADD_CMD("DAC",CmdDAC,"DACTest")
