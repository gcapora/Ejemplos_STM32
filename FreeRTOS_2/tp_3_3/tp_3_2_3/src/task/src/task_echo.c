/*
 * @file   : task_echo.c en tp_3_2_3
 * @date   : Abril 2023
 * @author : GFC en base a SB
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "app.h"
#include "eringbuffer.h"
#include "driver_uart.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (10)
#define BUFFER_SIZE_TASK_		(64)	// En principio, debe ser menor que
										// el tamaño del ringbuffer.
										// Sino, estaríamos desperdiciando memoria.

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern manejador_UART_t mi_UART;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_echo(void *args)
{

  while(true)
  {
    uint8_t paquete[RX_BUFFER_SIZE_];
    size_t tamanio;
    if ( 0 < (tamanio = driver_uart_rx(&mi_UART, paquete, RX_BUFFER_SIZE_)) )
    {
      driver_uart_tx(&mi_UART, paquete, tamanio);
    }

    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** end of file ******************************************/
