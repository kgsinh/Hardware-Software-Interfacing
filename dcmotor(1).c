/*****DC Motor*****/

/*************Header Files*****************/
#include<stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include"common.h"

/*****************Function Declaration**********************/
void dcdir(uint32_t motor, uint32_t dir);

/**********************Global Variables Declaration*********/
static volatile uint32_t stoptime, stoptime2 = 0;

/***************Initialize DC Motor************************/
void DCMotorInit()
{
 __HAL_RCC_GPIOA_CLK_ENABLE();
 GPIO_InitTypeDef  GPIO_InitStruct;

 GPIO_InitStruct.Pin = (GPIO_PIN_0|GPIO_PIN_1);
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 0;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);


 __HAL_RCC_GPIOB_CLK_ENABLE();
 GPIO_InitStruct.Pin = (GPIO_PIN_10|GPIO_PIN_11);
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 GPIO_InitStruct.Alternate = 0;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);
 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,0);
}

/*************Function to stop the motor1 using systick**********************/
void stoptimer()
{
 if(stoptime)
 {
  stoptime--;
  TIM1->CCR1 = 1000;
  //printf("%lu\n",stoptime);
  dcdir(1,1);
  if(stoptime==0)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,0);
  }
 }
}

/*************Function to stop the motor2 using systick**********************/
void stoptimer2()
{
 if(stoptime2)
 {
  stoptime2--;
  TIM1->CCR2 = 1000;
  //printf("%lu\n",stoptime2);
  dcdir(2,1);
  if(stoptime2==0)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,0);
  }
 }
}

/*******************Function to rotate the motor in different direction*************************/
void dcdir(uint32_t motor, uint32_t dir)
{ 
 switch(motor)
 {
  case 1:
  if(dir==1)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0); //Forward
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
   //DC Motor 1 will rotate in forward direction
  }

  else if(dir==2)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1); //Reverse
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
   //DC Motor 1 will rotate in reverse direction 
  }  
  break;

  case 2:
  if(dir==1)
  {
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0); //Forward
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,1);
   //DC Motor 2 will rotate in forward direction
  }

  else if(dir==2)
  { 
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1); //Reverse
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,0);
   //DC Motor 2 will rotate in reverse direction 
  }
  break;

  case 3:
  if(dir==1)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0); //Forward
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0); //Forward
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,1);
   //DC Motor 1 and DCMotor 2  will rotate in forward direction
  }

  else if(dir==2)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1); //Reverse
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1); //Reverse
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,0);
   //DC Motor 1 and DC Motor 2 will rotate in reverse direction 
  }
  break;

  case 4:
  if(dir==1)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0); //Stop
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0); 
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,0);
   //DC Motor 1 and DC Motor 2 are stopped
  }

  else if(dir==2)
  {
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1); //Stop
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1); 
   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,1);
   //DC Motor 1 and DC Motor 2 are stopped 
  }
  break;

  default:
  printf("Invalid motor index\n");
  return;
 }
}


/**********************Parser Functions***********************************/
ParserReturnVal_t CmdDCInit(int mode)
{
 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 DCMotorInit();
 printf("DC is initialized\n");

 return CmdReturnOk;
}
 ADD_CMD("DCInit", CmdDCInit, "Test DC Initialization") 

ParserReturnVal_t CmdDCDirection(int mode)
{
 uint32_t rc, dir, motor;

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 rc = fetch_uint32_arg(&motor);
 if(rc) 
 {
  printf("Test motor\n");
  return CmdReturnBadParameter1;
 }
  
 rc = fetch_uint32_arg(&dir);
 if(rc) 
 {
  printf("Test direction\n");
  return CmdReturnBadParameter1;
 }

 dcdir(motor, dir);

 return CmdReturnOk;
}
 ADD_CMD("DCDirection", CmdDCDirection, "Test Motor and Direction")

ParserReturnVal_t CmdStop(int mode)
{
 uint32_t rc,time;

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 rc=fetch_uint32_arg(&time);
 printf("Timer to stop DC Motor 1\n");
 if(rc) 
 {
  printf("Test timer\n");
  return CmdReturnBadParameter1;
 }

 stoptime = (time*1000);
 
 return CmdReturnOk;
}
 ADD_CMD("stoptimer", CmdStop, "Test stop timer for motor 1")

ParserReturnVal_t CmdStop2(int mode)
{
 uint32_t rc,time;

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 rc=fetch_uint32_arg(&time);
 printf("Timer to stop DC Motor 2\n");
 if(rc) 
 {
  printf("Test timer\n");
  return CmdReturnBadParameter1;
 }

 stoptime2 = time*1000;
 
 return CmdReturnOk;
}
 ADD_CMD("stoptimer2", CmdStop2, "Test stop timer for motor 2")

/************End of file***************/
