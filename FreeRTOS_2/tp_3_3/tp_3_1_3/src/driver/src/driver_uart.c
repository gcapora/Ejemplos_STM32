/* @file   : driver_uart.c en tp_3_1_3
 * @date   : abril 2023
 * @author : GFC en base a SB
 *
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hal.h"
#include "driver_uart.h"
#include "driver_uart_port.h"

/********************** macros and definitions *******************************/

#define TX_TIMEOUT_         (100)
#define RX_TIMEOUT_         (100)

/********************** internal data declaration ****************************/
/********************** internal functions declaration ***********************/
/********************** internal data definition *****************************/
/********************** external data definition *****************************/
/********************** internal functions definition ************************/
/********************** external functions definition ************************/

void driver_uart_init(manejador_UART_t * p_uart_a_utilizar)
{
	// Mutex para garantizar que no hay interferencia entre tareas.
	// No parece ser absolutamente necesario pero es una garantía más.
	p_uart_a_utilizar->mutex = xSemaphoreCreateMutex();
	if( p_uart_a_utilizar->mutex == NULL )
	{
		while(1)
		{
			// Hubo un error inesterado: Me quedo acá. :-(
		};
	}
}

size_t driver_uart_rx(manejador_UART_t * UART, uint8_t* buffer, size_t size)
{
	// Debo poder saber cuántos caracteres recibí realmente.
	// Para eso debo leer de a 1 byte e ir contando.
	size_t estado_rx = 0;
	size_t contador_rx = 0;
	uint8_t byte_rx;

	// Loop de rx hasta alcanzar:
	// 1) cantidad size de caracteres recibidos
	// 2) o timeout
	do {
		if ( 1 == (estado_rx = driver_uart_port_hal_rx(UART->p_UART, &byte_rx, RX_TIMEOUT_)) )
		{
			buffer[contador_rx]=byte_rx;
			contador_rx++;
		}
	} while (estado_rx == 1 && contador_rx < size);

	// Devuelve los caracteres recibidos (0 si pasó el timeout)
	// y los almacenó en buffer.
	return contador_rx;
}

size_t driver_uart_tx(manejador_UART_t * p_uart, uint8_t* buffer, size_t size)
{
	xSemaphoreTake(p_uart->mutex, portMAX_DELAY);			// MUTEX preventivo
	driver_uart_port_hal_tx( p_uart->p_UART, buffer, size, TX_TIMEOUT_ );
															// Transmisión :-)
    xSemaphoreGive(p_uart->mutex);							// Libero MUTEX
	return size;
}

/********************** end of file ******************************************/
