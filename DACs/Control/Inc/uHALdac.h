/**************************************************************************************************
 * Archivo:   uHALdac.h
 * Breve:     Interfaz HAL para DACs
 * Autor:     Guillermo Caporaletti
 * Fecha:     creado en agosto 2023
 *************************************************************************************************/

#ifndef __ISPEL_UHALDAC_H
#define __ISPEL_UHALDAC_H

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>

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

/****** Declaraciones de datos externos **********************************************************/

// extern DMA_HandleTypeDef hdma_dac2;
// extern DMA_HandleTypeDef hdma_dac1;

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

