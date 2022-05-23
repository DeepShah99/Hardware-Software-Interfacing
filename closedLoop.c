/* 
 * Filename:            closedLoop.c
 * Description:         Implementing closed loop monitor control using PID algorithm                    
 * Author & Student id: Group 20 Deep Shah 8750086 and Dhruv Patel 8743512
 * Date:                9th Dec 2021 
 *
 */
 
//private includes
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"

//typedef for timers
TIM_HandleTypeDef tim1;
TIM_HandleTypeDef tim3;

//variables
static int32_t previous = 0;
static int32_t sensorValue = 0;
static int32_t controlValue = 0;
static int32_t errorValue = 0;
static int32_t setPoint = 0;
static int32_t Kproportional = 0;
static int32_t readValue = 0;

//prototypes
void pwmDCEncoder_Init(void);
void PIDLoop(void);

ParserReturnVal_t pidInit(int mode)    //set pwm, dc, encoder                
{
	if(mode != CMD_INTERACTIVE) return CmdReturnOk;
	pwmDCEncoder_Init();
	return CmdReturnOk;
}
ParserReturnVal_t pidStart(int mode)   //get the required input
{
  	if(mode != CMD_INTERACTIVE) return CmdReturnOk;
	fetch_int32_arg(&setPoint);
	fetch_int32_arg(&Kproportional);
	PIDLoop();
  return CmdReturnOk;
}



void pwmDCEncoder_Init(void)
{
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = (GPIO_PIN_8| GPIO_PIN_9 |GPIO_PIN_10);
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	tim1.Instance = TIM1;
	tim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000;
	tim1.Init.CounterMode   = TIM_COUNTERMODE_UP;
	tim1.Init.Period        = 1000;
	tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim1.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&tim1);

	TIM_OC_InitTypeDef sConfig;
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.Pulse        = 0;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
	sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig,TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig,TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_3);
	
	
	
	
	
	
	HAL_StatusTypeDef rc;
	TIM_Encoder_InitTypeDef encoderConfig;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	

	GPIO_InitStruct.Pin = (GPIO_PIN_6| GPIO_PIN_7);
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	__HAL_RCC_TIM3_CLK_ENABLE();
	tim3.Instance = TIM3;
	tim3.Init.Prescaler = 0;
	tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim3.Init.Period = 0xffff;
	tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim3.Init.RepetitionCounter = 0;
	rc = HAL_TIM_Base_Init(&tim3);
	if(rc != HAL_OK) 
		{
		printf("Failed to initialize Timer 3 Base, "         			"rc=%u\n",rc);
		return;
		}
	
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
		printf("Timer three encoder initialization failed,"         			"rc=%u\n",rc);
		return;
		}
	rc = HAL_TIM_Encoder_Init(&tim3,&encoderConfig);
	if(rc != HAL_OK) 
		{
		printf("Timer three encoder initialization failed,"         			"rc=%u\n",rc);
		return;
		}
	rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_1);
	if(rc != HAL_OK) 
		{
		printf("Timer three encoder initialization failed, "         			"rc=%u\n",rc);
		return;
		}
	rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_2);
	if(rc != HAL_OK) 
		{
		printf("Timer three encoder initialization failed, "          			"rc=%u\n",rc);
		return;
		}




	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();


	GPIO_InitStruct.Pin = (GPIO_PIN_0| GPIO_PIN_1);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = (GPIO_PIN_10| GPIO_PIN_11);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0| GPIO_PIN_1,0);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10| GPIO_PIN_11,0);

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	TIM4->PSC = 72;
	TIM4->ARR = 1000;
	TIM4->CR1 |= TIM_CR1_URS;
	TIM4->DIER |= TIM_DIER_UIE;
	TIM4->EGR |= TIM_EGR_UG;

	NVIC_EnableIRQ(TIM4_IRQn);
	
	printf("dc and encoder initialized\n");
}



void PIDLoop()
{
	readValue =  TIM3->CNT;
	printf("readValue = %ld\n", readValue);
	sensorValue = readValue - previous;
	printf("sensorValue = %ld\n", sensorValue);
	previous = readValue;
	printf("previous = %ld\n", previous);
	errorValue = setPoint - sensorValue;
	printf("errorValue = %ld\n", errorValue);
	controlValue = Kproportional*errorValue;
	printf("controlValue = %ld\n", controlValue);
	
	if(controlValue<0)
	{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
		if(readValue<controlValue)
		{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
		}
	}
	
	if(readValue<=controlValue)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
	}
	else if (readValue>controlValue)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
	}
	else if (readValue==controlValue)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
	}

	if(controlValue==0)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
	}
}


ADD_CMD ("loopinit",pidInit,"initialize dc,encoder,pwm")
ADD_CMD("setspeed",pidStart," setspeed")
