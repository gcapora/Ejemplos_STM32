/**************************************************************************************************
 * Archivo: uHALadc.h
 * Breve:   Capa HAL para manejo simultáneo de ADC1 y ADC2.
 * Fecha:   Mayo 2024
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UHAL_ADC_H_
#define ISPEL_UHAL_ADC_H_

/****** Librerías (includes) *********************************************************************/

#include "uHAL.h"

/****** Definiciones públicas (macros) ***********************************************************/

#define UHAL_ADC_FRECUENCIA_MUESTREO_MAXIMA   2.4e6
#define UHAL_ADC_FRECUENCIA_MUESTREO_MINIMA   100

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	UHAL_ADC_1,
	UHAL_ADC_12 = UHAL_ADC_1,
	UHAL_ADC_2,
	UHAL_ADC_3,
	UHAL_ADC_TODOS
} adc_id_e;

typedef enum {
	U_ADC_CANAL_1,
	U_ADC_CANAL_2
} adc_canal_e;

typedef struct {
	adc_canal_e Canal;
	double		FrecuenciaMuestreo;
} adc_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool	uHALadcInicializar            ( adc_id_e );                        // Config estática en uHALconfig.h
//bool	uHALadcDesInicializar         ( adc_id_e );                        // Falta implementar
bool  uHALadcConfigurar             ( adc_id_e, adc_config_s * );        // Config dinámica
bool  uHALadcObtener                ( adc_id_e, adc_config_s * );        // Leer config
bool  uHALadcComenzarLectura        ( adc_id_e, uint32_t *, uint32_t );
bool	uHALadcPararLectura 				( adc_id_e );
void	uHALadcLecturaCompletada      ( adc_id_e );                        // Redefinir en aplicación
void	uHALadcMediaLecturaCompletada ( adc_id_e );                        // Redefinir en aplicación

/*************************************************************************************************/
#endif /* ISPEL_UHAL_ADC_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
