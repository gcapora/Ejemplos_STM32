/* Copyright 2020, Juan Manuel Cruz.
 * All rights reserved.
 *
 * This file is part of Project => freertos_app_Example7_6
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

    task_Led.c (Released 2022-10)

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
#include "task_Led.h"

// ------ Macros and definitions ---------------------------------------

// ------ internal data declaration ------------------------------------

// ------ internal functions declaration -------------------------------

// ------ internal data definition -------------------------------------
/* Define the strings that will be passed in as the Supporting Functions parameters.
 * These are defined const and off the stack to ensure they remain valid when the
 * tasks are executing. */
const char *pcTextForTask_LDXTOn		= " - LDX turn On \r\n";
const char *pcTextForTask_LDXTOff		= " - LDX turn Off\r\n";
const char *pcTextForTask_BinSemTaken	= "    - Binary Semaphore was taken\r\n";
const char *pcTextForTask_MutSemTaken	= "    - Mutex  Semaphore was taken\r\n";
const char *pcTextForTask_MutSemGiven	= "    - Mutex  Semaphore was given\r\n";

#define			ledTickCntMAX		pdMS_TO_TICKS( 250UL )

LDX_Config_t	LDX_Config[] 	= { { LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET, NotBlinking, 0 },
							  	    { LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET, NotBlinking, 0 }, \
									{ LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET, NotBlinking, 0 } };

// ------ external data definition -------------------------------------

// ------ internal functions definition --------------------------------

// ------ external functions definition --------------------------------

/*------------------------------------------------------------------*/
/* Task Led thread */
void vTaskLed( void *pvParameters )
{
	/*  Declare & Initialize Task Function variables for argument, led, button and task */
	LDX_Config_t * ptr = (LDX_Config_t *) pvParameters;

	uint32_t xLastWakeTime;

	/* The xLastWakeTime variable needs to be initialized with the current tick
	   count. ws*/
	xLastWakeTime = osKernelSysTick();

	char *pcTaskName = (char *) pcTaskGetName( NULL );

	/* Print out the name of this task. */
	vPrintTwoStrings( pcTaskName, "   - is running\r\n" );

	/* As per most tasks, this task is implemented within an infinite loop.
	 *
	 * Take the semaphore once to start with so the semaphore is empty before the
	 * infinite loop is entered.  The semaphore was created before the scheduler
	 * was started so before this task ran for the first time.*/
    xSemaphoreTake( BinarySemaphoreHandle, (portTickType) 0 );

    /* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Check Led Flag */
		if( ptr->ledFlag == Blinking )
		{
			/* Check, Update and Print Led State */
		   	if( ptr->ledState == GPIO_PIN_RESET )
		   	{
		   		ptr->ledState = GPIO_PIN_SET;
            	vPrintTwoStrings( pcTaskName, pcTextForTask_LDXTOn );
		   	}
	    	else
	    	{
	    		ptr->ledState = GPIO_PIN_RESET;
            	vPrintTwoStrings( pcTaskName, pcTextForTask_LDXTOff );
		   	}
			/* Update HW Led State */
		   	HAL_GPIO_WritePin( ptr->LDX_GPIO_Port, ptr->LDX_Pin, ptr->ledState );
		}

		/* Check Binary Semaphore */
		if( xSemaphoreTake( BinarySemaphoreHandle, (portTickType) 0 ) == pdTRUE )
		{
        	vPrintTwoStrings( pcTaskName, pcTextForTask_BinSemTaken );

        	/* Check Mutex Semaphore */
        	vPrintTwoStrings( pcTaskName, pcTextForTask_MutSemTaken );
    		xSemaphoreTake( MutexHandle, portMAX_DELAY );
        	{
        		/* The following line will only execute once the semaphore has been
        		 * successfully obtained. */

            	/* Update Led Flag */
    			ptr->ledFlag = (ledFlag_t)ledFlag;

       			/* 'Give' the semaphore to unblock the tasks. */
            	vPrintTwoStrings( pcTaskName, pcTextForTask_MutSemGiven );
       			xSemaphoreGive( MutexHandle );
        	}
		}
		/* We want this task to execute exactly every 250 milliseconds. */
		vTaskDelayUntil( &xLastWakeTime, ledTickCntMAX );
	}
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
