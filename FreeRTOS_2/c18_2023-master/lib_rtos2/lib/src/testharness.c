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
 * @file   : testharness.c
 * @date   : Feb 27, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

#if 0
/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "testharness.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

bool tlog_enable = false;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void tled_write_(bool red, bool green, bool blue)
{
  eboard_led_red(red);
  eboard_led_green(green);
  eboard_led_blue(blue);
}

/********************** external functions definition ************************/

void tlog_enable(bool enable)
{

}

void tled_off(void)
{
  tled_write_(false, false, false);
}

void tled_wip(void)
{
  tled_write_(false, false, true);
}

void tled_ok(void)
{
  tled_write_(false, true, false);
}

void tled_fail(void)
{
  tled_write_(true, false, false);
}

void test_start()
{
  th_log_enable = false;
  TH_DELAY(1000);
  vTaskSuspendAll();

  while (true)
  {
    TH_DELAY(1000);
  }
}

void test_end()
{
  th_log_enable = false;
  TH_DELAY(1000);
  vTaskSuspendAll();

  while (true)
  {
    TH_DELAY(1000);
  }
}

void test_fail()
{
  TH_LOG("Test FAIL");
  th_led_msg_fail();

  test_end();
}

void test_ok()
{
  TH_LOG("Test OK");
  th_led_msg_ok();

  test_end();
}

__WEAK void test_setup(void)
{
  return;
}

__WEAK void test_teardown(void)
{
  return;
}

static void test_case_run(void (*hcase)(void))
{
  test_setup_();
  hcase();
  test_teardown_();
}

static void case_without_events_(void)
{
  set_cars_(false, false);
  TH_DELAY(1000);
  assert_cars_counters_(0, 0, 0, 0);
}

#endif
/********************** end of file ******************************************/
