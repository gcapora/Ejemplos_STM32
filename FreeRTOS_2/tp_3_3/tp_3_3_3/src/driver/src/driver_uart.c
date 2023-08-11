/*******************************************************************************
* @file   : driver_uart.c en tp_3_2_3
* @date   : Abril 2023
* @author : GFC en base a SB
******************************************************************************/

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "driver_uart.h"
#include "eringbuffer.h"

/********************** macros and definitions *******************************/

#define RB_TX_BUFFER_SIZE_      (128)
#define RB_RX_BUFFER_SIZE_      (128)

/********************** internal data declaration ****************************/
/********************** internal functions declaration ***********************/
/********************** internal data definition *****************************/

uint8_t	num_UARTs = 0;
manejador_UART_t * p_UART_activa[MAX_NUM_UARTS];

/********************** external data definition *****************************/
/********************** internal functions definition ************************/
/********************** external functions definition ************************/

/******************************************************************************
* @function:	driver_uart_init
* @brief:		Inicializa un puerto UART y una memoria intermedia anillo.
******************************************************************************/
manejador_UART_t * driver_uart_init(manejador_UART_t * pmu)
{
	// Primero verifico que la UART no haya sido inicializada previamente:
	if ( NULL != driver_uart_identificar_manejador(pmu->p_UART_fabricante)) return NULL;

	// Luego verifico que haya espacio para memoria intermedia anillo...
	// y la asigno al puntero en la estructura.
	pmu->rb_rx_buffer_ = (uint8_t *) cese_memoria_asignar(RB_RX_BUFFER_SIZE_);
	if (NULL == pmu->rb_rx_buffer_ ) return NULL;
	pmu->rb_tx_buffer_ = (uint8_t *) cese_memoria_asignar(RB_TX_BUFFER_SIZE_);
	if (NULL == pmu->rb_tx_buffer_ ) {
		cese_memoria_liberar( pmu->rb_rx_buffer_ );
		return NULL;
	}

	// Tengo una nueva UART activa:
	p_UART_activa[num_UARTs] = pmu;
	num_UARTs ++;
	if (num_UARTs > MAX_NUM_UARTS) Error_Handler();	// Más UARTs que las posibles

	// Flag y mutex de tx
	pmu->tx_free_ = true;
	pmu->mutex_tx_ = cese_mutex_crear();

	// Inicializo la memoria intermedia anillo de tx y rx
	eringbuffer_init(&pmu->rb_tx_, pmu->rb_tx_buffer_, RB_TX_BUFFER_SIZE_);
	eringbuffer_init(&pmu->rb_rx_, pmu->rb_rx_buffer_, RB_RX_BUFFER_SIZE_);

	// Activamos recepción...
	cese_UART_esperar_recepcion(pmu->p_UART_fabricante, pmu->rx_buffer_, RX_BUFFER_SIZE_);

	return pmu;
}

/******************************************************************************
* @function:	driver_uart_identificar_manejador
* @brief:		Busca el manejador del driver a partir del puntero a UART del fabricante
******************************************************************************/
manejador_UART_t * driver_uart_identificar_manejador (fabricante_UART_t * puf)
{
	//static bool utilizado = false;
	manejador_UART_t * salida = NULL;

	for ( uint8_t i=0; i<num_UARTs; i++ ) {
		if ( puf == p_UART_activa[i]->p_UART_fabricante ) {
			salida = p_UART_activa[i];
			i = num_UARTs;
			//if (!utilizado) {
			//	HAL_UART_Transmit_IT(salida->p_UART_fabricante, (uint8_t *) "[ENCONTRADO]", 12);
			//	utilizado = true;
			//}
		}
	}
	return salida;
}

/******************************************************************************
* @function:	driver_uart_rx
* @brief:		Lee datos del ringbuffer recibidos desde UART.
******************************************************************************/
size_t driver_uart_rx(manejador_UART_t * pmu, uint8_t* buffer, size_t size)
{
  return eringbuffer_read(&pmu->rb_rx_, buffer, size);
}

/******************************************************************************
* @function:	driver_uart_tx
* @brief:		Carga datos al ringbuffer para transmitir a UART.
******************************************************************************/
size_t driver_uart_tx(manejador_UART_t * pmu, uint8_t* buffer, size_t size)
{
	//taskENTER_CRITICAL();
	cese_mutex_tomar(pmu->mutex_tx_);
	size_t ret = eringbuffer_write(&pmu->rb_tx_, buffer, size);
	if (pmu->tx_free_)
	{
		size_t tx_len = eringbuffer_read(&pmu->rb_tx_, pmu->tx_buffer_, TX_BUFFER_SIZE_);
		cese_UART_transmitir(pmu->p_UART_fabricante, pmu->tx_buffer_, tx_len);
		// tx_free_ = false;
		pmu->tx_free_ = false;
	}
	//taskEXIT_CRITICAL();
	cese_mutex_devolver(pmu->mutex_tx_);
	return ret;
}

/********************** end of file ******************************************/
