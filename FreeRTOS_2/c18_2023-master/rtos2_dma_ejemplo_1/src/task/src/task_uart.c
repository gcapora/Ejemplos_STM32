/*
 * Copyright (c) YEAR NOMBRE <MAIL>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_uart.c
 * @date   : Mar 14, 2023
 * @author : NOMBRE <MAIL>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "driver.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (10000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern QueueHandle_t hqueue;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_uart(void* args)
{
  char* pmsg;

  bool led_state = false;

  while(true)
  {
    size_t len = eboard_uart_rx_len();
    if(0 < len)
    {
      pmsg = (char*)pvPortMalloc(len + 1);
      if(NULL != pmsg)
      {
        ELOG("Se alocaron %d bytes", len + 1);
        eboard_uart_sread(pmsg, len + 1);
        ELOG("Mensaje Recibido: %s", pmsg);

        if(pdPASS == xQueueSend(hqueue, (void*)&pmsg, 0))
        {
          ELOG("Mensaje enviado al proceso");
        }
        else
        {
          ELOG("No hay lugar en la cola para enviar el mensajes");
          vPortFree((void*)pmsg);
          ELOG("Memoria liberada");
        }
      }
      else
      {
        ELOG("No hay suficiente memoria para alocar más mensajes");
      }
    }

    led_state = !led_state;
    eboard_led_red(led_state);
    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** end of file ******************************************/
