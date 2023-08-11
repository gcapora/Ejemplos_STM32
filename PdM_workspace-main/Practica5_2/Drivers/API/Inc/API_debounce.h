/*******************************************************************************
* @file    API_debounce.h
* @author  Guillermo Caporaletti
* @brief   CESE_Co18 - Práctica 4 - Punto 2:
*          Implementación de módulo anti-rebote de pulsador.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __API_DEBOUNCE_H
#define __API_DEBOUNCE_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */
#include "API_delay.h"
#include "errorHandler.h"

/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Funciones públicas---------------------------------------------------------*/

void debounceFSM_init();		// Carga el estado inicial de la MFE antirrebote
void debounceFSM_update();		// Actualiza estado de MFE antirrebote:
								// Lee las entradas, resuelve la lógica de transición
								// y actualizar las salidas.
bool_t readKey();
bool_t readKeyUp();

/*----------------------------------------------------------------------------*/
#endif /* __MAIN_H */

/***************************************************************END OF FILE****/
