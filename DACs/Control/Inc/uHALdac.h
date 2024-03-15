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
 * Problemas:
 *  - La función uHALdacdmaSincronizar() puede sacar de servicio un tempo si ambas señales no
 *    tienen la misma frecuencia de muestreo. Se incluye una verificación antes de actuar.
 *
 * Mejoras:
 *  - Para sincronizar, se apagan las señales, se paran los tempos y luego se inicia todos junto.
 *    Se puede sincronizar "al vuelo" si se implementa un sincronismo con un temporizador extra.
 *
 *************************************************************************************************/

#ifndef __ISPEL_UHALDAC_H
#define __ISPEL_UHALDAC_H

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>
#include <stdbool.h>

/****** Definiciones públicas (macros) ***********************************************************/

#define UHAL_CANTIDAD_DACS          		2
#define UHAL_FRECUENCIA_RELOJ				180000000 // Hz
#define UHAL_DAC_FRECUENCIA_MUESTREO_MAX	5000000   // 5 Msps. Considera un margen.
#define MILLON                      		1000000   // 1 millon :-)
#define MHZ                        		 	MILLON    // 1 megahertz
#define KHZ                         		1000      // 1 kilohertz

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	UHAL_DAC_1,
	UHAL_DAC_2,
	UHAL_DAC_TODOS
} dac_id_t;

/****** Declaración de datos públicos ************************************************************/

extern const uint32_t MAXIMO_DAC[UHAL_CANTIDAD_DACS]; // Valores máximos recomendables para DAC's (<=4095)
extern const uint32_t MINIMO_DAC[UHAL_CANTIDAD_DACS]; // Valores mínimos recomendables para DAC's (>=0)
extern const uint32_t CERO_DAC[UHAL_CANTIDAD_DACS];
extern const double   TRANSFERENCIA_DAC[UHAL_CANTIDAD_DACS]; // Voltios / cuenta (aprox. 805,9 uV/cuenta)

/****** Declaración de funciones públicas ********************************************************/

bool     uHALdacInicializar ( dac_id_t );
bool     uHALdacEstablecerValor ( dac_id_t, uint32_t);
bool     uHALdacParar ( dac_id_t );
bool     uHALdacdmaComenzar ( dac_id_t, uint32_t *, uint32_t );
double   uHALdacdmaConfigurarFrecuenciaMuestreo ( dac_id_t, double );
double   uHALdacdmaLeerFrecuenciaMuestreo ( dac_id_t );
double   uHALdacdmaLeerFrecuenciaBase (void);
double   uHALdacdmaLeerFrecuenciaMaxima (void);
double   uHALdacdmaLeerFrecuenciaMinima (void);
uint32_t uHALdacdmaLeerDivisor (dac_id_t);
bool     uHALdacdmaSincronizar ( void );

/*************************************************************************************************/
#endif /* __ISPEL_UHALDAC_H */
/****************************************************************** FIN DE ARCHIVO ***************/

