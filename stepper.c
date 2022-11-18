/*****StepperMotor*****/

#include<stdio.h>
#include<stdio.h>

#include"common.h"

void DACInit(void);

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
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, 1);/* OE Pin*/ 
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4, 1); /* PS Pin */
 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14, 0); /* STEP Pin */
 
 DAC_Init(); /* DAC Initialization */
 timInit(); /* Timer Initialization */
}

void stepper(int32_t steps, uint32_t Delay)
{

 if(steps < 0)
 {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5, 1); /* FR Pin */
  steps = -steps;
 }
 else
 {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5, 0); /* FR Pin */
 }

 for(int i=0; i < steps; i++)
 {
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14, 1); /* Step Pin */
  delay(Delay);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14, 0); /* Step Pin */ 
  delay(Delay);
 }
}

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

ParserReturnVal_t CmdStepInit(int mode)
{
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  stepperInit();
  printf("Stepper is intialized\n");
  return CmdReturnOk;
}
  ADD_CMD("StepperInit", CmdStepInit, "Test Init")


ParserReturnVal_t CmdStepEnable(int mode)
{
  int32_t rc;
  uint32_t Val;
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  rc = fetch_uint32_arg(&Val);
  if(rc) {
  printf("Test CmdStepEnable\n");
  return CmdReturnBadParameter1;
 }
  printf("Stepper Enabled\n");
  stepperEnable(Val);
  return CmdReturnOk;
}
  ADD_CMD("StepEnable",CmdStepEnable,"Test Stepper Enable")

ParserReturnVal_t CmdStep(int mode)
{
  int32_t rc;
  int32_t steps;
  uint32_t Delay;
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  rc = fetch_int32_arg(&steps);
  if(rc) {
    printf("Test CmdStep steps\n");
  return CmdReturnBadParameter1;
  }
 
  rc = fetch_uint32_arg(&Delay);
  if(rc) {
  printf("Test CmdStep Delay\n");
  return CmdReturnBadParameter1;
  }

  printf("Stepper is On\n");
  stepper(steps, Delay);
  return CmdReturnOk;
}

  ADD_CMD("Step",CmdStep,"Test Step")
