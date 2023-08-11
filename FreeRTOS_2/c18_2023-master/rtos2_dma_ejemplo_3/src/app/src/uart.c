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
 * @file   : uart.c
 * @date   : Mar 21, 2023
 * @author : NOMBRE <MAIL>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hal.h"
#include "driver.h"
#include "uart.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (10000)
#define BUFFER_LEN_             (64)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static uint8_t pbuffer_[BUFFER_LEN_];
static uart_buffer_handler_t buffer_handler_;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void uart_read_(void)
{
  size_t len = eboard_uart_read(pbuffer_, BUFFER_LEN_);
  buffer_handler_(pbuffer_, len);
  ELOG("Mensaje Recibido");
}

static void task_(void* args)
{
  bool led_state = false;

  while(true)
  {
    uart_read_();

    led_state = !led_state;
    eboard_led_red(led_state);
    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** external functions definition ************************/

void uart_init(uart_buffer_handler_t buffer_handler)
{
  buffer_handler_ = buffer_handler;

  BaseType_t status;
  status = xTaskCreate(task_, "task_uart", 128, NULL, tskIDLE_PRIORITY, NULL);
  while (pdPASS != status)
  {
    // error
  }
}

/********************** end of file ******************************************/
