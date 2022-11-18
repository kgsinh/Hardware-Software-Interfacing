/***Temperature Controlled DC Motor with PWM****/
/***********finalproject.c***********/

/**************Header files****************/
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include "common.h"

/******************Macro definitions*********************/
#define ADC_VREF           3.3
#define ADC_V_PER_COUNT     (ADC_VREF/4095.0) //12-bit resolution
#define MCP9700_T_COEFF     100.0      /*From MCP9700 Spec*/
#define MCP9700_OFFSET      0.5    /*500mV offset*/

/***********************Function Declaration****************************/
void PWMIni();
void DCMotorInit();

/****************************Global variable declaration*********************************/
ADC_HandleTypeDef hadc;
uint32_t a, val;
HAL_StatusTypeDef rc;
ADC_ChannelConfTypeDef config;
GPIO_InitTypeDef  GPIO_InitStruct;
int16_t ADCValue;
float voltage, temperature = 0;
uint16_t percentage;

/* GPIO init function */
void GPIO_Initialization()
{   
 PWMIni(); //PWM initialization
 DCMotorInit(); //DC Motor initialization

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOC_CLK_ENABLE();
    
 /*Configure these ADC pins in analog*/
 GPIO_InitStruct.Pin = GPIO_PIN_0;//Configure PC0 (channel 6)
 GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 0;
 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* ADC init function */
void ADC_Initialization()
{  
 __HAL_RCC_ADC1_CLK_ENABLE();

 /**Common config**/
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
 if (rc != HAL_OK) 
 {
  printf("ADC initialization failed with rc=%u\n",rc);
 }	
}

unsigned int readADCValue()
{ 
 /**Configure the selected Channel**/
 config.Channel = 6;
 config.Rank = 1;
 config.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
 config.SingleDiff = ADC_SINGLE_ENDED;
 config.OffsetNumber = ADC_OFFSET_NONE;
 config.Offset = 0;
 rc = HAL_ADC_ConfigChannel(&hadc, &config);
 if (rc != HAL_OK) 
 {
  printf("ADC channel configure failed with rc=%u\n",(unsigned)rc);
 return 0;
 }
	
 /* Start the ADC peripheral */
 rc = HAL_ADC_Start(&hadc);
 if(rc != HAL_OK)
 {
  printf("ADC start failed with rc=%u\n",(unsigned)rc);
 return 0;				   				
 }

 /* Wait for end of conversion */
 rc = HAL_ADC_PollForConversion(&hadc, 100);
 if(rc != HAL_OK)
 {
  printf("ADC poll for conversion failed with " "rc=%u\n",(unsigned)rc);
 return 0;
 } 

 /* Read the ADC converted values */
 val = HAL_ADC_GetValue(&hadc);

 /* Stop the ADC peripheral */
 rc = HAL_ADC_Stop(&hadc);
 if(rc != HAL_OK)
 {
  printf("ADC stop failed with" "rc=%u\n",(unsigned)rc);
 return 0;
 }
 printf("ADCValue:%lu\n",val);   
 return val;
}

void startProcess(int ADCValue)
{
 while(1)
 {
  ADCValue =  readADCValue();
  voltage = ADCValue * ADC_V_PER_COUNT; 
  printf("Voltage = %0.2fV\n",voltage);
  temperature = ((voltage - MCP9700_OFFSET) * MCP9700_T_COEFF);
  float adjustedTemp = temperature-16.4; //Setting to room temperature
  printf("Temp %0.1f Degree\n", adjustedTemp);
  HAL_Delay(500);

  if(adjustedTemp <= 20) //Temperature
  {
   percentage = 0;  //0%
   TIM1->CCR1 = percentage;
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0); 
  }
  else if(adjustedTemp >= 21 && adjustedTemp <= 30)
  {
   percentage = 200;   //20%
   TIM1->CCR1 = percentage;
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
  }
  else if((adjustedTemp >= 31) && (adjustedTemp <=40))
  {
   percentage = 500;   //50%
   TIM1->CCR1 = percentage;
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
  } 
  else if(adjustedTemp > 40)
  {
   percentage = 1000;   //100%
   TIM1->CCR1 = percentage;
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
  }
  else
  {
   percentage = 0;
   TIM1->CCR1 = percentage;
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
  }
   WDTFeed();
 }  
}


/**********************Parser Functions***************************/
 ParserReturnVal_t CmdADCfp(int mode) 
 { 
   if(mode != CMD_INTERACTIVE) return CmdReturnOk; 
   GPIO_Initialization();   
   ADC_Initialization(); 
   printf("GPIOs are Initialized\n");
   printf("ADC is Initialized\n");
   printf("PWM is Initialized\n");
   printf("DC Motor is Initialized\n");
 
   return CmdReturnOk; 
 } 
ADD_CMD("ADCI",CmdADCfp,"Test GPIO and ADC Initialization") 

ParserReturnVal_t CmdADCChfp(int mode) 
 { 
   if(mode != CMD_INTERACTIVE) return CmdReturnOk; 
   startProcess(ADCValue);
   return CmdReturnOk; 
 } 
ADD_CMD("ADCC",CmdADCChfp,"Test ADC Channel") 
