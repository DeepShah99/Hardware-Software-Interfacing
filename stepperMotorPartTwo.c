/* 
 * Filename:            stepperMotor_PartTwo_Group20.c
 * Description:         Initialize stepper motor in c 
                        Control the speed of the motor via pwm
                        Rotate the rotor in clockwise as well as in anticlockwise directions
                        Give the delay to rotate via timers 
 * Author :             (Group 20) Deep Shah and Dhruv Patel
 * Date:                4th Nov 2021 
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"
#include "main.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);
void speed(uint8_t pwm);
int32_t pwmValue = 0;
int32_t ispwmValueEntered = 0;
uint32_t isDelayEntered;            
uint32_t delayMicro = 0;
int isStepperInit = 0;
int isStepperEnabled = 0;
int32_t steps = 0;
int32_t isStepsEntered = 0;
int32_t OutputOnOff = 0;
int32_t isOutputOn = 0;
 
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef tim2;               //  handle for timer2 (for counter)
 ParserReturnVal_t pwminit(int action)
{

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  __HAL_RCC_TIM1_CLK_ENABLE();
TIM_ClockConfigTypeDef sClockSourceConfig = {0};
TIM_MasterConfigTypeDef sMasterConfig = {0};
TIM_OC_InitTypeDef sConfigOC = {0};
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

/* USER CODE BEGIN TIM1_Init 1 */

/* USER CODE END TIM1_Init 1 */
htim1.Instance = TIM1;
htim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
htim1.Init.Period = 1000;
htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
htim1.Init.RepetitionCounter = 0;
htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
{
Error_Handler();
}
sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
{
Error_Handler();
}
if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
{
Error_Handler();
}
sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
{
Error_Handler();
}
sConfigOC.OCMode = TIM_OCMODE_PWM1;
sConfigOC.Pulse = 0;
sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
{
Error_Handler();
}
if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
{
Error_Handler();
}
if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
{
Error_Handler();
}
sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
sBreakDeadTimeConfig.DeadTime = 0;
sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
{
Error_Handler();
}
/* USER CODE BEGIN TIM1_Init 2 */

/* USER CODE END TIM1_Init 2 */
HAL_TIM_MspPostInit(&htim1);

HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  return CmdReturnOk;
}

  
void speed(uint8_t pwm) {
	htim1.Instance->CCR1 = pwm;
}
 void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspPostInit 0 */

  /* USER CODE END TIM1_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    PA9     ------> TIM1_CH2
    PA10     ------> TIM1_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM1_MspPostInit 1 */

  /* USER CODE END TIM1_MspPostInit 1 */
  }

}

                      



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

// Function:     stepperspeed
// Description:  Get value from terminal and produce a speed for the stepper
// Prameter:     mode
// Return:       delayMicro
ParserReturnVal_t stepspeed(int mode)
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
   
    ispwmValueEntered = fetch_int32_arg(&pwmValue);
    if(ispwmValueEntered) {
    printf("Please enter the pwm vale\n");
    return CmdReturnBadParameter1;
  }
  
speed(pwmValue);
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



ADD_CMD("pwminit",pwminit,"initialize pwm")                      //initialize the pwm channel
ADD_CMD("stepperinit",stepperinit,"                timer initialize...")   //command to initialize stepper
ADD_CMD("stepperenable",stepperenable,"<channel>       get timer value for the delay")//command to enable outputs of the motor
ADD_CMD("stepspeed",stepspeed,"<channel>       get pwm speed")//command to feed delay and steps 

