/* @file   : task_button.c de tp_1_1_3
 * @date   : Marzo, 2023
 * @author : GFC en base a plantilla de Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "test.h"
#include "test_mock.h"

#include "app.h"
#include "tarea_sensor.h"
#include "tarea_tunel.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (45)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

BaseType_t estado_cola_ts;
mensaje_a_tunel_t auto_en_espera_ts;
uint32_t cuenta_autos;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

/**
 * @brief	Tarea que sensa si hay un auto en la entrada argumento
 * @param	argumento es el ID de access_t
 */
void tarea_sensor(void * argumento)
{
  access_t acceso_id = *(access_t *) argumento;
  switch (acceso_id)
  {
  	  case ACCESS_EAST:
  		ELOG("Inicia tarea sensor ESTE.");
  		break;

  	  case ACCESS_WEST:
  	  	ELOG("Inicia tarea sensor OESTE.");
  	  	break;

  	  default:
  	  	ELOG("Inicia tarea sensor desconocido...");
  }

  bool hay_auto = false;
  auto_en_espera_ts.cuenta = 0;

  while(true)
  {
    hay_auto = car_sensor_read(acceso_id);
    auto_en_espera_ts.entrada_id = acceso_id;

    if (hay_auto == true) {
    	// Debo enviar mensaje a la cola
    	auto_en_espera_ts.cuenta ++;

    	switch (acceso_id)	// Mensaje por pantalla primero...
    	{
    	  case ACCESS_EAST:
    		ELOG("Viene auto desde el ESTE.");
    		break;

    	  case ACCESS_WEST:
    	  	ELOG("Viene auto desde el OESTE.");
    	  	break;

    	  default:
    	  	ELOG("¡Auto de dirección desconocida!");
    	}

    	// Ahora sí envío mensaje a cola!!!
    	estado_cola_ts = xQueueSend ( mensajes_a_tunel, & auto_en_espera_ts, portMAX_DELAY );
    	if (estado_cola_ts == pdFALSE) {
			while(true) {
				// Error !!!
			}
		}
    }

    // Ahora espero a que pase el auto!!!
    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** end of file ******************************************/
