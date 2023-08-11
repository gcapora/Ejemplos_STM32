/* @file   : task_button.c de tp_1_1_3
 * @date   : Marzo, 2023
 * @author : GFC en base a plantilla de Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "ao_led.h"
#include "test.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (90)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

//static ao_led_t ao_led_;
static bool led_state_;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

void led_state_callback_(ao_led_t* hao, bool state)
{
  led_state_ = state;
  ELOG("callback, led:%d", led_state_);
}

/********************** external functions definition ************************/

void tarea_boton(void * argument)
{
  button_t boton_led = *(button_t *) argument;
  ELOG("Inicia tarea boton :%d", boton_led);
  bool led_state2 = false;
  // ao_led_init(&ao_led_, led_state_callback_);

  while(true)
  {
    led_state2 = button_read(boton_led);

    //ELOG("Loop, led:%d", boton_led);
	//ELOG("Estado:%d", led_state2);

	// Envío mensaje a OA!!!
	ao_led_write(boton_led, led_state2);
	// ao_led_write(&ao_led_, eboard_switch());

	//ELOG("read and send");

    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** end of file ******************************************/
