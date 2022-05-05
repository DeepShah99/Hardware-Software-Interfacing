/*
File name: Assignment_2.c
Description: Implementing GPIO on pin 0,1,6 on port A to toggle LED on and off.
Author: Group 20 - Deep Shah and Dhruv Patel
Date: 22nd Sept 2021
*/
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"
#include "main.h"
//Function: ParserReturnVal_t gpio(int mode)
//Description: function to toggle pin 0,1,6 on port A
//Parameters: mode
//Returns: integer 
ParserReturnVal_t gpio(int mode)
{
  uint32_t ledOnOff,ledFetch, pin, pinFetch;
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  
  pinFetch = fetch_uint32_arg(&pin);  //fetch the pin
  if(pinFetch) {
    printf("Please supply pin number from 0 to 4 for port A\n"); //if any pin is not entered print the given message
    return CmdReturnBadParameter1;
  }
  if(pin > 4 || pin < 0) {
    printf("Enter the pin only from 0 to 4"); //check if the pin is between 0 and 2 for the led to turn on/off
    return CmdReturnBadParameter1;
  }
  ledFetch = fetch_uint32_arg(&ledOnOff); //fetch the value
  if(ledFetch) {
    printf("Please supply 1 | 0 to turn on or off the LED\n"); //if any value is not entered print the given message
    return CmdReturnBadParameter1;
  }
  if(ledOnOff > 1 || ledOnOff < 0) {
    printf("Enter only 1 | 0 to turn on/off\n"); //check if the number is  0 or 1 for the led to turn on/off
    return CmdReturnBadParameter1;
  }
  
  switch(pin){
  case 0:   HAL_GPIO_WritePin(DS1_GPIO_Port,DS1_Pin,ledOnOff); //PA0
  break;
  case 1:   HAL_GPIO_WritePin(DS2_GPIO_Port,DS2_Pin,ledOnOff); //PA1
  break;
  case 2:   HAL_GPIO_WritePin(DS_GPIO_Port,DS_Pin,ledOnOff); //PA6
  break;
  case 3:   HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,ledOnOff); //PA5 // On board LED
  break;
  case 4:   HAL_GPIO_WritePin(DS1_GPIO_Port,DS1_Pin,ledOnOff); //control all the leds
            HAL_GPIO_WritePin(DS2_GPIO_Port,DS2_Pin,ledOnOff); 
            HAL_GPIO_WritePin(DS_GPIO_Port,DS_Pin,ledOnOff);
            HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,ledOnOff);
  break;
  }

  return CmdReturnOk;
}
ADD_CMD("gpio",gpio,"                led On/Off")//print the given statements and call the gpio function as well
