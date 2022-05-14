/* 
 * Filename:            pwm.c
 * Description:         Initialize pwm in c and configure three channels of the timer.
                        Turn on individiual channel or all three pwm channels.
                        Make different profile like breathing, sine wave and parabolic.
                        Give the pwm value as a percentage.                       
 * Author & Student id: Group 20 Deep Shah 8750086 and Dhruv Patel 8743512
 * Date:                3rd Nov 2021 
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "common.h"

/* This include will give us the CubeMX generated defines */
#include "main.h"

/* This function is called from the CubeMX generated main.c, after all
 * the HAL peripherals have been initialized. */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);
void set_rgb(uint8_t red, uint8_t green, uint8_t blue);
int32_t channel = 0;
int32_t isChannelEntered = 0;
int32_t pwmValue = 0;
int32_t ispwmValueEntered = 0;
 
 TIM_HandleTypeDef htim1;
 
 ParserReturnVal_t pwminit(int action)
{

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  __HAL_RCC_TIM1_CLK_ENABLE();                          //enable the clock
TIM_ClockConfigTypeDef sClockSourceConfig = {0};
TIM_MasterConfigTypeDef sMasterConfig = {0};
TIM_OC_InitTypeDef sConfigOC = {0};
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

/* USER CODE BEGIN TIM1_Init 1 */

/* USER CODE END TIM1_Init 1 */
htim1.Instance = TIM1;
htim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000 - 1;  //get the prescalar value
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

 ParserReturnVal_t pwm(int action)
{

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
    isChannelEntered = fetch_int32_arg(&channel);
    if(isChannelEntered) {
    printf("Please enter the channel\n");             //Check if channel is entered or not
    return CmdReturnBadParameter1;
  }
  if(channel > 3 || channel < 1){
    printf("Please enter a valid channel\n");   //value should be between 0 and 100
    return CmdReturnBadParameter1;
  }
    ispwmValueEntered = fetch_int32_arg(&pwmValue);
    if(ispwmValueEntered) {
    printf("Please enter the pwm value in percentage\n");   //value should be in percentage
    return CmdReturnBadParameter1;
  }
  if(pwmValue > 100 || pwmValue < 0){
    printf("Please enter the valid value in percentage\n");   //value should be between 0 and 100
    return CmdReturnBadParameter1;
  }
pwmValue = pwmValue * 2.55;                           //convert the percentage into raw pwm value
if(channel == 1){                                     //check if channel one is selected
set_rgb(pwmValue, 0, 0);
}
else if(channel == 2){                                //check if channel two is selected
set_rgb(0, pwmValue, 0);
}
else if(channel == 3){                                //check if channel three is selected
set_rgb(0, 0, pwmValue);
}
  
   return CmdReturnOk;
}
  
ParserReturnVal_t sine(int action)                    //sine wave profile
{
                                                      
if(action==CMD_SHORT_HELP) return CmdReturnOk;
while(1){
for(int i = 100; i<=200; i++){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed();
}
for(int i = 200; i>=100; i--){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed();
}
for(int i = 100; i>=0; i--){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed();
}
for(int i = 0; i<=100; i++){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed();
}
}
return CmdReturnOk;
}

ParserReturnVal_t breath(int action)                   //linear profile
{
while(1){
if(action==CMD_SHORT_HELP) return CmdReturnOk;
for(int i = 0; i<=100; i++){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed(); 
}
for(int i = 100; i>=0; i--){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed();
}
}
return CmdReturnOk;
}
ParserReturnVal_t para(int action)                   //parabolic profile
{
while(1){
for(int i = 100; i>=0; i--){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed();
}
if(action==CMD_SHORT_HELP) return CmdReturnOk;
for(int i = 0; i<=100; i++){
htim1.Instance->CCR1 = i;
HAL_Delay(20);
WDTFeed(); 
}

}
return CmdReturnOk;
}
ParserReturnVal_t glowThree(int action)
{
 if(action==CMD_SHORT_HELP) return CmdReturnOk;
 ispwmValueEntered = fetch_int32_arg(&pwmValue);
    if(ispwmValueEntered) {
    printf("Please enter the pwm value\n");
    return CmdReturnBadParameter1;
  }
  pwmValue = pwmValue * 2.55;
  set_rgb(pwmValue, pwmValue, pwmValue);                   //activate all three channels
  
 return CmdReturnOk;
}
void set_rgb(uint8_t red, uint8_t green, uint8_t blue) { //feed the raw pwm values into individual channels
	htim1.Instance->CCR1 = red;
	htim1.Instance->CCR2 = green;
	htim1.Instance->CCR3 = blue;
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
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;    //Initialize PA8, PA9, PA10
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM1_MspPostInit 1 */

  /* USER CODE END TIM1_MspPostInit 1 */
  }

}
ADD_CMD("pwminit",pwminit,"initialize pwm")     
ADD_CMD("pwm",pwm," pwm")                        
ADD_CMD("sine",sine," sine")
ADD_CMD("breath",breath," breath")
ADD_CMD("glowThree",glowThree," glowThree")
ADD_CMD("para",para," para")
