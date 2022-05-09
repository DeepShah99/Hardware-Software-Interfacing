/* 
 * Filename:            Timers_Group20 .c
 * Description:         Implement a delay in Timers using STM32F411
 * Author & Student id: Deep Shah and Dhruv Patel
 * Date:                7th Oct 2021 
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"
#include "main.h"

TIM_HandleTypeDef tim2;                      

//prototype
void TIM_IRQHandler(int delayMilli);

//global variables
uint32_t isDelayEntered;            
uint32_t delayMicro = 0;
int isTimerInit = 0;

// Function:     timerinit
// Description:  Initialize timer 
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t timerinit(int mode)
{
  /* Timer Initialization */


  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  __HAL_RCC_TIM2_CLK_ENABLE();
  
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim2.Init.Period = 0xffff;
  tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim2.Init.RepetitionCounter = 0;

  printf("Timer initialized successfully...\n");
  isTimerInit = 1;
  
  return CmdReturnOk;

}

// Function:     timer
// Description:  Get value from terminal and produce a delay
// Prameter:     mode
// Return:       delayMicro
ParserReturnVal_t timerdelay(int mode)
{
if(isTimerInit == 1){                              //check first if timer is initialized
  HAL_TIM_Base_Init(&tim2);
  HAL_NVIC_SetPriority(TIM2_IRQn,0,0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  isDelayEntered = fetch_uint32_arg(&delayMicro);

  if(isDelayEntered) {
    printf("Please enter the delay in Microsecond\n");
    return CmdReturnBadParameter1;
  }
  
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
    printf("Counter completed\n");
  }
  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,0);						
 
  HAL_Delay(1000);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  TIM_IRQHandler(delayMicro);
  printf("IRQ Handler executed\n");
  }
  else{
  printf("please initialize the timer");          //print the command if timer not initialized
  }
  return delayMicro;
}

// Function:     TIM_IRQHandler
// Description:  Initialize interrupt
// Prameter:     delayMilli
// Return:       none
void TIM_IRQHandler(int delayMilli)
{
    delayMilli = delayMilli/1000;                           //Convert microseconds to milliseconds
    HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,1);
    HAL_Delay(delayMilli);
    HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,0);
}


ADD_CMD("timerinit",timerinit,"                timer initialize...")   //command to initialize timer 
ADD_CMD("timerdelay",timerdelay,"<channel>       get timer value for the delay");//command to feed delay in us


