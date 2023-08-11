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
 * @file   : eboard.h
 * @date   : Mar 4, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

#ifndef INC_EBOARD_H_
#define INC_EBOARD_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/********************** macros ***********************************************/

#define ELOG_MAXLEN             (64)

#ifdef EBOARD_CONFIG_VERBOSE
#define ELOG(...)\
    taskENTER_CRITICAL();\
    {\
      elog_msg_len = snprintf(elog_msg, (ELOG_MAXLEN - 1), __VA_ARGS__);\
      eboard_log(elog_msg);\
    }\
    taskEXIT_CRITICAL()
#else
#define ELOG(...)
#endif

/********************** typedef **********************************************/

typedef enum
{
  EBOARD_GPIO_LEDR,
  EBOARD_GPIO_LEDG,
  EBOARD_GPIO_LEDB,
  EBOARD_GPIO_SW,
  EBOARD_GPIO__CNT,
} eboard_gpio_idx_t;

/********************** external data declaration ****************************/

extern char* const elog_msg;
extern int elog_msg_len;

/********************** external functions declaration ***********************/

uint32_t eboard_osal_port_get_time(void);

void eboard_osal_port_delay(uint32_t time_ms);

void eboard_uart_init(void* phuart);

void eboard_gpio_init(eboard_gpio_idx_t idx, void* hgpio);

void eboard_gpio_write(eboard_gpio_idx_t idx, bool value);

bool eboard_gpio_read(eboard_gpio_idx_t idx);

void eboard_led_red(bool value);

void eboard_led_green(bool value);

void eboard_led_blue(bool value);

bool eboard_switch(void);

size_t eboard_uart_tx_len(void);

size_t eboard_uart_write(const uint8_t *buffer, size_t size);

size_t eboard_uart_write_byte(uint8_t byte);

size_t eboard_uart_swrite(const char *str);

size_t eboard_uart_swrite_line(const char *str);

size_t eboard_uart_rx_len(void);

size_t eboard_uart_read(uint8_t *buffer, size_t size);

size_t eboard_uart_read_byte(uint8_t* pbyte);

size_t eboard_uart_sread(char *str, size_t max_size);

void eboard_hal_port_uart_error(void* huart);

void eboard_hal_port_uart_rx_irq(void* huart, uint16_t size);

void eboard_hal_port_uart_tx_irq(void* huart);

void eboard_hal_port_gpio_write(void* handle, bool value);

bool eboard_hal_port_gpio_read(void* handle);

void eboard_log(const char* str);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_EBOARD_H_ */
/********************** end of file ******************************************/

