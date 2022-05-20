/* 
 * Filename            : group20_lab10.c
 * Description         : measure the incremental position of a turning motor shaft. 
 * Author & Student id : Deep Shah (8750086)
                         Dhruv Patel (8743512)
 * Date                : 17th November 2021 
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "main.h"

static TIM_HandleTypeDef tim3;


// Function:     encoderinit
// Description:  Initialize the encoder
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t encoderinit(int mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // configure gpio
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = (GPIO_PIN_6| GPIO_PIN_7) ;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   __HAL_RCC_TIM3_CLK_ENABLE();

   // configure timer
   tim3.Instance = TIM3;
   tim3.Init.Prescaler = 0;			
   tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
   tim3.Init.Period = 0xffff;											
   tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   tim3.Init.RepetitionCounter = 0;
   HAL_TIM_Base_Init(&tim3);

   HAL_StatusTypeDef rc;
   TIM_Encoder_InitTypeDef encoderConfig;
   
   // configure encoder
   encoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;
   encoderConfig.IC1Polarity = 0;
   encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
   encoderConfig.IC1Prescaler = 0;
   encoderConfig.IC1Filter = 3;
   encoderConfig.IC2Polarity = 0;
   encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
   encoderConfig.IC2Prescaler = 0;
   encoderConfig.IC2Filter = 3;

   rc = HAL_TIM_Encoder_Init(&tim3,&encoderConfig);
   if(rc != HAL_OK) 
   {
     printf("Failed to initialize Timer 3 Encoder," "rc=%u\n",rc);
   }
   rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_1);
   if(rc != HAL_OK) 
   {
     printf("Failed to start Timer 3 Encoder, " "rc=%u\n",rc);
   }
   rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_2);
   if(rc != HAL_OK) 
   {
     printf("Failed to start Timer 3 Encoder, " "rc=%u\n",rc);
   }
  return CmdReturnOk;      
}
ADD_CMD("encoderinit",encoderinit," initialize encoder")

// Function:     encoder
// Description:  Display encoder count and angle of dc motor
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t encoder(int mode)
{
  int32_t deg = 0;
  int32_t value = 0;
  
    value = TIM3->CNT;				// get the count value
    deg = (value * 360)/794;			// convert value to deg
    printf("the encoder value is %ld\n",value);
    printf("the motor angle is %ld\n",deg);
  
  return CmdReturnOk;   
}
ADD_CMD("encoder",encoder," show count and angle of motor")

// Function:     encoderCount
// Description:  move dc motor for supplied count
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t encoderCount(int mode)
{
  uint32_t rc, totalcounts;
  int16_t value = 0;
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  rc = fetch_uint32_arg(&totalcounts);
  if(rc)		
  {
    printf("Enter time\n");                       //check if value entered or not 
    return CmdReturnBadParameter1;
  }

  TIM1->CCR1 = 700;			// set the timer
      
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,SET);	// set motor direction
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,RESET);  
  while(value < totalcounts)				// rotate motor till enterd value
  {
    value = TIM3->CNT;
    printf("the value of encoder is %d\n",value);
    WDTFeed();
  }
        
  value = 0; 					// reset count value
  TIM3->CNT = 0;  				// reset counter
  HAL_Delay(2000);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,RESET);	// set dc motor in opposite direction
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,SET);
  while(value < totalcounts)				// rotate motor till entered value
  {
    value = TIM3->CNT;
    printf("the value of encoder is %d\n",value);
    WDTFeed();
  }
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,RESET);	// stop dc motor
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,RESET);  
  return CmdReturnOk;
}
	
ADD_CMD("encoderCount",encoderCount,"motor rotates till supplied count")
