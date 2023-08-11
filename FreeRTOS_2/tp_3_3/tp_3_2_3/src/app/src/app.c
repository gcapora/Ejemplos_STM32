/*
 * @file   : app.c en tp_3_2_3
 * @date   : Abril 2023
 * @author : GFC en base a Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "app.h"
//#include "driver_uart_port.h"   --> no debería ser necesario
#include "task_tick.h"
#include "task_echo.h"

/********************** macros and definitions *******************************/
/********************** internal data declaration ****************************/
/********************** internal functions declaration ***********************/
/********************** internal data definition *****************************/

manejador_UART_t mi_UART;

/********************** external data definition *****************************/

extern fabricante_UART_t huart3;

/********************** internal functions definition ************************/
/********************** external functions definition ************************/

void app_init(void)
{
  // driver
  {
    driver_init();
    mi_UART.p_UART_fabricante = &huart3;	// Aquí indico qué UART deseo utilizar:
    										// huart3 fue definida en main.c.
    driver_uart_init(&mi_UART);
  }

  // tasks
  {
    BaseType_t status;
    status = xTaskCreate(task_echo, "task_echo", 128, NULL, tskIDLE_PRIORITY, NULL);
    while (pdPASS != status)
    {
      // error
    }

    status = xTaskCreate(task_tick, "task_tick", 128, NULL, tskIDLE_PRIORITY, NULL);
    while (pdPASS != status)
    {
      // error
    }
  }
}

/********************** end of file ******************************************/
