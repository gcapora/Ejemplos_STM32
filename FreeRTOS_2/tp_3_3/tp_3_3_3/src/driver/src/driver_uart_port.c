/******************************************************************************
* @file:	driver_uart_port.h
* @brief:	Definiciones de funciones para abstraer OS
* 			declaradas en driver_uart_port.c
******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "driver_uart.h"
#include "driver_uart_port.h"
//#include "eringbuffer.h"

/********************** external functions declaration ***********************/

//extern manejador_UART_t * driver_uart_identificar_manejador (fabricante_UART_t * );

/********************** external functions definition ************************/

// *********************************** OSAL ***********************************

void * cese_memoria_asignar(size_t tamanio)
{
	return pvPortMalloc(tamanio);
}

void cese_memoria_liberar(void * puntero)
{
	return vPortFree(puntero);
}

SemaphoreHandle_t cese_mutex_crear()
{
	return xSemaphoreCreateMutex();
	//return xSemaphoreCreateBinary();
}

void cese_mutex_tomar(SemaphoreHandle_t manejador)
{
	xSemaphoreTake(manejador, portMAX_DELAY);
	return;
}

void cese_mutex_devolver(SemaphoreHandle_t manejador)
{
	xSemaphoreGive(manejador);
	return;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  // TODO:
}

// *********************************** HAL port *******************************

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
	cese_UART_esperar_recepcion(pum->p_UART_fabricante, pum->rx_buffer_, RX_BUFFER_SIZE_);
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
		cese_UART_transmitir(pum->p_UART_fabricante, pum->tx_buffer_, tx_len);
	}
}

void cese_UART_esperar_recepcion(UART_HandleTypeDef * huart, uint8_t *pData, uint16_t Size)
{
	HAL_UARTEx_ReceiveToIdle_IT(huart, pData, Size);
}

void cese_UART_transmitir(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	HAL_UART_Transmit_IT(huart, pData, Size);
}
