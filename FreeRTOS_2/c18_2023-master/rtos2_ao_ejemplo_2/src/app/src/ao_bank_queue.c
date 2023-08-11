/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
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
 * @file   : ao_bank_queue.c
 * @date   : Mar 6, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "ao_bank_queue.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_           (1)
#define QUEUE_ITEM_SIZE_        (sizeof(char))
#define MAX_THREADS_            (5)
#define TASK_PERIOD_MS_         (10*1000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static struct
{
  QueueHandle_t hclient_queue;
  ao_bank_queue_client_callback_t callback;
  size_t task_cnt;
  size_t client_cnt;
} bank_;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void task_delete_(void)
{
  ELOG("Borro una tarea");
  bank_.task_cnt--;
  ELOG("Cantidad de tareas: %d", bank_.task_cnt);

  vTaskDelete(NULL);
}

static void task_(void* argument)
{
  char client;
  while (true)
  {
    if (pdPASS == xQueueReceive(bank_.hclient_queue, &client, 0))
    {
      ELOG("Llamo al cliente: [%c]", client);

      bank_.client_cnt++;
      ELOG("Cantidad de clientes siendo atendidos: %d", bank_.client_cnt);

      bank_.callback(client);
      vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
      ELOG("Fin de la atención del cliente: [%c]", client);

      bank_.client_cnt--;
      ELOG("Cantidad de clientes siendo atendidos: %d", bank_.client_cnt);
    }
    else
    {
      task_delete_();
    }
  }
}

static bool task_create_(void)
{

  if(bank_.task_cnt < MAX_THREADS_)
  {
    ELOG("Creo una tarea");
    bank_.task_cnt++;
    ELOG("Cantidad de tareas: %d", bank_.task_cnt);
    BaseType_t status;
    status = xTaskCreate(task_, "task_bank", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
    while (pdPASS != status)
    {
      ELOG("Error!!!");
      // error
    }
    return true;
  }
  else
  {
    ELOG("No puedo crear nuevas tareas");
    return false;
  }
}

static void callback_(char client)
{
  return;
}

/********************** external functions definition ************************/

void ao_bank_queue_init(void)
{
  bank_.client_cnt = 0;
  bank_.task_cnt = 0;
  bank_.callback = callback_;

  bank_.hclient_queue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
  while (NULL == bank_.hclient_queue)
  {
    // error
  }
}

void ao_bank_queue_client_callback(ao_bank_queue_client_callback_t callback)
{
  bank_.callback = callback;
}

bool ao_bank_queue_new_client(char new_client)
{
  if (pdPASS == xQueueSend(bank_.hclient_queue, (void* )&new_client, 0))
  {
    // ok
    ELOG("Ingresa el cliente [%c] a la fila", new_client);
    if(0 == bank_.task_cnt)
    {
      task_create_();
    }
    return true;
  }
  else if (task_create_())
  {
    if (pdPASS == xQueueSend(bank_.hclient_queue, (void* )&new_client, 0))
    {
      // ok
      ELOG("Ingresa el cliente [%c] a la fila", new_client);
      return true;
    }
    else
    {
      ELOG("Esto seria un error de prioridades de las tareas");
    }
  }
  else
  {
    ELOG("No es posible crear más recursos para atender a los clientes");
  }

  ELOG("Error, el cliente [%c] no tiene lugar en la fila", new_client);
  return false;
}

/********************** end of file ******************************************/
