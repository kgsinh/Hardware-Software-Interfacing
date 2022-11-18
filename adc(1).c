/* adc.c:
 *
 *  
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include "common.h"

#define ADC_VREF           3.3
#define ADC_V_PER_COUNT     ADC_VREF/4095.0 //12-bit resolution

ADC_HandleTypeDef hadc;
uint32_t a, val;
HAL_StatusTypeDef rc;
ADC_ChannelConfTypeDef config;
GPIO_InitTypeDef  GPIO_InitStruct;
int16_t ADCValue;
float voltage;

/* GPIO init function */
void GPIO_Init()
{   
/* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOC_CLK_ENABLE();
    
/*Configure these ADC pins in analog*/
GPIO_InitStruct.Pin = (GPIO_PIN_0 | GPIO_PIN_1);//Configure PC0 and PC1
GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
}

/* ADC init function */
void ADC_Init()
{  
  // HAL_StatusTypeDef rc;
  __HAL_RCC_ADC1_CLK_ENABLE();

	/**Common config
	 */
	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.NbrOfConversion = 1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
        hadc.Init.NbrOfDiscConversion = 0;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = 0;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	
	rc =  HAL_ADC_Init(&hadc);
	if (rc != HAL_OK) {
	  printf("ADC initialization failed with rc=%u\n",rc);
	}
	
}

unsigned int readADC(int a)
{ 
	/**Configure the selected Channel
	 */
        config.Channel = a;
        config.Rank = 1;
	config.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	config.SingleDiff = ADC_SINGLE_ENDED;
	config.OffsetNumber = ADC_OFFSET_NONE;
	config.Offset = 0;
	rc = HAL_ADC_ConfigChannel(&hadc, &config);
	if (rc != HAL_OK) {
	  printf("ADC channel configure failed with rc=%u\n",(unsigned)rc);
	  return 0;
	}
	
        /* Start the ADC peripheral */
        rc = HAL_ADC_Start(&hadc);
        if(rc != HAL_OK){
        printf("ADC start failed with rc=%u\n",(unsigned)rc);
        return 0;				   				
        }

      /* Wait for end of conversion */
rc = HAL_ADC_PollForConversion(&hadc, 100);
if(rc != HAL_OK){
printf("ADC poll for conversion failed with " "rc=%u\n",(unsigned)rc);
return 0;
} 

/* Read the ADC converted values */
val = HAL_ADC_GetValue(&hadc);

/* Stop the ADC peripheral */
rc = HAL_ADC_Stop(&hadc);
if(rc != HAL_OK){
printf("ADC stop failed with" "rc=%u\n",(unsigned)rc);
return 0;
}
 printf("ADCValue:%lu\n",val);   
return val;
}

ParserReturnVal_t CmdADC(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  fetch_uint32_arg(&a);
  GPIO_Init();  
  ADC_Init();
  ADCValue =  readADC(a);
 voltage = ADCValue * ADC_V_PER_COUNT;
 printf("Voltage = %0.2fV\n",voltage);
  return CmdReturnOk;
}
ADD_CMD("ADC",CmdADC,"Test")
