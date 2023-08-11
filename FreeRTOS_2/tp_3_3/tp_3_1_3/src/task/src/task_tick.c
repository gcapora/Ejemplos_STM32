/******************************************************************************
* @file   : task_tick.c en tp_3_1_3
* @date   : Abril 2023
* @author : GFC en base a SB
******************************************************************************/

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "driver_uart.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (1000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern manejador_UART_t Mi_UART;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_tick(void *args)
{
	while (true)
	{
		vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
		driver_uart_tx(&Mi_UART, (uint8_t *) "{tick}", 6);
	}
}

/********************** end of file ******************************************/
