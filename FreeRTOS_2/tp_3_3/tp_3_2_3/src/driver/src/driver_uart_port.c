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

/********************** external functions definition ************************/

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
