/*
 * @file   : driver_uart.c en tp_3_2_3
 * @date   : Abril 2023
 * @author : GFC en base a SB
 */

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

static manejador_UART_t * driver_uart_identificar_manejador (fabricante_UART_t * );

/********************** internal data definition *****************************/

uint8_t	num_UARTs = 0;
manejador_UART_t * p_UART_activa[MAX_NUM_UARTS];

//static bool tx_free_;
//static eringbuffer_t rb_tx_;
//static eringbuffer_t rb_rx_;
//static uint8_t tx_buffer_[TX_BUFFER_SIZE_];
//static uint8_t rx_buffer_[RX_BUFFER_SIZE_];
//static uint8_t rb_tx_buffer_[RB_TX_BUFFER_SIZE_];
//static uint8_t rb_rx_buffer_[RB_RX_BUFFER_SIZE_];

/********************** external data definition *****************************/

//extern UART_HandleTypeDef huart3;

/********************** internal functions definition ************************/

/******************************************************************************
* @function:	driver_uart_identificar_manejador
* @brief:		Busca el manejador del driver a partir del puntero a UART del fabricante
******************************************************************************/
static manejador_UART_t * driver_uart_identificar_manejador (fabricante_UART_t * puf)
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

/********************** external functions definition ************************/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  // TODO:
}

/******************************************************************************
* @function:	HAL_UARTEx_RxEventCallback
* @brief:		Actua luego de recibir un paquete desde UART.
******************************************************************************/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
	// Primero busco el manejador correspondiente a la huart
	manejador_UART_t * pum = driver_uart_identificar_manejador (huart);
	if ( pum == NULL ) return;	// Ocurre si la huart no fue inicializada

	// Si leímos algo, lo cargo en el ringbuffer de transmisión:
	if(0 < size)
	{
		eringbuffer_write(&pum->rb_rx_, pum->rx_buffer_, size);
	}

	// Finalmente se vuelve a activar la recepción:
	HAL_UARTEx_ReceiveToIdle_IT(pum->p_UART_fabricante, pum->rx_buffer_, RX_BUFFER_SIZE_);
}

/******************************************************************************
* @function:	HAL_UART_TxCplyCallback
* @brief:		Actua luego de completar la transmisíon de un paquete por UART.
******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	// Primero busco el manejador correspondiente a la huart
	manejador_UART_t * pum = driver_uart_identificar_manejador (huart);
	if ( pum == NULL ) return;	// Ocurre si la huart no fue inicializada
	// if((&huart3) != huart) return; // Prueba para sólo un driver

	// Leo ringbuffer y decido si transmito mediante función HAL
	if(eringbuffer_is_empty(&pum->rb_tx_))
	{
		pum->tx_free_ = true;
		// tx_free_ = true;
	}
	else
	{
		size_t tx_len = eringbuffer_read(&pum->rb_tx_, pum->tx_buffer_, TX_BUFFER_SIZE_);
		HAL_UART_Transmit_IT(pum->p_UART_fabricante, pum->tx_buffer_, tx_len);
	}
}

/******************************************************************************
* @function:	driver_uart_init
* @brief:		Inicializa un puerto UART y una memoria intermedia anillo.
******************************************************************************/
void driver_uart_init(manejador_UART_t * pmu)
{
	// Primero verifico que la UART no haya sido inicializada previamente:
	if ( NULL != driver_uart_identificar_manejador(pmu->p_UART_fabricante)) return;

	// Luego verifico que haya espacio para memoria intermedia anillo...
	// y la asigno al puntero en la estructura.
	pmu->rb_rx_buffer_ = (uint8_t *) cese_memoria_asignar(RB_RX_BUFFER_SIZE_);
	if (NULL == pmu->rb_rx_buffer_ ) return;
	pmu->rb_tx_buffer_ = (uint8_t *) cese_memoria_asignar(RB_TX_BUFFER_SIZE_);
	if (NULL == pmu->rb_tx_buffer_ ) {
		cese_memoria_liberar( pmu->rb_rx_buffer_ );
		return;
	}

	// Tengo una nueva UART activa:
	p_UART_activa[num_UARTs] = pmu;
	num_UARTs ++;
	if (num_UARTs > MAX_NUM_UARTS) Error_Handler();	// Más UARTs que las posibles

	// Flag y mutex de tx
	// tx_free_ = true;
	pmu->tx_free_ = true;
	pmu->mutex_tx_ = cese_mutex_crear();

	// Inicializo la memoria intermedia anillo de tx y rx
	eringbuffer_init(&pmu->rb_tx_, pmu->rb_tx_buffer_, RB_TX_BUFFER_SIZE_);
	eringbuffer_init(&pmu->rb_rx_, pmu->rb_rx_buffer_, RB_RX_BUFFER_SIZE_);

	// Activamos recepción...
	HAL_UARTEx_ReceiveToIdle_IT(pmu->p_UART_fabricante, pmu->rx_buffer_, RX_BUFFER_SIZE_);
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
		HAL_UART_Transmit_IT(pmu->p_UART_fabricante, pmu->tx_buffer_, tx_len);
		// tx_free_ = false;
		pmu->tx_free_ = false;
	}
	//taskEXIT_CRITICAL();
	cese_mutex_devolver(pmu->mutex_tx_);
	return ret;
}

/********************** end of file ******************************************/
