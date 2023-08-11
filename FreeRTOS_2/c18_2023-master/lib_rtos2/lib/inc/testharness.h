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
 * @file   : testharness.h
 * @date   : Feb 27, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

#ifndef APP_INC_TESTHARNESS_H_
#define APP_INC_TESTHARNESS_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "eboard.h"

/********************** macros ***********************************************/

#define TASK_PRIORITY           (tskIDLE_PRIORITY + 10)

#define TLOG(...)\
  if(tlog_enable)\
  {\
    ELOG(__VA_ARGS__);\
  }

#define TLOG_ENABLE(enable)     tlog_enable = (enable)

#define TDELAY(time_ms)         eboard_osal_port_delay(time_ms)

#define TRUN(fnc_name)\
  TLOG("------------------------------");\
  TLOG(# fnc_name);\
  test_case_run_((fnc_name));

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

static bool tlog_enable = false;

/********************** external functions declaration ***********************/

static void test_setup_(void);

static void test_teardown_(void);

static void task_(void* argument);

static void tled_write_(bool red, bool green, bool blue)
{
  eboard_led_red(red);
  eboard_led_green(green);
  eboard_led_blue(blue);
}

static void tled_off_(void)
{
  tled_write_(false, false, false);
}

static void tled_wip_(void)
{
  tled_write_(false, false, true);
}

static void tled_ok_(void)
{
  tled_write_(false, true, false);
}

static void tled_fail_(void)
{
  tled_write_(true, false, false);
}

static void test_start_()
{
  TLOG_ENABLE(false);
  tled_wip_();
  TDELAY(1000);
  TLOG_ENABLE(true);
}

static void test_end_()
{
  TLOG_ENABLE(false);
  TDELAY(1000);
//  vTaskSuspendAll();

  while (true)
  {
    TDELAY(1000);
  }
}

void test_fail_()
{
  TLOG("Test FAIL");
  tled_fail_();
  test_end_();
}

void test_ok_()
{
  TLOG("Test OK");
  tled_ok_();
  test_end_();
}

static void test_case_run_(void (*hcase)(void))
{
  test_setup_();
  hcase();
  test_teardown_();
}

static void testharness_init_(void)
{
  tled_off_();

  BaseType_t status;
  status = xTaskCreate(task_, "task_test", 256, NULL, TASK_PRIORITY, NULL);
  while (pdPASS != status)
  {
    // error
  }
}

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* APP_INC_TESTHARNESS_H_ */
/********************** end of file ******************************************/

