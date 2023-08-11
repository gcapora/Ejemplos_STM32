/* @file   : tarea_tunel.c de tp_1_2_3
 * @date   : Marzo, 2023
 * @author : GFC en base a plantilla de Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "tarea_tunel.h"
#include "test.h"
#include "test_mock.h"

/********************** macros and definitions *******************************/

#define APERTURA_DE_BARRERA_MS_	(2000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

/**
 * @brief	Tarea que lee mensajes en cola y decida abrir o cerrar
 * @param	No tiene argumento.La cola debe ser creada globalmente en app
 */
void oa_tunel (void * argumento)
{
  //access_t acceso_id = *(access_t *) argumento;
  ELOG("Inicia OA tunel...");

  mensaje_a_tunel_t auto_en_espera;
  BaseType_t estado;

  while(true)
  {
	// Leo cola y espero indefinidamente si no hay nada:
	estado = xQueueReceive ( mensajes_a_tunel, & auto_en_espera, portMAX_DELAY );

	if (estado == pdFALSE) {
		while(true) {
			// Error !!!
		}
	}

	ELOG("Entra auto numero %d:", (uint16_t) auto_en_espera.cuenta );

	// ¿Qué barrera debo abrir?
	access_open(auto_en_espera.entrada_id);									// Abro barrera;
	vTaskDelay((TickType_t)(APERTURA_DE_BARRERA_MS_ / portTICK_PERIOD_MS));	// espero que pase...
	access_close(auto_en_espera.entrada_id);								// Cierro barrera.

  }
}

/********************** end of file ******************************************/
