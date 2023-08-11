/* Copyright 2020, Juan Manuel Cruz.
 * All rights reserved.
 *
 * This file is part of Project => freertos_app_Example5_6
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

    app.c (Released 2022-10)

--------------------------------------------------------------------

    app file for FreeRTOS - Event Driven System (EDS) - Project for
    STM32F429ZI_NUCLEO_144.

    See readme.txt for project information.

-*--------------------------------------------------------------------*/


// ------ Includes -----------------------------------------------------
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

/* Application & Tasks includes. */
#include "app.h"
#include "task_Button.h"
#include "task_Led.h"

// ------ Macros and definitions ---------------------------------------

// ------ internal data declaration ------------------------------------
/* Declare a variable of type xTaskHandle. This is used to reference tasks. */
TaskHandle_t xTaskButtonHandle;
TaskHandle_t xTaskLedHandle;

/* Declare a variable of type QueueHandle_t.  This is used to send messages from
the button task to the Led task. */
QueueHandle_t QueueHandle;

// ------ internal functions declaration -------------------------------

// ------ internal data definition -------------------------------------
const char *pcTextForMain = "freertos_app_Example5_6 is running: PO (5 de 6)\r\n\n";

// ------ external data definition -------------------------------------

// ------ internal functions definition --------------------------------

// ------ external functions definition --------------------------------


/*------------------------------------------------------------------*/
/* App Initialization */
void appInit( void )
{
	LDX_Config_t* ptr;
	BaseType_t ret;

	/* Print out the name of this Example. */
  	vPrintString( pcTextForMain );

    /* Before a queue is used it must be explicitly created.
     * The queue is created to hold a maximum of 5 long values. */
	QueueHandle = xQueueCreate( 5, sizeof( ledFlag_t ) );

	/* Check the queues was created successfully */
	configASSERT( QueueHandle != NULL );

	ptr = &LDX_Config[0];
	/* Task Led thread at priority 1 */
	ret = xTaskCreate( vTaskLed,					/* Pointer to the function thats implement the task. */
					   "Task Led",						/* Text name for the task. This is to facilitate debugging only. */
					   (2 * configMINIMAL_STACK_SIZE),	/* Stack depth in words. 				*/
	                   (void*)ptr,    				/* Pass the ptr as the task parameter. */
					   (tskIDLE_PRIORITY + 1UL),	/* This task will run at priority 1. 		*/
					   &xTaskLedHandle );			/* We are using a variable as task handle.	*/

	/* Check the task was created successfully. */
	configASSERT( ret == pdPASS );

	ptr = &LDX_Config[0];
	/* Task Button thread at priority 1 */
	ret = xTaskCreate( vTaskButton,					/* Pointer to the function thats implement the task. */
					   "Task Button",				/* Text name for the task. This is to facilitate debugging only. */
					   (2 * configMINIMAL_STACK_SIZE),	/* Stack depth in words. 				*/
	                   (void*)ptr,    				/* Pass the ptr as the task parameter. */
					   (tskIDLE_PRIORITY + 1UL),	/* This task will run at priority 1. 		*/
					   &xTaskButtonHandle );		/* We are using a variable as task handle.	*/

	/* Check the task was created successfully. */
	configASSERT( ret == pdPASS );
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
