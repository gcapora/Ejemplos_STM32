/* @file:	bank.c en tp_2_1_3
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "client.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_           (10)
#define QUEUE_ITEM_SIZE_        (sizeof(client_t))
#define QUEUE_ITEM_SIZE_P_		(sizeof(client_t*))  // definición supérflua pero ayuda

/********************** internal data declaration ****************************/

/*static struct {
 	 QueueHandle_t hclient_queue;
} bank_;*/						// Estructura  de un dato me pareció engorroso...

QueueHandle_t cola_p_clientes;	// Este es el identificador de
								// la cola de punteros a clientes.

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/* Función: client_callback_
 * Breve:	Se llama a esta función cada vez que hay un nuevo cliente.
 * 			Debe almacenar en una cola a los punteros a la dirección de
 * 			memoria dinámica donde están los datos del cliente.
 */
static void client_callback_(client_t* pnew_client)
{
	client_t* p_cliente_md;		// puntero local, para pedir memoria.

	// Ingreso en cola con memoria dinámica
	p_cliente_md = (client_t*) pvPortMalloc( sizeof(client_t) );	// Asigno memoria dinámica (md)

	if ( p_cliente_md == NULL ) {									// Verifico si se asignó md
		ELOG("ERROR: No hay memoria para un nuevo cliente en espera.");
	} else {														// Sí, se asignó md
		ELOG("1) Memoria dinamica (md) reservada para nuevo cliente.");
		memcpy( (void*) p_cliente_md,
				(void*) pnew_client,
				sizeof(client_t) );									// Copio el contenido de pnew_client

		if ( pdPASS == xQueueSend(cola_p_clientes, (void*) &p_cliente_md, 0) ) {
																	// Cargo en cola de punteros (si hay lugar)
			ELOG("2) Nuevo cliente almacenado en cola de punteros a md.");
		} else {
			ELOG("ERROR: El cliente no tiene lugar en la fila.");
		}
	}

	/* Ingreso en cola original
	if (pdPASS == xQueueSend(bank_.hclient_queue, (void*)pnew_client, 0)) {
		ELOG("Ingresa el cliente a la fila");
	} else {
		ELOG("Error, el cliente no tiene lugar en la fila");
	}*/
}

static void task_(void *args)
{
  client_t* pclient;

  client_set_callback(client_callback_);

  while (true)
  {
    //if (pdPASS == xQueueReceive(bank_.hclient_queue, &client, portMAX_DELAY))
	if (pdPASS == xQueueReceive(cola_p_clientes, &pclient, portMAX_DELAY))
    {
      ELOG("3) Llamo al cliente:");
      client_process(pclient);
      vPortFree( (void*) pclient );
      ELOG("4) Fin de la atencion del cliente");
    }
  }
}

/********************** external functions definition ************************/

void bank_init(void)
{
	// Cola de punteros a tipo client_t
	cola_p_clientes = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_P_);
	while (NULL == cola_p_clientes)
	{
	    // error
	}

	// Cola original
	/*
	bank_.hclient_queue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while (NULL == bank_.hclient_queue) {
		// error
	} */

	// Tarea!!!
	BaseType_t status;
	status = xTaskCreate(task_, "task_bank", 128, NULL, tskIDLE_PRIORITY, NULL);
	while (pdPASS != status)
	{
		// error
	}
}

/********************** end of file ******************************************/
