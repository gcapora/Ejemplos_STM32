/* Copyright 2020, Juan Manuel Cruz.
 * All rights reserved.
 *
 * This file is part of Project => freertos_app_Example001
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

    task_A.c (Released 2022-06)

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
#include "task_A.h"

// ------ Macros and definitions ---------------------------------------

// ------ internal data declaration ------------------------------------

// ------ internal functions declaration -------------------------------

// ------ internal data definition -------------------------------------
/* Define the strings that will be passed in as the Supporting Functions parameters.
 * These are defined const and off the stack to ensure they remain valid when the
 * tasks are executing. */
const char *pcTextForTaskA    				= "  ==> Task    A - Running\r\n";

const char *pcTextForTaskA_WaitEntry_A      = "  ==> Task    A - Wait:   Entry_A     \r\n\n";
const char *pcTextForTaskA_WaitExit_A       = "  ==> Task    A - Wait:   Exit_A      \r\n\n";

const char *pcTextForTaskA_WaitMutex        = "  ==> Task    A - Wait:   Mutex       \r\n\n";
const char *pcTextForTaskA_SignalMutex      = "  ==> Task    A - Signal: Mutex    ==>\r\n\n";

// ------ external data definition -------------------------------------

// ------ internal functions definition --------------------------------

// ------ external functions definition --------------------------------

/*------------------------------------------------------------------*/
/* Task A thread */
void vTaskA( void *pvParameters )
{
	/* Print out the name of this task. */
	vPrintString( pcTextForTaskA );

	/* As per most tasks, this task is implemented within an infinite loop.
	 *
	 * Take the semaphore once to start with so the semaphore is empty before the
	 * infinite loop is entered.  The semaphore was created before the scheduler
	 * was started so before this task ran for the first time.*/
    xSemaphoreTake( xBinarySemaphoreEntry_A, (portTickType) 0 );
    xSemaphoreTake( xBinarySemaphoreExit_A, (portTickType) 0 );
    while( 1 )
    {
	    /* Toggle LD3 state */
		HAL_GPIO_TogglePin( LD3_GPIO_Port, LD3_Pin );

        /* Use the semaphore to wait for the event.  The task blocks
         * indefinitely meaning this function call will only return once the
         * semaphore has been successfully obtained - so there is no need to check
         * the returned value. */
    	vPrintString( pcTextForTaskA_WaitEntry_A );
    	xSemaphoreTake( xBinarySemaphoreEntry_A, portMAX_DELAY );
        {
    		/* The semaphore is created before the scheduler is started so already
    		 * exists by the time this task executes.
    		 *
    		 * Attempt to take the semaphore, blocking indefinitely if the mutex is not
    		 * available immediately.  The call to xSemaphoreTake() will only return when
    		 * the semaphore has been successfully obtained so there is no need to check
    		 * the return value.  If any other delay period was used then the code must
    		 * check that xSemaphoreTake() returns pdTRUE before accessing the resource. */
        	vPrintString( pcTextForTaskA_WaitMutex );
    		xSemaphoreTake( xMutex, portMAX_DELAY );
        	{
        		/* The following line will only execute once the semaphore has been
        		 * successfully obtained. */
    			vPrintString( pcTextForTaskA_WaitExit_A );
        		xSemaphoreTake( xBinarySemaphoreExit_A,  portMAX_DELAY );
        		{
        			/* 'Give' the semaphore to unblock the tasks. */
        			vPrintString( pcTextForTaskA_SignalMutex );
        			xSemaphoreGive( xMutex );
        		}
        	}
        }
	}
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
