/*****StepperMotorPartTwo*****/

/*************Header Files*****************/
#include<stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include"common.h"

void DACInit(void);

TIM_HandleTypeDef tim15;

/************Function for stepper and DAC initialization***************/
void stepperInit()
{
 GPIO_InitTypeDef GPIO_InitStruct;

 /* Configure Clock */
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();


 /* Initialize GPIO */
 GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 0;
 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

 GPIO_InitStruct.Pin = GPIO_PIN_14;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 0;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

 /* Configure initial states */
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3, 1); /* RST Pin */
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, 1); /* OE Pin */
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4, 1);/* PS Pin*/ 
 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14, 0); /* STEP Pin */
 
 DAC_Init(); /* DAC Initialization */
}

/*****************Function for PWM Initialization*********************/
void PWMInit()
{
 GPIO_InitTypeDef GPIO_InitStruct;

 /* Initialize GPIOs */
 __HAL_RCC_GPIOB_CLK_ENABLE();
 GPIO_InitStruct.Pin = GPIO_PIN_14;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 1;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

 /* Initialize PWM */
 __HAL_RCC_TIM15_CLK_ENABLE();
 tim15.Instance = TIM15;
 tim15.Init.Prescaler     =  HAL_RCC_GetPCLK2Freq() / 1000000;
 tim15.Init.CounterMode   =  TIM_COUNTERMODE_UP;
 tim15.Init.Period        = 1000;
 tim15.Init.ClockDivision =  TIM_CLOCKDIVISION_DIV1;
 tim15.Init.RepetitionCounter = 0;
 HAL_TIM_Base_Init(&tim15);


 TIM_OC_InitTypeDef sConfig;
 sConfig.OCMode       = TIM_OCMODE_PWM1;
 sConfig.Pulse        = 0;
 sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
 sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
 sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
 sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
 sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;
 HAL_TIM_PWM_ConfigChannel(&tim15,&sConfig, TIM_CHANNEL_1);
 HAL_TIM_PWM_Start(&tim15, TIM_CHANNEL_1);
}

/***********************Function for Period and Pulse**************************************/
void PWMPeriod(int period)
{
 __HAL_RCC_TIM15_CLK_ENABLE();
 tim15.Instance = TIM15;
 tim15.Init.Prescaler     =  HAL_RCC_GetPCLK2Freq() / 1000000;
 tim15.Init.CounterMode   =  TIM_COUNTERMODE_UP;
 tim15.Init.Period        = period;
 tim15.Init.ClockDivision =  TIM_CLOCKDIVISION_DIV1;
 tim15.Init.RepetitionCounter = 0;
 HAL_TIM_Base_Init(&tim15);


 TIM_OC_InitTypeDef sConfig;
 sConfig.OCMode       = TIM_OCMODE_PWM1;
 sConfig.Pulse        = period/2;
 sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
 sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
 sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
 sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
 sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;
 HAL_TIM_PWM_ConfigChannel(&tim15,&sConfig, TIM_CHANNEL_1);

 HAL_TIM_PWM_Start(&tim15, TIM_CHANNEL_1);
}

/*************************Function for reverse rotation of  stepper motor********************************/
void reverse(int a)
{
 if(a < 0)
 {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5, 1); /* FR Pin */
  a = -a;
  PWMPeriod(a);
 }
 else
 {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5, 0); /* FR Pin */
  PWMPeriod(a);
 }
}

/************************************Function for Step Enable***********************************/
void stepperEnable(uint32_t enable)
{
 if(enable)
 {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, 0); /* OE Pin */
 }
 else
 {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, 1); /* OE Pin */
 }
}

/**********************Parser Functions***********************************/
ParserReturnVal_t CmdStepInit(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;
 stepperInit();
 printf("Stepper is intialized\n");
 return CmdReturnOk;
}
 ADD_CMD("StepperInit", CmdStepInit, "Test Stepper Initialization")

ParserReturnVal_t CmdPWMInit(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;
 PWMInit();
 printf("PWM is initialized\n");
 return CmdReturnOk;
}
 ADD_CMD("PWMInit", CmdPWMInit, "Test PWM Initialization")

ParserReturnVal_t CmdPWM(int mode)
{
 uint32_t rc, a;
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;
 rc = fetch_uint32_arg(&a);
 if(rc) 
 {
  printf("Test Period\n");
  return CmdReturnBadParameter1;
 }
  reverse(a);
  return CmdReturnOk;
}
  ADD_CMD("PWM", CmdPWM, "Test PWM Initialization")

ParserReturnVal_t CmdStepEnable(int mode)
{
 int32_t rc;
 uint32_t Val;
  
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;
 rc = fetch_uint32_arg(&Val);
 if(rc) 
 {
  printf("Test CmdStepEnable\n");
  return CmdReturnBadParameter1;
 }
  printf("Stepper Enabled\n");
  stepperEnable(Val);
  return CmdReturnOk;
}
  ADD_CMD("StepEnable",CmdStepEnable,"Test Stepper Enable")
