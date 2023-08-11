/******************************************************************************
* @file:	driver_uart_port.h
* @brief:	Declaraciones para abstraer funciones del OS y tipos
* 			utilizadas por driver_uart.c y .h
******************************************************************************/

#ifndef DRIVER_INC_DRIVER_UART_PORT_H_
#define DRIVER_INC_DRIVER_UART_PORT_H_

/********************** macros ***********************************************/

#define	MAX_NUM_UARTS	3	// NUCLEO-F429ZI tiene 3 salidas UART.
//#define NO_EXISTE_UART	255	// Número reservado para error.

/********************** typedef **********************************************/

typedef UART_HandleTypeDef fabricante_UART_t;
	// Esta definicíon sirve para abstraernos de la estructura del fabricante.
	// Es el tipo de estructura que utilizaremos dentro de driver_uart.c
	// Si cambia el fabricante, debo cambiar aquí UART_HandleTypeDef
	// por la estructura que corresponda.

/********************** external functions declaration ***********************/

void * cese_memoria_asignar(size_t);
void cese_memoria_liberar(void *);
SemaphoreHandle_t cese_mutex_crear();
void cese_mutex_tomar(SemaphoreHandle_t);
void cese_mutex_devolver(SemaphoreHandle_t);
void cese_UART_esperar_recepcion(UART_HandleTypeDef * huart, uint8_t *pData, uint16_t Size);
void cese_UART_transmitir(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/*****************************************************************************/

#endif	// DRIVER_INC_DRIVER_UART_PORT_H_

/********************** end of file ******************************************/
