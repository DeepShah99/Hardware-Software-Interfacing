/*
File name: simpletask.c
Description: To demonstrate a simple inter-process communication using a shared global variable 
Author: Group 8 - Deep Shah 8750086 and Jeevan Dsouza 8716171
Date: 25th May 2022
*/

#include <stdio.h>
#include <stdint.h>
#include "common.h"

/* A global variable to print the message */
static uint32_t Counter;

/* The main command to set the initial value of the counter variable */
ParserReturnVal_t CmdCount(int mode)
{
uint32_t count;
if(mode != CMD_INTERACTIVE) return CmdReturnOk;
count = 0;
fetch_uint32_arg(&count);
Counter += count;
return CmdReturnOk;
}



/* Only print the message if the counter is non-zero and after printing decrement the counter*/
void TaskCounter(void *data)
{
if(Counter) {
printf("The counter is %u\n",(unsigned)Counter);
Counter--;
}
}

ADD_CMD("count", CmdCount,
"<n> Add <n> to the message print count");




ADD_TASK(TaskCounter, /* the name of the function for the task */
NULL, /* Initialization function */
NULL, /* This pointer is passed as 'data' to the functions */
1000, /* the number of milliseconds between calls */
"This is the help text for the counter task");
