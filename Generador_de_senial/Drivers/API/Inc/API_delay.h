/*******************************************************************************
  * @file    Practica3 ... API_delay.h
  * @author  Guillermo Caporlaetti
  * @brief   Header for API_delay.c module
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __API_DELAY_H
#define __API_DELAY_H

/* Includes ------------------------------------------------------------------*/
#include <errorHandler.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"  		/* <- HAL include */

#include "../../BSP/stm32f4xx_nucleo_144.h" 	/* <- BSP include */

/* Types ---------------------------------------------------------------------*/
typedef uint32_t tick_t;
typedef bool bool_t;
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;

/* Constants -----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
void delayInit( delay_t * delay, tick_t duration );
bool_t delayRead( delay_t * delay );
void delayWrite( delay_t * delay, tick_t duration );
void delayReset( delay_t * delay);

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/***************************************************************END OF FILE****/
