/*****PWM*****/

/*************Header Files*****************/
#include<stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include"common.h"

TIM_HandleTypeDef tim1;

/*******************PWM Initialization Function************************/
void PWMIni(void)
{
 TIM_OC_InitTypeDef sConfig;
 GPIO_InitTypeDef GPIO_InitStruct;

 /* Initialize GPIOs */
 __HAL_RCC_GPIOA_CLK_ENABLE();
 GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 6;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 /* Initialize PWM */
 __HAL_RCC_TIM1_CLK_ENABLE();
 tim1.Instance = TIM1;
 tim1.Init.Prescaler     =  HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
 tim1.Init.CounterMode   =  TIM_COUNTERMODE_UP;
 tim1.Init.Period        = 1000;
 tim1.Init.ClockDivision =  TIM_CLOCKDIVISION_DIV1;
 tim1.Init.RepetitionCounter = 0;
 HAL_TIM_Base_Init(&tim1);
 HAL_TIM_Base_Start(&tim1);

 sConfig.OCMode       = TIM_OCMODE_PWM1;
 sConfig.Pulse        = 0;
 sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
 sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
 sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
 sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
 sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;
 HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_1);
 HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1);
 HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_2);
 HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_2);
 HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_3);
 HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_3);
}

/*******************PWM input values/percentage and channel selection***********************/
void PWMCV(uint32_t channel, uint32_t value)
{
 /******Check inputs********/
 switch(channel)
 {
  case 1:
  TIM1 -> CCR1 = value;
  printf("Channel 1 is in use\n"); //PA8
  break; 
  
  case 2:
  TIM1 -> CCR2 = value;
  printf("Channel 2 is in use\n"); //PA9
  break;

  case 3:
  TIM1 -> CCR3 = value;
  printf("Channel 3 is in use\n"); //PA10
  break;
 }
}

/**********************Parser Functions***********************************/
ParserReturnVal_t CmdPWMIni(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;
 PWMIni();
 printf("PWM is initialized\n");
 return CmdReturnOk;
}
 ADD_CMD("PWMIni", CmdPWMIni, "Test PWM Initialization")

ParserReturnVal_t CmdPWMCV(int mode)
{
 uint32_t rc, channel, value;
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 rc = fetch_uint32_arg(&channel);

 if(channel < 1 || channel > 3 )
 {
  printf("Invalid Channel input\n");
 }

 if(rc) 
 {
  printf("Test Channel\n");
  return CmdReturnBadParameter1;
 }
  
 rc = fetch_uint32_arg(&value);
 
 if(value < 0 || value > 100) 
 {
   printf("Enter percentage between 0 and 100\n");
 }
 else
 {
  value = value*10; //Maximum value limit is 1000
  PWMCV(channel, value); 
 }

 if(rc) 
 {
  printf("Test Value\n");
  return CmdReturnBadParameter1;
 }

 return CmdReturnOk;
}
 ADD_CMD("PWMCV", CmdPWMCV, "Test PWM Channel and Value")
