/* 
 * Filename:            motorDC.c
 * Description:         Control two dc motors using the driver and feed the pwm value for the speed of the motor.
                        Control how long the motor should rotate by giving a delay value.
                        Implement a motion queue for the motor as well.                       
 * Author & Student id: Group 20 Deep Shah 8750086 and Dhruv Patel 8743512
 * Date:                17th Nov 2021 
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "main.h"

//prototypes
void my_Tick(void);
void pwminit(void);
void TaskingTickHandler(void);

//global variables
int32_t channel = 0;
int32_t isChannel = 0;
int32_t pwmValue = 0;
int32_t ispwmValue = 0;
int32_t dir = 0;
int32_t isDir = 0;
int32_t isDelayValue = 0;
static int32_t delayValue = 0; 

TIM_HandleTypeDef htim1;			            // Handle for timer


// Function:     pwminit
// Description:  Initialize pwm
// Prameter:     mode
// Return:       cmdReturnOK
void pwminit(void)
{
  __HAL_RCC_TIM1_CLK_ENABLE();			            // Enable timer 1 clock
  TIM_OC_InitTypeDef sConfigOC = {0}; 
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  HAL_TIM_Base_Init(&htim1);
  HAL_TIM_PWM_Init(&htim1); 

                                                                  // Configuration of pwm
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  __HAL_RCC_GPIOA_CLK_ENABLE();	                            // GPIO port A clock enable
  
                                                                  // Configuration of GPIO port A
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                                                                  // Configuration of channel 1, 2 and 3 of timer 1
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);

                                                                  // Start channel 1, 2 and 3 of timer 1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  
}

// Function:     dc
// Description:  control two dc motors with the pwm value
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t dc(int mode)
{    
 
    isChannel = fetch_int32_arg(&channel);
    if(isChannel) 					
    {
      printf("Please enter the channel\n");       // Check for channel entered or not
      return CmdReturnBadParameter1;
    }
    
    ispwmValue = fetch_int32_arg(&pwmValue);      // Check for pwm value entered or not
    if(ispwmValue) 
    {
      printf("Please enter the pwm value\n");
      return CmdReturnBadParameter1;
    }
    if(pwmValue <0 || pwmValue > 100)             // Check for pwm value between 0 to 100 or not
    {
      printf("Please enter the pwm value between 0 to 100 percentage\n");
      return CmdReturnBadParameter1;
    }  
    pwmValue = pwmValue * 10;
    isDir = fetch_int32_arg(&dir);		
    if(isDir) 
    {
      printf("Please enter the direction\n");     // Check for direction entered or not
      return CmdReturnBadParameter1;
    }
      isDelayValue = fetch_int32_arg(&delayValue);  
      if(isDelayValue) 
    {
      printf("Please enter the delay value\n");     // Check for delay entered or not
      return CmdReturnBadParameter1;
    }
    pwminit();
     		
  if(channel == 1)
  {
    htim1.Instance->CCR1 = pwmValue;	
    
    
    if(dir == 1)                                 //clockwise
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);  
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);   
    }
    if(dir == 2)
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);     //anticlockwise
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);     
    }
    if(dir == 0)                                 //stop the motor
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);    
    }        
  }
  else if(channel == 2)
  {
    htim1.Instance->CCR2 = pwmValue;
   
    
    if(dir == 1)                                 //clockwise
    {
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,0);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,1);   
    }
    if(dir == 2)                                 //anticlockwise
    {
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1);      
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,0);       
    }
    if(dir == 0)                                 //stop the motor
    {
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,0);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,0);       
    }        
  }
  
  return CmdReturnOk;
}
   
// Function:     SysTick_Handler
// Description:  handler to control how long motor will rotate
// Prameter:     null
// Return:       void
void SysTick_Handler(void)                        
{
 

   static int count=0;                            //every millisecond this handler will be called 
  HAL_IncTick();                                  
  my_Tick();
  TaskingTickHandler();
   count++;
  if(count > delayValue){                         //when count value is greater than delay then stop the pwm channels
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); 
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
  count=0;
  }
   
}
// Function:     trap
// Description:  motion profile for trap
// Prameter:     action
// Return:       null
ParserReturnVal_t trap(int action)                //trapezoidal profile
{


    ispwmValue = fetch_int32_arg(&pwmValue);      // Check for pwm value entered or not
    if(ispwmValue) 
    {
      printf("Please enter the pwm value\n");
      return CmdReturnBadParameter1;
    }
    
  if(pwmValue <0 || pwmValue > 100) 		   // Check for pwm value between 0 to 100 or not
    {
      printf("Please enter the pwm value between 0 to 100 percentage\n");
      return CmdReturnBadParameter1;
    }  
    pwmValue = pwmValue * 10;
    isDir = fetch_int32_arg(&dir);		
    if(isDir) 
    {
      printf("Please enter the direction\n");    //check if direction entered or not
      return CmdReturnBadParameter1;
    }
    isDelayValue = fetch_int32_arg(&delayValue); 
    if(isDelayValue) 
    {
      printf("Please enter the delay value\n");  // Check for delay entered or not
      return CmdReturnBadParameter1;
    } 
    pwminit();
    
    
    if(dir == 1)                                 //clockwise
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);   
   
    }
    if(dir == 2)                                 //anticlockwise
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);      
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);    
    }
    if(dir == 0)
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);     //stop the motor
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);   
       
    }   
for(int i = 0; i<=pwmValue; i = i+10){          //speed up
htim1.Instance->CCR1 = i;
HAL_Delay(50);
WDTFeed();     
  
  }
for(int i = pwmValue; i>= 0; i= i-10){          //speed down
htim1.Instance->CCR1 = i;
HAL_Delay(50);
WDTFeed();
}
if(action==CMD_SHORT_HELP) return CmdReturnOk;
 
  
return CmdReturnOk;
}


ADD_CMD("dc",dc," insert dc motor parameteres") //command for dc motor operations
ADD_CMD("trap",trap," trap")                    //trap motion profile
