/* @file   : app_port.c en tp_3_1_3
 * @date   : Abril 2023
 * @author : GFC a partir de SB
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "driver_uart_port.h"

/********************** macros and definitions *******************************/
/********************** internal data declaration ****************************/
/********************** internal functions declaration ***********************/
/********************** internal data definition *****************************/
/********************** external data definition *****************************/
/********************** internal functions definition ************************/
/********************** external functions definition ************************/

// Recibe parámetros de envío
// Devuelve caracteres enviados o 0 si hubo error
size_t driver_uart_port_hal_tx(void* handler, uint8_t* buffer, size_t size, uint32_t timeout)
{
  if(HAL_OK == HAL_UART_Transmit((estructura_UART_t *)handler, buffer, size, timeout))
  {
    return size;
  }
  return 0;
}

// Recibe parámetros de recepción: solo lee de a un caracter
// Devuelve 1 si leyó o 0 si no leyó.
size_t driver_uart_port_hal_rx(void* handler, uint8_t* buffer, uint32_t timeout)
{
  if(HAL_OK == HAL_UART_Receive((estructura_UART_t *)handler, buffer, 1, timeout))
  {
    return 1;
  }
  return 0;
}

void driver_uart_port_osal_enter_critical(void)
{
  taskENTER_CRITICAL();
}

void driver_uart_port_osal_exit_critical(void)
{
  taskEXIT_CRITICAL();
}

/********************** end of file ******************************************/
