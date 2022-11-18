/*************ClosedLoopMotorControl.c*************/

/**************Header Files************/
#include <stdint.h>
#include <stdio.h>

#include "common.h"

/**************Global variable declaration*******************/
uint32_t setPoint;
int32_t Kp;
int32_t speed;
int32_t error;
int32_t control;

/*********************Function Declaration***********************/
void PWMIni(void);
void encoderInit(void);
void DCMotorInit(void);


void PIDInit(void *data)
{
 PWMIni();
 DCMotorInit();
 encoderInit();
}

void PIDTask(void *data)
{
 int32_t reading;
 static int32_t previousReading = 0;
 reading = encoder();                     //Get Current Position
 speed = reading - previousReading;   //Calculate speed
 previousReading = reading;
 error = setPoint - speed;
 control = Kp * error;
 //control = control + 500;

 if(Kp==0)
 {
  dcdir(4, 1); //Motor 1 stopped
 }

 if(setPoint==0)
 {
  dcdir(4, 1); //Motor 1 stopped
 }

 /*Set output*/
 if(control >= 0)
 {
  PWMCV(1, control);
  dcdir(1, 1); //Motor 1, Forward direction
 }
 else
 {
  PWMCV(1, -control); 
  dcdir(1, 2); //Motor 1, Reverse direction
 }

}

ADD_TASK(PIDTask,PIDInit, NULL, 100, "PID control loop task")

/**********Parser Functions************/
ParserReturnVal_t CmdKp(int mode)
{
 int rc;

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 rc=fetch_int32_arg(&Kp);
 

 if(rc) 
 {
  printf("Test Kp\n");
  return CmdReturnBadParameter1;
 }
 
 return CmdReturnOk;
}
 ADD_CMD("Kp", CmdKp, "Set Kp value")

ParserReturnVal_t CmdSetPoint(int mode)
{
 uint32_t rc;

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

 rc=fetch_uint32_arg(&setPoint);
 
 if(rc) 
 {
  printf("Test SetPoint\n");
  return CmdReturnBadParameter1;
 }
 
 return CmdReturnOk;
}
 ADD_CMD("spt", CmdSetPoint, "SetPoint")

ParserReturnVal_t CmdState(int mode)
{

 if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  printf("Set Point = %ld\n"
          "Kp = %ld\n"
          "Speed = %ld\n"
          "Control = %ld\n"
          "Error = %ld\n", setPoint, Kp, -speed*2, control, error);
 
 return CmdReturnOk;
}
 ADD_CMD("state", CmdState, "State")

/************End of file************/
