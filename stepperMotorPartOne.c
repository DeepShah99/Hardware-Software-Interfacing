/* 
 * Filename:            stepperMotor_Group20.c
 * Description:         Initialize stepper motor in c 
                        Rotate the rotor in clockwise as well as in anticlockwise directions
                        Give the delay to rotate via timers as well as interrupts
                        Implement trapezoidal motion profile for the stepper motor
 * Author & Student id: Deep Shah and Dhruv Patel
 * Date:                13th Oct 2021 
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"
#include "main.h"

TIM_HandleTypeDef tim11;		// handle for timer11 (for interrupt)
TIM_HandleTypeDef tim2;               //  handle for timer2 (for counter)
                      
//prototype
void trapProfile(int dt);             //trapezoidal motion profile

//global variables
uint32_t isDelayEntered;            
uint32_t delayMicro = 0;
int isStepperInit = 0;
int isStepperEnabled = 0;
int32_t steps = 0;
int32_t isStepsEntered = 0;
int32_t OutputOnOff = 0;
int32_t isOutputOn = 0;



// Function:     stepperinit
// Description:  Initialize stepper motor and timer as well 
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t stepperinit(int mode)
{
  /* timer Initialization */


  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  __HAL_RCC_TIM2_CLK_ENABLE();
  
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim2.Init.Period = 0xffff;
  tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim2.Init.RepetitionCounter = 0;
  
  /* Stepper Initialization */

  HAL_GPIO_WritePin(PS_PORT,PS,1);
  HAL_GPIO_WritePin(RST_PORT,RST,1);
  HAL_GPIO_WritePin(MD1_PORT,MD1,1);
  HAL_GPIO_WritePin(MD2_PORT,MD2,1);
  HAL_GPIO_WritePin(VREF_PORT,VREF,1);
  HAL_GPIO_WritePin(OE_PORT,OE,1);
  

  printf("Stepper initialized successfully...\n");
  isStepperInit = 1;
  isStepperEnabled = 0;
  
  return CmdReturnOk;

}

// Function:     stepperenable
// Description:  enable the outputs of the stepper motor 
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t stepperenable(int mode)
{
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
   if(isStepperInit == 1){
  isOutputOn = fetch_int32_arg(&OutputOnOff);

  if(isOutputOn) {                              //check if stepper enable pin is entered
    printf("Please enter the On/Off\n");
    return CmdReturnBadParameter1;
  }
  if(OutputOnOff > 1 || OutputOnOff < 0) {
    printf("Enter only 1 | 0 to turn on/off\n"); //check if the number is  0 or 1 for the stepper enable pin
    return CmdReturnBadParameter1;
  }
  if(OutputOnOff == 1){                          //Enable the stepper outputs
    HAL_GPIO_WritePin(OE_PORT,OE,0);             
    printf("Stepper outputs enabled...\n");
    isStepperEnabled = 1;
    }
    else{                                        //disable the stepper outputs
      HAL_GPIO_WritePin(OE_PORT,OE,1);           
      printf("Stepper outputs disabled...\n");
          isStepperEnabled = 0;
    }
  }
  else{
  printf("Please initialize stepper motor first");
  }
  return CmdReturnOk;

}

// Function:     stepperdelay
// Description:  Get value from terminal and produce a delay for the stepper
// Prameter:     mode
// Return:       delayMicro
ParserReturnVal_t stepperdelay(int mode)
{
 
if(isStepperInit == 1 && isStepperEnabled == 1){      //check first stepper is initialized or not
 steps = 0;                        
  HAL_TIM_Base_Init(&tim2);

  isStepsEntered = fetch_int32_arg(&steps);
 

  if(isStepsEntered) {
    printf("Please enter the steps\n");
    return CmdReturnBadParameter1;
  }
  
   if(steps >= 0) {                                   //if steps are positive then clockwise rotation
    HAL_GPIO_WritePin(FR_PORT,FR,1);

  }
  else {
    HAL_GPIO_WritePin(FR_PORT,FR,0);                  //if steps are negative then anticlockwise rotation
    steps = -steps;  
  }
   
    isDelayEntered = fetch_uint32_arg(&delayMicro);

  if(isDelayEntered) {
    printf("Please enter the delay in Microsecond\n");
    return CmdReturnBadParameter1;
  }
  for(int i = 0; i <= steps; i++){
  printf("step = %d\n", i);
  HAL_TIM_Base_Start(&tim2);  
  TIM2->CR1 |= TIM_CR1_CEN;
  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,1);
  
  if(delayMicro>65535)                           //execute this if delay is greater than 65535
  {
    for(int temp = 0; temp<delayMicro; temp+= 65535)    //execute this loop till temp is less than 65535 
    {
       HAL_TIM_Base_Start(&tim2);
       TIM2->CNT = 0;                            // reset the counter
       while(__HAL_TIM_GET_COUNTER(&tim2) < 65535)
       {
   	  WDTFeed();

       }
    }
      printf("Counter completed\n");
  }	
  else                                           //execute this if delay is less than 65535  
  {
    TIM2->CNT = 0;                               // reset the counter
    while(__HAL_TIM_GET_COUNTER(&tim2) < delayMicro)
    {
  	WDTFeed();
    }
 
  }
  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,0);						

  }
  
  }
 
  else{
  printf("please initialize the stepper and enable the outputs as well");          //print the command if stepper not initialized
  }
  return delayMicro;
}

// Function:     CmdTimerIrq
// Description:  Command to Initialize interrupt
// Prameter:     delayMilli
// Return:       none
ParserReturnVal_t CmdTimerIrq(int mode)
{
if(isStepperInit == 1 && isStepperEnabled == 1){
  steps = 0;
  isStepsEntered = fetch_int32_arg(&steps);
  steps = 2 * steps;

  if(isStepsEntered) {
    printf("Please enter the steps\n");
    return CmdReturnBadParameter1;
  }
  
   if(steps >= 0) {                                
    HAL_GPIO_WritePin(FR_PORT,FR,1);                    //if steps are positive then clockwise rotation
  }
  else {
    HAL_GPIO_WritePin(FR_PORT,FR,0);                    //if steps are negative then anti clockwise rotation
    steps = -steps;  
  }

    isDelayEntered = fetch_uint32_arg(&delayMicro);

  if(isDelayEntered) {
    printf("Please enter the delay in Microsecond\n");
    return CmdReturnBadParameter1;
  }


	  
	if(mode != CMD_INTERACTIVE) return CmdReturnOk;

	/* Put your command implementation here */


  

	__HAL_RCC_TIM11_CLK_ENABLE();
	tim11.Instance = TIM11;
	tim11.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000 - 1;// this will cause the timer to create ms (set to 1000000 - 1 for us)
	tim11.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim11.Init.Period = delayMicro;	// in this example the number here will be the number of ms for on/off
	tim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim11.Init.RepetitionCounter = 0;
	tim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;// this parameter will cause the timer to reset when timed out
	
  
	HAL_TIM_Base_Init(&tim11);

	HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10, 0U);	// note the timer 11 IRQ on this board is shared with Timer 1
	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

	HAL_TIM_Base_Start_IT(&tim11);
	}
	
 else{
  printf("please initialize the stepper and enable the outputs as well");      //print the command if timer not initialized
  }

	return CmdReturnOk;
}
// FUNCTION      : TIM1_TRG_COM_TIM11_IRQHandler
// DESCRIPTION   : This is the interrupt handler for timer 11 and 1.  It is usually created by CubeMX if we were using it
// PARAMETERS    : None
// RETURNS       : Nothing
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{ 

	HAL_TIM_IRQHandler(&tim11);

}

// FUNCTION      : HAL_TIM_PeriodElapsedCallback
// DESCRIPTION   : This is the interrupt handler for timer 11 and 1.  It is usually created by CubeMX if we were using it
// PARAMETERS    : htim - the handle to the timer configuration information
// RETURNS       : Nothing
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim )
{
      if(steps != 0){
	if( htim == &tim11 ){	
					
		HAL_GPIO_TogglePin( LD2_GPIO_Port, LD2_Pin );
		} 
       steps--;
	}
	
}

// Function:     trap
// Description:  trap command for the trapezoidal profile
// Prameter:     mode
// Return:       delayMicro
ParserReturnVal_t trap(int mode)
{
 
if(isStepperInit == 1){                   
  HAL_TIM_Base_Init(&tim2);
  int d = 1000;
  int dt = 0;
  for(int i=1; i<=200; i++)                 //acceleration    
  {
    dt = d - (i*5) + 1;
    printf("dt = %d\n",dt);
    trapProfile(dt);
  }
    for(int i=200; i<=800; i++)             //constant maximum speed
  { 
    printf("dt = %d\n",dt);
    trapProfile(dt);
  }
  for(int i=1; i<=200; i++)                 //deceleration
  {
    dt = i * 5;
    printf("dt = %d\n",dt);
    trapProfile(dt);
  }
  

  }
 
  else{
  printf("please initialize the stepper");          //print the command if stepper not initialized
  }
  return delayMicro;
}

// Function:     trapProfile
// Description:  Initialize the trap motion profile for the stepper motor 
// Prameter:     mode
// Return:       cmdReturnOK
void trapProfile(int dt)
{

  HAL_TIM_Base_Start(&tim2);  
  TIM2->CR1 |= TIM_CR1_CEN;
  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,1);
  
  if(dt>65535)                                   //execute this if delay is greater than 65535
  {
    for(int temp = 0; temp<dt; temp+= 65535)     //execute this loop till temp is less than 65535 
    {
       HAL_TIM_Base_Start(&tim2); 
       TIM2->CNT = 0;                            // reset the counter   
       while(__HAL_TIM_GET_COUNTER(&tim2) < 65535)
       {
   	  WDTFeed();

       }
    }
      
  }	
  else                                           //execute this if delay is less than 65535  
  {
    TIM2->CNT = 0;                               // reset the counter
    while(__HAL_TIM_GET_COUNTER(&tim2) < dt)
    {
  	WDTFeed();
    }

  }
  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,0);	

}

ADD_CMD("stepperinit",stepperinit,"                timer initialize...")   //command to initialize stepper
ADD_CMD("stepperenable",stepperenable,"<channel>       get timer value for the delay")//command to enable outputs of the motor
ADD_CMD("stepperdelay",stepperdelay,"<channel>       get timer value for the delay")//command to feed delay and steps 
ADD_CMD("stepperIrq",CmdTimerIrq,"                using a timer with IRQ example")  //command to start motor with Interrupt handler
ADD_CMD("trap",trap,"                using a timer with IRQ example")      //trapezoidal motion profile

