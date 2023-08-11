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
const tick_t MAX_DELAY = 600*1000;   // medido en milisegundos

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  Carga el valor de duración del retardo en la estructura
  *			Inicializa el flag running en `false´.
  */
void delayInit( delay_t * delay, tick_t duration )
{
	if (duration <= MAX_DELAY && delay != NULL) {
		delay->duration = duration;
		delay->running = false;
	} else {
		// Parámetros erróneos
	    Error_Handler();
	}
}

/*******************************************************************************
  * @brief  Deja de contar el retardo.
  * 		Es necesario un delayRead para volver a empezar
  */
void delayReset( delay_t * delay) {
	delay->running = false;
}

/*******************************************************************************
  * @brief	Verifica el estado del flag running.
  */
bool_t delayRead( delay_t * delay )
{
	bool_t ended = false;
	if (delay == NULL) {
		// Parámetro erróneo
		Error_Handler();
	}
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
	if (duration <= MAX_DELAY && delay != NULL) {
			delay->duration = duration;
	} else {
		// Parámetros erróneos
	    Error_Handler();
	}
}

/***************************************************************END OF FILE****/
