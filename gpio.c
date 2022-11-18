#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "common.h"
uint32_t a;
//Initialize gpio
void gpioinit()
  {
__HAL_RCC_GPIOA_CLK_ENABLE();

GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitStruct.Pin = (GPIO_PIN_5);
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 
  }


ParserReturnVal_t CmdKunal(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  gpioinit();
  fetch_uint32_arg(&a);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,a);
  return CmdReturnOk;
}
 ADD_CMD("input",CmdKunal," PA5 ON")
