/**********quadratureEncoder.c************/

/*******************Header Files***********************/
#include<stdint.h>
#include<stdio.h>

#include "common.h"

TIM_HandleTypeDef tim3;
uint32_t stopValue, flag=0;
int32_t value;

/***********Function Declaration*************/
void DCMotorInit(void);
void PWMIni(void);
void PWMCV(uint32_t channel, uint32_t value);

/*****************Encoder Initialization*********************/
void encoderInit(void)
{
 DCMotorInit(); /*DC Motor Initialization*/
 PWMIni();      /*PWM Initialization*/

 HAL_StatusTypeDef rc;
 TIM_Encoder_InitTypeDef encoderConfig;
 GPIO_InitTypeDef  GPIO_InitStruct;

 __HAL_RCC_GPIOA_CLK_ENABLE();
 GPIO_InitStruct.Pin = (GPIO_PIN_6|GPIO_PIN_7);
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 2;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 /*Initialize Timer base*/
 __HAL_RCC_TIM3_CLK_ENABLE();
 tim3.Instance = TIM3;
 tim3.Init.Prescaler = 0;
 tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
 tim3.Init.Period = 0xffff;
 tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 tim3.Init.RepetitionCounter = 0;

 rc = HAL_TIM_Base_Init(&tim3);
 if(rc != HAL_OK) 
 {
  printf("Failed to initialize Timer 3 Base,"
        "rc=%u\n",rc);
 return;
 }

 encoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;
 encoderConfig.IC1Polarity = 0;
 encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
 encoderConfig.IC1Prescaler = 0;
 encoderConfig.IC1Filter = 3;
 encoderConfig.IC2Polarity = 0;
 encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
 encoderConfig.IC2Prescaler = 0;
 encoderConfig.IC2Filter = 3;

 rc = HAL_TIM_Encoder_Init(&tim3,&encoderConfig);
 if(rc != HAL_OK) 
 {
  printf("Failed to initialize Timer 3 Encoder,"
        "rc=%u\n",rc);
 return;
 }

 rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_1);
 if(rc != HAL_OK) 
 {
  printf("Failed to start Timer 3 Encoder,"
         "rc=%u\n",rc);
  return;
 }

 rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_2);
 if(rc != HAL_OK) 
 {
  printf("Failed to start Timer 3 Encoder,"
         "rc=%u\n",rc);
  return;
 }
}

int32_t encoder(void)
{
 return (int16_t)(TIM3->CNT);
}


/****************Rotate and Stop************************/
/*
void sMotor(void)
{
 value = encoder();
 if(flag==1)
 {
  if(value > stopValue)
   {
    PWMCV(1,0);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0); //Forward
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
   }
   else
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0); //Stop
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
   stopValue--;
  }
 }
}*/

/**********************Parser Functions***********************************/
ParserReturnVal_t CmdencoderInit(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 encoderInit();
 printf("Encoder is initialized\n");
 printf("DC Motor is initialized\n");
 printf("PWM is initialized\n");

 return CmdReturnOk;
}
 ADD_CMD("enInit", CmdencoderInit, "Test Encoder") 

ParserReturnVal_t CmdrEncoder(int mode)
{
 int32_t x;

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 value = encoder();

 if(value >= -118 && value <= 118) //1 rotation = 29.47, So, it is 117.88 & approx. = 118.
 {
  x = (value * 360) / 118;
  printf("Encoder position: %ld Degree\n", x);
  printf("Raw value: %ld\n",value);
 }
 else
 {
  printf("Degree is out of range and value is %ld\n",value);
 }

 return CmdReturnOk;
}
 ADD_CMD("encoder", CmdrEncoder, "Test Read Encoder")

ParserReturnVal_t CmdEncoderReset(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 TIM3->CNT = 0;
 printf("Encoder has been reset\n");

 return CmdReturnOk;
}
 ADD_CMD("enReset", CmdEncoderReset, "Test Encoder Reset")

/*
ParserReturnVal_t CmdMotorStop(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 fetch_uint32_arg(&stopValue);
 printf("Motor will stop at %ld\n",stopValue);
 flag=1;
 sMotor();
 return CmdReturnOk;
}
 ADD_CMD("stopMotor", CmdMotorStop, "Test Motor stop")
*/
/*************End of file**************/
