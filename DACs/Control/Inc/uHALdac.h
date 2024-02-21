/**************************************************************************************************
 * Archivo:   uHALdac.h
 * Breve:     Interfaz HAL para DACs
 * Autor:     Guillermo Caporaletti
 * Fecha:     creado en agosto 2023
 *
 * Descripción:
 *    La librería brinda las funciones necesarias para el manejo de hardware de dos DACs.
 *    Está implementada inicialmente sobre STM32 Nucleo-429ZI.
 *    La librería inlcuye algunas definiciones específicas de esta placa.
 *    En una posterior versión, se debe trasladar las definiciones específicas a uHALdac_conf.h.
 *
 *************************************************************************************************/

#ifndef __ISPEL_UHALDAC_H
#define __ISPEL_UHALDAC_H

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>
#include <stdbool.h>
//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_dac.h"

/****** Definiciones públicas (macros) ***********************************************************/

#define UHAL_CANTIDAD_DACS          2
#define FRECUENCIA_RELOJ            180000000 // Hz
#define DAC_FRECUENCIA_MUESTREO_MAX 5000000   // 5 Msps. Considera un margen.
#define MILLON                      1000000   // 1 millon :-)
#define MHZ                         MILLON    // 1 megahertz
#define KHZ                         1000      // 1 kilohertz

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	UHAL_DAC_1,
	UHAL_DAC_2,
	UHAL_DAC_TODOS
} dac_id_t;

/****** Declaraciones de datos públicos **********************************************************/

// extern DMA_HandleTypeDef hdma_dac2;
// extern DMA_HandleTypeDef hdma_dac1;
// extern TIM_HandleTypeDef htim [UHAL_CANTIDAD_DACS];// para intento de soncronización
extern uint32_t MAXIMO_DAC[UHAL_CANTIDAD_DACS];        // Valores máximos recomendables para DAC's (<=4095)
extern uint32_t MINIMO_DAC[UHAL_CANTIDAD_DACS];        // Valores mínimos recomendables para DAC's (>=0)
extern double   TRANSFERENCIA_DAC[UHAL_CANTIDAD_DACS]; // Voltios / cuenta (aprox. 805,9 uV/cuenta)

/****** Declaración de funciones públicas ********************************************************/

void   uHALdacdmaInicializar ( dac_id_t );
double uHALdacdmaLeerFrecuenciaMuestreo ( dac_id_t );
double uHALdacdmaConfigurarFrecuenciaMuestreo ( dac_id_t, double );
void   uHALdacdmaComenzar ( dac_id_t, uint32_t *, uint32_t );
void   uHALdacdmaParar ( dac_id_t );
void   uHALdacdmaReanudar ( dac_id_t );
void   uHALdacdmaSincronizar ( void );

/*************************************************************************************************/
#endif /* __ISPEL_UHALDAC_H */
/****************************************************************** FIN DE ARCHIVO ***************/

