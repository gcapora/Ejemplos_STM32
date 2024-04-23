/**************************************************************************************************
 * Archivo: uHALadc.h
 * Breve:
 * Fecha:
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UHAL_ADC_H_
#define ISPEL_UHAL_ADC_H_

/****** Librerías (includes) *********************************************************************/

#include "uHAL.h"

/****** Definiciones públicas (macros) ***********************************************************/


/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	UHAL_ADC_1,
	UHAL_ADC_12 = UHAL_ADC_1,
	UHAL_ADC_2,
	UHAL_ADC_3,
	UHAL_ADC_TODOS
} adc_id_e;

typedef struct {
	uint32_t Canal;
	uint32_t FrecuenciaMuestreo;
} adc_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool    uHALadcInicializar            ( adc_id_e );                        // Config estática en uHALconfig.h
bool    uHALadcDesInicializar         ( adc_id_e );                        // Falta implementar
bool    uHALadcConfigurar             ( adc_id_e, adc_config_s * );        // Config dinámica
bool    uHALadcComenzarLectura        ( adc_id_e, uint32_t *, uint32_t );
void	uHALadcLecturaCompletada      ( adc_id_e );                        // Redefinir en aplicación
void	uHALadcMediaLecturaCompletada ( adc_id_e );                        // Redefinir en aplicación

/*************************************************************************************************/
#endif /* ISPEL_UHAL_ADC_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
