/* Copyright 2020, Juan Manuel Cruz.
 * All rights reserved.
 *
 * This file is part of Project => freertos_app_Example000
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


/*--------------------------------------------------------------------*-

    taskStats.c (Released 2022-06)

--------------------------------------------------------------------

    task file for FreeRTOS - Event Driven System (EDS) - Project for
    STM32F429ZI_NUCLEO_144.

    See readme.txt for project information.

-*--------------------------------------------------------------------*/


// ------ Includes -------------------------------------------------
/* Project includes. */
#include "main.h"
#include "cmsis_os.h"

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Demo includes. */
#include "supporting_Functions.h"

/* Application includes. */
#include "app_Resources.h"
#include "taskStats.h"

// ------ Macros and definitions ---------------------------------------

// ------ internal data declaration ------------------------------------

// ------ internal functions declaration -------------------------------

// ------ internal data definition -------------------------------------
/* Define the strings that will be passed in as the Supporting Functions parameters.
 * These are defined const and off the stack to ensure they remain valid when the
 * tasks are executing. */
const char *pcTextForTaskStats    			= " - Running\r\n";

// ------ Private variables ----------------------------------------

// ------ internal data definition -------------------------------------

// ------ external data definition -------------------------------------

// ------ internal functions definition --------------------------------

// ------ external functions definition --------------------------------

/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
void configureTimerForRunTimeStats(void)
{
    ulHighFrequencyTimerTicks = 0;
	HAL_TIM_Base_Start( &htim2 );
}

unsigned long getRunTimeCounterValue(void)
{
	return ulHighFrequencyTimerTicks;
}

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
	/* Run time stack overflow checking is performed if
	   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
	   called if a stack overflow is detected.
	   https://www.freertos.org/Stacks-and-stack-overflow-checking.html */
	vPrintTwoStrings( " Application Stack Overflow!! on Task:", ( char* )pcTaskName );

    taskENTER_CRITICAL();
    configASSERT( 0 );   /* hang the execution for debugging purposes */
    taskEXIT_CRITICAL();
}

/*------------------------------------------------------------------*/
/* Task Function thread */
void vTaskStats( void *pvParameters )
{
	/*  Declare & Initialize Task Function variables */
	portTickType xLastExecutionTime;

	/* The buffer used to hold the run time stats text needs to be quite large.  It
	   is therefore declared static to ensure it is not allocated on the task stack.
	   This makes this function non re-entrant. */
	static char cStringBuffer[ 512 ];

	/* The task will run every 5 seconds. */
	const uint32_t xBlockPeriod = 5000 / portTICK_RATE_MS;

	/* Initialise xLastExecutionTime to the current time.  This is the only
	   time this variable needs to be written to explicitly.  Afterwards it is
	   updated internally within the vTaskDelayUntil() API function. */
	xLastExecutionTime = xTaskGetTickCount();

	char *pcTaskName = ( char * ) pcTaskGetName( NULL );

	/* Print out the name of this task. */
	vPrintTwoStrings( pcTaskName, pcTextForTaskStats );

	/* As per most tasks, this task is implemented in an infinite loop. */
    while( 1 )
	{
		/* Wait until it is time to run this task again. */
		vTaskDelayUntil( &xLastExecutionTime, xBlockPeriod );

		/* Generate a text table from the run time stats.  This must fit into
		the cStringBuffer array.
		https://www.freertos.org/a00021.html#vTaskGetRunTimeStats */
		vTaskGetRunTimeStats( cStringBuffer );

		/* Print out column headings for the run time stats table. */
		vPrintString( "\nTask\t\tAbs\t\t%\n" );
		vPrintString( "------------------------------------\n" );

		/* Print out the run time stats themselves. */
		/* Shows the amount of time each task has spent in the Running state
		   (how much CPU time each task has consumed). */
		vPrintString( cStringBuffer );

		/* Generate a text table from the task list. This must fit into
		the cStringBuffer array.
		https://www.freertos.org/a00021.html#vTaskList */
		vTaskList( cStringBuffer );

		/* Print out column headings for the task list table. */
		vPrintString( "\nTask\t\tState\tPrior.\tStack\tNum\n" );
		vPrintString( "-------------------------------------------\n" );

		/* Print out the run task list themselves. */
		/* Shows the state of each task, including the task's stack high water mark
		   (the smaller the high water mark number the closer the task has come to
		   overflowing its stack). */
		vPrintString( cStringBuffer );
	}
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
