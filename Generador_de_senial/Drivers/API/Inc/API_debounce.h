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
#include <errorHandler.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "API_delay.h"
#include "../../BSP/stm32f4xx_nucleo_144.h" 	/* <- BSP include */

/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Funciones públicas---------------------------------------------------------*/

void debounceFSM_init();		// Carga el estado inicial de la MFE antirrebote
void debounceFSM_update();		// Actualiza estado de MFE antirrebote:
								// Lee las entradas, resuelve la lógica de transición
								// y actualizar las salidas.
bool_t readKeyPush();
bool_t readKeyRelease();
bool_t readPresionadoLargo();

/*----------------------------------------------------------------------------*/
#endif /* __MAIN_H */

/***************************************************************END OF FILE****/
