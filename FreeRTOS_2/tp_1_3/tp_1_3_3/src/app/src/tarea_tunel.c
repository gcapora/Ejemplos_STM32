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

#define APERTURA_DE_BARRERA_MS_		(2000)
#define PERIODO_DE_LECTURA_MS_		(45)

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

	// Leo cola de EMREGENCIA primero
	estado = xQueueReceive ( mensajes_emergencia_a_tunel,
							 & auto_en_espera,
							 (TickType_t)(PERIODO_DE_LECTURA_MS_ / portTICK_PERIOD_MS) );

	if (estado == pdTRUE) {
		// Hay una solicitud de EMERGENCIA
		ELOG("Entra EMERGENCIA: auto numero %d:", (uint16_t) auto_en_espera.cuenta );

		// ¿Qué barrera debo abrir?
		emergency_access_open(auto_en_espera.entrada_id);							// Abro barrera;
		vTaskDelay((TickType_t)(APERTURA_DE_BARRERA_MS_ / portTICK_PERIOD_MS));		// espero que pase...
		emergency_access_close(auto_en_espera.entrada_id);							// Cierro barrera.

	} else {
		// Leo cola de autos comunes
		estado = xQueueReceive ( mensajes_a_tunel, & auto_en_espera, 0 );

		if (estado==pdTRUE) {
			// Hay un auto común esperando para entrar y debo abrir
			ELOG("Entra auto numero %d:", (uint16_t) auto_en_espera.cuenta );

			// ¿Qué barrera debo abrir?
			access_open(auto_en_espera.entrada_id);									// Abro barrera;
			vTaskDelay((TickType_t)(APERTURA_DE_BARRERA_MS_ / portTICK_PERIOD_MS));	// espero que pase...
			access_close(auto_en_espera.entrada_id);								// Cierro barrera.
		}
	}	// if-else dentro del while de la tarea

  }		// while de tarea
}		// fin de tarea

/********************** end of file ******************************************/
