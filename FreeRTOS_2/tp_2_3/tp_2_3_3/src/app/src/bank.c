/* @file:	bank.c en tp_2_3_3
*/

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "client.h"
#include "bank.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_           (10)
#define QUEUE_ITEM_SIZE_        (sizeof(client_t))
#define QUEUE_ITEM_SIZE_P_		(sizeof(client_t*))	// Definición supérflua pero ayuda
#define TIEMPO_MAXIMO_SIN_TAREA	(1000)				// Luego de este tiempo libero un cajero
#define MAX_CAJEROS_AGREGADOS	(10)

/********************** internal data declaration ****************************/

/*static struct {
  QueueHandle_t hclient_queue;
} bank_;*/

QueueHandle_t cola_p_clientes;	// Este es el identificador de
								// la cola de punteros a clientes.
volatile uint8_t cajeros = 0;
volatile uint16_t clientes = 0;
TaskHandle_t tarea_cajero[MAX_CAJEROS_AGREGADOS];	// <-- Al final no usamos realmente esta estructura.
//volatile bool liberar_cajero = false;

/********************** internal functions declaration ***********************/
static void client_callback_(client_t* pnew_client);
static void task_(void *args);

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
		clientes++;

		if ( p_cliente_md == NULL ) {									// Verifico si se asignó md
			ELOG("ERROR: No hay memoria para un nuevo cliente en espera.");
		} else {														// Sí, se asignó md
			// ELOG("0) Memoria dinamica (md) reservada para nuevo cliente.");
			memcpy( (void*) p_cliente_md,
					(void*) pnew_client,
					sizeof(client_t) );									// Copio el contenido de pnew_client

			if ( pdPASS == xQueueSend(cola_p_clientes, (void*) &p_cliente_md, 0) ) {
				// Cargo en cola de punteros (si hay lugar)
				ELOG("1) Nuevo cliente Nro %d espera en la cola.", clientes);
			} else {
				// Nos quedamos sin lugar en la cola
				// Debo aumentar la velocidad de procesamiento
				BaseType_t status;
				cajeros++;
				status = xTaskCreate(task_, "task_bank", 128, NULL, tskIDLE_PRIORITY, &tarea_cajero[cajeros]);
				if (pdPASS != status) {
					ELOG("ERROR: No pude agregar cajero.");
					while (1) {}	// error
				}
				ELOG("1.PRE) Llamo a otro cajero: %d.", cajeros);
				vTaskDelay( 5 );	// Espero a que venga luego de llamarlo...
				if ( pdPASS == xQueueSend(cola_p_clientes, (void*) &p_cliente_md, 0) ) {
						// Cargo en cola de punteros (si hay lugar)
						ELOG("1) Nuevo cliente Nro %d espera (y otro cajero atiende).", clientes);
				} else {
					ELOG("ERROR: Perdimos al cliente porque no tiene lugar en la fila.");
					vPortFree( (void*) p_cliente_md );
				}
			}
		}

		/* Ingreso en cola original
		if (pdPASS == xQueueSend(bank_.hclient_queue, (void*)pnew_client, 0)) {
			ELOG("Ingresa el cliente a la fila");
		} else {
			ELOG("Error, el cliente no tiene lugar en la fila");
		}*/
}

/* Función: task_
 * Breve:	Tarea que lee la cola y atiende a los clientes.
 */
static void task_(void *args)
{
	client_t* pclient;

	client_set_callback(client_callback_);

	while (true)
	{
		if (pdPASS == xQueueReceive(cola_p_clientes, &pclient, (TickType_t)(TIEMPO_MAXIMO_SIN_TAREA / portTICK_PERIOD_MS)))
	    {
	      ELOG("2) Llamo al cliente.");
	      client_process(pclient);
	      vPortFree( (void*) pclient );
	      ELOG("3) Fin de la atencion del cliente. :-)");
	    } else if (uxQueueMessagesWaiting( cola_p_clientes ) == 0 && cajeros>0) {
			// Si pasó TIEMPO_MAXIMO_SIN_TAREA, no hay más mensajes y hay más de un cajero...
	    	// Entonces libero un cajero:
			ELOG("(!) Me sobra al menos un cajero...");
			cajeros--;
			vTaskDelete( NULL );	// Me autodestruyo (pero siemrpe dejo al menos una tarea task_
			//vTaskDelete( tarea_cajero[cajeros] ); <-- esto lo iba a usar pero lo descarté
		}
	}
}

/********************** external functions definition ************************/

void bank_init(void)
{
	// Cola de punteros a tipo client_t
	cola_p_clientes = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_P_);
	while (NULL == cola_p_clientes) {} 	    // error

	// Tarea!!!
	BaseType_t status;
	status = xTaskCreate(task_, "task_bank", 128, NULL, tskIDLE_PRIORITY, &tarea_cajero[cajeros]);
	while (pdPASS != status) {}				// error

}

/********************** end of file ******************************************/
