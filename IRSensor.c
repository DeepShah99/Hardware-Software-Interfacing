/*
File name: ADC.c
Description: Interacing of an IR sensor with STM32
Author: Deep Shah | 8750086
Date: 26th May 2022
*/

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "main.h"


__IO uint32_t uhADCxConvertedValue = 0;
ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef sConfig = {0};
int isADCinit = 0;  //to check whether ADC is initialized or not
float dist = 0;



// FUNCTION      : void my_init(void *data)
// DESCRIPTION   : Function responsible for necessary initilizations
// PARAMETERS    : *data
// RETURNS       : none
void my_init(void *data)
{
  /* Initialize the terminal system */
  TerminalInit();

  /* Print out reset source */
  WDTCheckReset();
  
  /* Initialize the task system */
  TaskingInit();

  my_Init();
 
} 


// FUNCTION      : my_main(void *data)
// DESCRIPTION   : Function responsible for necessary initilizations
// PARAMETERS    : *data
// RETURNS       : none
void my_main(void *data)
{
  TaskingRun();  /* Run all registered tasks */
  my_Loop();

  WDTFeed();

} 


// FUNCTION      : ParserReturnVal_t adcinit(int mode)
// DESCRIPTION   : Function responsible for ADC initilization
// PARAMETERS    : int mode
// RETURNS       : CmdReturnOk
ParserReturnVal_t adcinit(int mode)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};




  __HAL_RCC_ADC1_CLK_ENABLE();

  __HAL_RCC_GPIOC_CLK_ENABLE();

 
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  
 
    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    PC2     ------> ADC1_IN12
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    isADCinit = 1;                                              //if ADC is initialized
    printf("ADC initialized\n");

  return CmdReturnOk;
}


// FUNCTION      : ParserReturnVal_t adc(int mode)
// DESCRIPTION   : Function responsible for ADC channel activation out of three channels
// PARAMETERS    : int mode
// RETURNS       : CmdReturnOk
ParserReturnVal_t adc(int mode)
{
if(isADCinit == 1){                           //check first if ADC is initialized
uint32_t adc, adcChannel;
  adc = fetch_uint32_arg(&adcChannel);
  if(adc) {
    printf("Please supply ADC channel number from 1 to 3\n"); //if any channel is not entered
    return CmdReturnBadParameter1;
  }

while(1){
HAL_Delay(500);
float vol;
vol = 0.0008059 * uhADCxConvertedValue;   //convert raw adc value to voltage
if(mode != CMD_INTERACTIVE) return CmdReturnOk;
if(adcChannel == 1){                      //PC0
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
	if (HAL_ADC_Start(&hadc1) != HAL_OK)
	{
		/* Start Conversation Error */
		printf("HAL_ADC_Start Error\r\n");
	}
	if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
	{
		printf("HAL_ADC_PollForConversion Error\r\n");
	}
	else
	{
		/* ADC conversion completed */
		/*## Get the converted value of regular channel  ########################*/
		uhADCxConvertedValue = HAL_ADC_GetValue(&hadc1);
		WDTFeed();
	}
	
  printf("ADC Count is %li and Voltage is %f\n", uhADCxConvertedValue, vol);
  
  dist = (vol - 2.7)/0.25;
  printf("The distance in cm is %f\n", dist);
  
  }
 
  } 
  }
  else{
  printf("ADC not initialized\n");                       
  }
  return CmdReturnOk;
}

ADD_CMD("adcinit",adcinit,"                Initialize ADC")//command to initialize ADC channel
ADD_CMD("adc",adc,"                ADC channel 1")//command to select ADC channel
