/* @file   : app.c de TP_1_1_3
 * @author : GFC <gcaporaletti@fi.uba.ar>
 *           en base a plantilla de Sebastian Bedin <sebabedin@gmail.com>
 * @date   : Marzo, 2023
 *
 */

/********************** incluídos ********************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "test.h"
#include "test_mock.h"
#include "task_button.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

button_t boton1 = BUTTON_RED;
button_t boton2 = BUTTON_GREEN;
button_t boton3 = BUTTON_BLUE;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void app_init(void)
{
  // drivers
  {
    driver_init();
    ELOG("drivers init");
  }

  // test
  {
    test_init();
    ELOG("test init");
  }

  // OA
  // {
  //  ELOG("ao init");
  // }
  // Esta inicialización no fue necesaria.

  // tasks

  {
	BaseType_t status;	// Variable para ver que las tareas se hayan creado adecuadamente

	// Pulsador para botón rojo
	status = xTaskCreate(tarea_boton, "task_button RED", 128, &boton1, tskIDLE_PRIORITY+4, NULL);
    while (pdPASS != status)
    {
      // error
    }

    // Pulsador para botón verde
    status = xTaskCreate(tarea_boton, "task_button GREEN", 128, &boton2, tskIDLE_PRIORITY+5, NULL);
    while (pdPASS != status)
    {
       // error
    }

    // Pulsador para botón azul
    status = xTaskCreate(tarea_boton, "task_button BLUE", 128, &boton3, tskIDLE_PRIORITY+6, NULL);
    while (pdPASS != status)
    {
       // error
    }
  }

  {
    ELOG("tasks init");
  }

  ELOG("app init");
}

/********************** end of file ******************************************/
