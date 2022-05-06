/*
File name: ADCGroup20.c
Description: Implementing a 3 channel ADC which displays raw ADC value, corresponding voltage, a graphical range
Author: Group 20 - Deep Shah and Dhruv Patel
Date: 29th Sept 2021
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
void plotGraph(void); 
void goToPos(int row, int col);


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
  if(adcChannel > 3 || adcChannel < 1) {
    printf("Enter the channel only from 1 to 3\n"); //if channel entered is not from 1,2 and 3
    return CmdReturnBadParameter1;
  }
  
while(1){
HAL_Delay(20);
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
	}
	
  printf("ADC1 Count is %li and Voltage is %f", uhADCxConvertedValue, vol);
  plotGraph();    //plot the graphical range
  
  }
  if(adcChannel == 2){                         //PC1
  sConfig.Channel = ADC_CHANNEL_11;
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
	if (HAL_ADC_PollForConversion(&hadc1, 11) != HAL_OK)
	{
		printf("HAL_ADC_PollForConversion Error\r\n");
	}
	else
	{
		/* ADC conversion completed */
		/*##- Get the converted value of regular channel  ########################*/
		uhADCxConvertedValue = HAL_ADC_GetValue(&hadc1);
	}
	
  printf("ADC2 Count is %li and Voltage is %f", uhADCxConvertedValue, vol);
  plotGraph();     //plot the graphical range

  
  }
  if(adcChannel == 3){                               //PC2
  sConfig.Channel = ADC_CHANNEL_12;
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
	if (HAL_ADC_PollForConversion(&hadc1, 12) != HAL_OK)
	{
		printf("HAL_ADC_PollForConversion Error\r\n");
	}
	else
	{
		/* ADC conversion completed */
		/*## Get the converted value of regular channel  ########################*/
		uhADCxConvertedValue = HAL_ADC_GetValue(&hadc1);
	}
	
  printf("ADC3 Count is %li and Voltage is %f", uhADCxConvertedValue, vol);
  plotGraph();     //plot the graphical range
  
  }
  } 
  }
  else{
  printf("ADC not initialized\n");                       
  }
  return CmdReturnOk;
}

// FUNCTION      : void plotGraph(void)
// DESCRIPTION   : Function responsible to print graphical range based on the given raw ADC value
// PARAMETERS    : none
// RETURNS       : none
void plotGraph(void){

     int scaledDownADC = uhADCxConvertedValue/ 100;  //scale down For eg - for 4000 rawADCvalue 40 stars will be prnted 
  goToPos(45,45); // On this position graphical range will be printed
printf("The range is ");
          for (int j=0; j<=scaledDownADC; j++) // Condition to print * based on scaled downed raw adc

        {  printf("*");
        if(j == scaledDownADC){
        printf("\n");                          // go to a new line if all stars are printed
        }
        }
          
       
  }
  
 
// FUNCTION      : void goToPos(int row, int col)
// DESCRIPTION   : Function responsible to go to a particular coordinate for the plotGraph function
// PARAMETERS    : int row, int col
// RETURNS       : none
  void goToPos(int row, int col) {
  printf("\033[%d;%dH", row, col);    //source - stackoverflow
}

    

ADD_CMD("adcinit",adcinit,"                Initialize ADC")//command to initialize ADC channel

ADD_CMD("adc",adc,"                ADC channel 1|2|3")//command to select ADC channel
