/* @file   : ao_led.c de tp_1_1_3
 *
 * @author : GFC <gcaporaletti@fi.uba.ar>
 * 			 en base a la plantilla de Sebastian Bedin <sebabedin@gmail.com>.
 *
 * @date   : Marzo, 2023
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "ao_led.h"
#include "test.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

bool ao_led_write(button_t boton_led_ao, bool estado_ao)
{
	static uint8_t comando_ao=0;				// Este comando podrá ir variando a medida que esta función es llamada.
	uint8_t mascara_ao=0x01 << boton_led_ao;	// Esta máscara servirá para cambiar el comando

	if (estado_ao == true) {					// Cuando el boton_led_ao está presionado...
		comando_ao = (comando_ao | mascara_ao);	// Le impongo un 1 a un bit del comando
	} else {									// Cuando el boton_led_ao está levantado...
		mascara_ao = ~mascara_ao;
		comando_ao = (comando_ao & mascara_ao);	// Le impongo un 0 a un bit del comando
	}

	led_command_send(comando_ao); 				// Finalmente mando el comando... :-)

}

void ao_led_init(ao_led_t* hao, ao_led_callbak_t callback)
{
  eboard_led_red(false);
  eboard_led_green(false);
  eboard_led_blue(false);
  hao->callback = callback;
}




/********************** end of file ******************************************/
