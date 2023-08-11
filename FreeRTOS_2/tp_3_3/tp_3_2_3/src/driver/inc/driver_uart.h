/*
 * @file   : driver_uart.h en tp_3_2_3
 * @date   : Apr 1, 2023
 * @author : NOMBRE <MAIL>
 * @version	v1.0.0
 */

#ifndef DRIVER_INC_DRIVER_UART_H_
#define DRIVER_INC_DRIVER_UART_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "eringbuffer.h"
#include "driver_uart_port.h"

/********************** macros ***********************************************/

#define TX_BUFFER_SIZE_         (32)
#define RX_BUFFER_SIZE_         (32)

/********************** typedef **********************************************/

typedef struct {
	fabricante_UART_t * p_UART_fabricante;
	bool tx_free_;
	SemaphoreHandle_t	mutex_tx_;
	eringbuffer_t rb_tx_;
	eringbuffer_t rb_rx_;
	uint8_t tx_buffer_[TX_BUFFER_SIZE_];
	uint8_t rx_buffer_[RX_BUFFER_SIZE_];
	uint8_t * rb_tx_buffer_;
	uint8_t * rb_rx_buffer_;
} manejador_UART_t;
	// Esta estructura es el manejador en sí de nuestro driver.
	// Incluye el puntero a la estructura del fabricvante,
	// y otros datos para su utilización.

/********************** external data declaration ****************************/
/********************** external functions declaration ***********************/

void driver_uart_init(manejador_UART_t * p_manejador_uart);
size_t driver_uart_rx(manejador_UART_t * pmu, uint8_t* buffer, size_t size);
size_t driver_uart_tx(manejador_UART_t * pmu, uint8_t* buffer, size_t size);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* DRIVER_INC_DRIVER_UART_H_ */
/********************** end of file ******************************************/

