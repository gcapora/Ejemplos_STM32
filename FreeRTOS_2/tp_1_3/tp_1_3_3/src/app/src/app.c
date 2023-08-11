/*
 * @file   : app.c de TP_1_3_3
 * @date   : Mar, 2023
 * @author : GFC
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

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

access_t acceso_este =  ACCESS_EAST;
access_t acceso_oeste =  ACCESS_WEST;
QueueHandle_t mensajes_a_tunel;				// Mensajes para vehículos comunes
QueueHandle_t mensajes_emergencia_a_tunel;	// Mensajes de vehículos de emergencia
BaseType_t status;							// Variable para ver que las tareas
											// se hayan creado adecuadamente

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void app_init(void)
{
  // drivers ------------------------------------------------------------------
  driver_init();
  ELOG("drivers init");

  // test ---------------------------------------------------------------------
  test_init();
  ELOG("test init");

  // OA tunel -----------------------------------------------------------------
  status = xTaskCreate( oa_tunel, "OA_tunel", 128, NULL, tskIDLE_PRIORITY+7, NULL);
  while (pdPASS != status) {} // ¿error?
  ELOG("ao init");

  // Cola de mensajes ---------------------------------------------------------
  mensajes_a_tunel = xQueueCreate ( 16, sizeof(mensaje_a_tunel_t) );
  mensajes_emergencia_a_tunel = xQueueCreate ( 16, sizeof(mensaje_a_tunel_t) );

  // Tareas -------------------------------------------------------------------

  	// Tarea de lectura de sensor ESTE para vehículos comunes
	status = xTaskCreate(tarea_sensor, "tarea_sensor_ESTE", 128, &acceso_este, tskIDLE_PRIORITY+4, NULL);
	while (pdPASS != status) {}	// si me quedo en este while es porque hubo error

	// Tarea de lectura de sensor ESTE para vehículos EMERGENCIA
	status = xTaskCreate(tarea_sensor_emergencia, "tarea_EMERGENCIA_sensor_ESTE", 128, &acceso_este, tskIDLE_PRIORITY+5, NULL);
	while (pdPASS != status) {}	// si me quedo en este while es porque hubo error

	// Tarea de lectura de sensor OESTE para vehículo comunes
	status = xTaskCreate(tarea_sensor, "tarea_sensor_OESTE", 128, &acceso_oeste, tskIDLE_PRIORITY+4, NULL);
	while (pdPASS != status) {}	// si me quedo en este while es porque hubo error

	// Tarea de lectura de sensor OESTE para vehículo EMERGENCIA
	status = xTaskCreate(tarea_sensor_emergencia, "tarea_EMERGENCIA_sensor_OESTE", 128, &acceso_oeste, tskIDLE_PRIORITY+5, NULL);
	while (pdPASS != status) {}	// si me quedo en este while es porque hubo error

    ELOG("tasks init");

  // Inicio de aplicación exitosa ---------------------------------------------
  ELOG("app init");

}
/********************** end of file ******************************************/
