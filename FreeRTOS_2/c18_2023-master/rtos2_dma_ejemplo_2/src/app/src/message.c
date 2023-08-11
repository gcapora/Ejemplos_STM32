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
 * @file   : message.c
 * @date   : Mar 14, 2023
 * @author : NOMBRE <MAIL>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hal.h"
#include "driver.h"
#include "message.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_           (5)
#define QUEUE_ITEM_SIZE_        (sizeof(char*))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static QueueHandle_t hqueue_;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void message_init(void)
{
  hqueue_ = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
  while(NULL == hqueue_)
  {
    // error
  }
}

char* message_new(size_t size)
{
  char* pmsg = (char*)pvPortMalloc(size + 1);
  if(NULL != pmsg)
  {
    ELOG("Se alocaron %d bytes", size + 1);
  }
  else
  {
    ELOG("No hay suficiente memoria para alocar más mensajes");
  }
  return pmsg;
}

bool message_send(char* pmsg)
{
  if(pdPASS == xQueueSend(hqueue_, (void*)&pmsg, 0))
  {
    return true;
  }
  ELOG("No hay lugar en la cola para enviar el mensajes");
  return false;
}

void message_destroy(char* pmsg)
{
  vPortFree((void*)pmsg);
  ELOG("Memoria liberada");
}

char* message_receive(TickType_t ticks)
{
  char* pmsg;
  if(pdPASS == xQueueReceive(hqueue_, &pmsg, ticks))
  {
    return pmsg;
  }
  return NULL;
}

/********************** end of file ******************************************/
