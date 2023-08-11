/*******************************************************************************
  * @file		API_generador.h
  * @brief      Maneja acciones de los estados del
  *             Generador de señales con entrada por UART
  * @author		Guillermo F. Caporaletti <gfcaporaletti@undav.edu.ar>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __API_GENERADOR_H
#define __API_GENERADOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal_dac.h"
#include <errorHandler.h>
#include "API_delay.h"
#include "API_debounce.h"
#include "API_uart.h"
#include "API_dac_dma.h"

/* Macros públicas -----------------------------------------------------------*/
#define N_MUESTRAS			105		// Muestras en un período de señal

/* Typedef públicos ----------------------------------------------------------*/
// Los estados por los que puede pasar cada generador (DAC1 o DAC2)
typedef enum {
	Espera,
	Vacio = Espera,
	Recibiendo,
	Cargado,
	Generando,
	Pausa
} estadosMEF;

/* Funciones públicas --------------------------------------------------------*/
void Gen_Init(void);
void Gen_Espera(void);
void Gen_Recibir(void);
void Gen_Cargar(uint32_t Senial[]);
void Gen_Encender(void);
void Gen_Pausar(void);
estadosMEF Gen_Estado(void);
void Gen_Actualiza_Leds(void);

#endif /* __API_GENERADOR_H */
