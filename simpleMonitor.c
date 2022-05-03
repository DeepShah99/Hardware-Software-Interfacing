/*
File name: Assignment_1.c
Description: Initilizing serial monitor
Author: Deep Shah and Dhruv Patel
Date: 22nd Sept 2021
*/
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"

//Function: ParserReturnVal_t gpio(int action)
//Description: function to display a message on serial monitor
//Parameters: action
//Returns: integer 
ParserReturnVal_t CmdTest2(int action)
{

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("testasm\n\n"
	   "This command tests assignment-1\n"
	   );

    return CmdReturnOk;
  }
  
  printf("Hello World!\n"); //print this message when cmdtest2 function is called

  return CmdReturnOk;
}

ADD_CMD("cmdtest2",CmdTest2,"Test assignment-1 demo") //display the given message on serial monitor and call the CmdTest2 function as well



