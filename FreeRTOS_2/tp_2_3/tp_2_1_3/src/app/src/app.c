/* @file   : app.c en tp_2_1_3
 *
 * Basado en:
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "test.h"
#include "bank.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

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

  // ao
  {
    bank_init();
    ELOG("services init");
  }

  ELOG("app init");
}

/********************** end of file ******************************************/
