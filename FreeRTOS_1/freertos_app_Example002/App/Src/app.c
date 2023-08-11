/* Copyright 2020, Juan Manuel Cruz.
 * All rights reserved.
 *
 * This file is part of Project => freertos_app_Example001
 *
 */

/*--------------------------------------------------------------------*-

    taskA.c (Released 2022-06)

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
#include "app_Resources.h"
#include "task_A.h"
#include "task_B.h"
#include "task_Test.h"

// ------ Macros and definitions ---------------------------------------

// ------ internal data declaration ------------------------------------
/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * semaphore that is used to synchronize a task with other task. */
xSemaphoreHandle xBinarySemaphoreEntry;
xSemaphoreHandle xBinarySemaphoreExit1;
xSemaphoreHandle xBinarySemaphoreExit2;
xSemaphoreHandle xCountSemaphoreContinue;

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * mutex type semaphore that is used to ensure mutual exclusive access to ........ */
xSemaphoreHandle xMutex;

/* Declare a variable of type xTaskHandle. This is used to reference tasks. */
xTaskHandle vTaskAHandle;
xTaskHandle vTaskB1Handle;
xTaskHandle vTaskB2Handle;
xTaskHandle vTaskTestHandle;

/* Declaracion estructura para salida de autos 							 */

const uint8_t	NroSalidas = 2;

/* Task A & B Counter	*/
uint32_t	lTasksCnt;

// ------ internal functions declaration -------------------------------

// ------ internal data definition -------------------------------------
const char *pcTextForMain = "freertos_app_Example002 is running: parking lot\r\n\n";

// ------ external data definition -------------------------------------

// ------ internal functions definition --------------------------------

// ------ external functions definition --------------------------------


/*------------------------------------------------------------------*/
/* App Initialization */
void appInit( void )
{
	/* Print out the name of this Example. */
  	vPrintString( pcTextForMain );

    /* Before a semaphore is used it must be explicitly created.
     * In this example a binary semaphore is created. */
    vSemaphoreCreateBinary( xBinarySemaphoreEntry    );
    xCountSemaphoreContinue = xSemaphoreCreateCounting( lTasksCntMAX, lTasksCntMAX );
    vSemaphoreCreateBinary( xBinarySemaphoreExit1     );
    vSemaphoreCreateBinary( xBinarySemaphoreExit2     );

    /* Check the semaphore was created successfully. */
	configASSERT( xBinarySemaphoreEntry    !=  NULL );
	configASSERT( xBinarySemaphoreExit1    !=  NULL );
	configASSERT( xBinarySemaphoreExit2    !=  NULL );
	configASSERT( xCountSemaphoreContinue  !=  NULL );

    // Configuro salida/s
    AdministraSalida Salida1, Salida2;
    Salida1.xBinarySemaphoreExit_AE = xBinarySemaphoreExit1;
    Salida1.Id_AE = (uint8_t) 1;

    Salida2.xBinarySemaphoreExit_AE = xBinarySemaphoreExit2;
    Salida2.Id_AE = (uint8_t) 2;

    AdministraSalida * ptr = &Salida1;

    /* Before a semaphore is used it must be explicitly created.
     * In this example a mutex semaphore is created. */
    xMutex = xSemaphoreCreateMutex();

    /* Check the mutex was created successfully. */
    configASSERT( xMutex !=  NULL );

	BaseType_t ret;

    /* Task A thread at priority 2 */
    ret = xTaskCreate( vTaskA,						/* Pointer to the function thats implement the task. */
					   "Task A",					/* Text name for the task. This is to facilitate debugging only. */
					   (2 * configMINIMAL_STACK_SIZE),	/* Stack depth in words. 				*/
					   NULL,						/* We are not using the task parameter.		*/
					   (tskIDLE_PRIORITY + 2UL),	/* This task will run at priority 1. 		*/
					   &vTaskAHandle );				/* We are using a variable as task handle.	*/

    /* Check the task was created successfully. */
    configASSERT( ret == pdPASS );

    /* Task B thread at priority 2 */
    ptr = &Salida1;
    ret = xTaskCreate( vTaskB,						/* Pointer to the function thats implement the task. */
					   "Task B1",					/* Text name for the task. This is to facilitate debugging only. */
					   (4 * configMINIMAL_STACK_SIZE),	/* Stack depth in words. 				*/
					   (void *) ptr,						/* 		*/
					   (tskIDLE_PRIORITY + 2UL),	/* This task will run at priority 1. 		*/
					   &vTaskB1Handle );				/* We are using a variable as task handle.	*/

    /* Task B thread at priority 2 */
    ptr = &Salida2;
	// vPrintStringAndNumber("Probando Salida2", ptr->Identificador_AE );
    ret = xTaskCreate( vTaskB,						/* Pointer to the function thats implement the task. */
					   "Task B2",					/* Text name for the task. This is to facilitate debugging only. */
					   (4 * configMINIMAL_STACK_SIZE),	/* Stack depth in words. 				*/
					   (void *) ptr,						/* 		*/
					   (tskIDLE_PRIORITY + 2UL),	/* This task will run at priority 1. 		*/
					   &vTaskB2Handle );				/* We are using a variable as task handle.	*/

    /* Check the task was created successfully. */
	vPrintStringAndNumber("Probando Salida2", ptr->Id_AE );
    configASSERT( ret == pdPASS );

	/* Task Test at priority 1, periodically excites the other tasks */
    ret = xTaskCreate( vTaskTest,					/* Pointer to the function thats implement the task. */
					   "Task Test",					/* Text name for the task. This is to facilitate debugging only. */
					   (2 * configMINIMAL_STACK_SIZE),	/* Stack depth in words. 				*/
					   NULL,						/* We are not using the task parameter.		*/
					   (tskIDLE_PRIORITY + 1UL),	/* This task will run at priority 2. 		*/
					   &vTaskTestHandle );			/* We are using a variable as task handle.	*/

    /* Check the task was created successfully. */
    configASSERT( ret == pdPASS );
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
