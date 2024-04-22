/**************************************************************************************************
 * Archivo: uHALadc.h
 * Breve:
 * Fecha:
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UHALADC_H_
#define ISPEL_UHALADC_H_

/****** Librerías (includes) *********************************************************************/

#include "uHALconfig.h"
#include "uOSAL.h"

/****** Definiciones públicas (macros) ***********************************************************/

#define UHAL_CANTIDAD_ADCS          		3

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	UHAL_ADC_1,
	UHAL_ADC_2,
	UHAL_ADC_3,
	UHAL_ADC_TODOS
} adc_id_e;

typedef struct {
	uint32_t Canal;
} adc_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool    uHALadcInicializar       ( adc_id_e );                   // Config estática en uHALconfig.h
bool    uHALadcDesInicializar    ( adc_id_e );
bool    uHALadcConfigurar        ( adc_id_e, adc_config_s * );   // Config dinámica
bool    uHALadcComenzarLectura   ( adc_id_e, uint32_t *, uint32_t );
bool	uHALadcLecturaCompletada ( adc_id_e );

/*************************************************************************************************/
#endif /* ISPEL_UHALADC_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
