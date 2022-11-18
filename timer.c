/*****timer*****/


#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include "common.h"

void timInit(void);
void delay(uint32_t delayVal);

TIM_HandleTypeDef tim17;

void timInit()
{
  //timer Clock Enable & Initialization
  __HAL_RCC_TIM17_CLK_ENABLE();

  tim17.Instance = TIM17;
  tim17.Init.Prescaler     =  HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  tim17.Init.CounterMode   =  TIM_COUNTERMODE_UP;
  tim17.Init.Period        = 0xffff;
  tim17.Init.ClockDivision =  TIM_CLOCKDIVISION_DIV1;
  tim17.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&tim17);
  HAL_TIM_Base_Start(&tim17);

  //GPIO Clock Enable & Initialization
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void delay(uint32_t delayVal)
{
  TIM17->CNT = 0;       /* Reset counter */
  while(TIM17->CNT < delayVal) 
  {
    asm volatile ("nop\n");
  }    
}

ParserReturnVal_t CmdInit(int mode)
{
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
     timInit();
     printf("Timer and GPIO are intialized\n");
  return CmdReturnOk;
}
  ADD_CMD("Init",CmdInit,"Test Init")


ParserReturnVal_t CmdTimer(int mode)
{
  uint32_t delayVal,rc;
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  rc = fetch_uint32_arg(&delayVal);
  if(rc) {
    printf("Please supply 1 | 0 to turn on or off the LED\n");
  return CmdReturnBadParameter1;
  }
  if(delayVal >= 0 && delayVal <= 65535 ){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);
    delay(delayVal);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
    delay(delayVal);
  }
  else{
    printf("Enter value between 0 to 65535\n");
  }
  return CmdReturnOk;
}

  ADD_CMD("delay",CmdTimer,"Test Delay")

