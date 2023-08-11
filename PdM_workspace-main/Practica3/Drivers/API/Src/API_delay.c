/*******************************************************************************
  * @file    API_delay.c
  * @author  Guillermo Caporaletti
  * @brief   Módulo de funciones para retardo no bloqueante.
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "API_delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  Carga el valor de duración del retardo en la estructura
  *			Inicializa el flag running en `false´.
  */
void delayInit( delay_t * delay, tick_t duration )
{
	delay->duration = duration;
	delay->running = false;
}

/*******************************************************************************
  * @brief	Verifica el estado del flag running.
  */
bool_t delayRead( delay_t * delay )
{
	bool_t ended = false;
	if (!delay->running) {
		// Configuro inicio de contador de retardo
		delay->startTime = HAL_GetTick();
		delay->running = true;
	} else {
		// Analizo si debo continuar con el retardo o no
		if ( (HAL_GetTick()-delay->startTime) > (delay->duration) ) {
			// Terminó el retardo!!!
			ended = true;
			delay->running = false;
		}
	}
	return ended;
}

/*******************************************************************************
  * @brief  Cambia el valor de duración del retardo en la estructura.
  */
void delayWrite( delay_t * delay, tick_t duration )
{
	delay->duration = duration;
}

/***************************************************************END OF FILE****/
