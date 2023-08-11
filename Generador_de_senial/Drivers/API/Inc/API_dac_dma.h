/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for API_dac_dma.c file.
  *                   Manejo de DAC con DMA.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __API_DAC_DMA_H
#define __API_DAC_DMA_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dac.h"
#include "errorHandler.h"
/*#include <stdlib.h>
#include "stm32f4xx_nucleo_144.h"
#include <stdint.h>
#include <stdbool.h> */

/* Funciones públicas --------------------------------------------------------*/
void Inicializar_DAC_DMA(void);
void Comenzar_DAC_DMA(uint32_t * Datos, uint32_t Num_Datos);
void Parar_DAC_DMA(void);

/* Private includes ----------------------------------------------------------*/

#endif /* __API_DAC_DMA_H */
