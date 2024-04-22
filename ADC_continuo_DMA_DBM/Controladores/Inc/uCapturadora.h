/**************************************************************************************************
 * Archivo: uHALadc.h
 * Breve:
 * Fecha:
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UCAPTURADORA_H_
#define ISPEL_UCAPTURADORA_H_

/****** Librerías (includes) *********************************************************************/

#include "uHALadc.h"
#include "uOSAL.h"

/****** Definiciones públicas (macros) ***********************************************************/

#define CAPTURADORAS_CANTIDAD          		2

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	CAPTURADORA_1,
	CAPTURADORA_2,
	CAPTURADORA_3,
	CAPTURADORAS_TODAS
} capturadora_id_e;

typedef enum {
	SUBIDA,
	BAJA,
	AMBOS
} flanco_e;

typedef struct {
	float    EscalaHorizontal;	// Intervalo almacenado desde disparo
	float    EscalaVertical;	// Máxima tensión positiva
	double   NivelDisparo;
	flanco_e FlancoDisparo;
} capturadora_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/


/*************************************************************************************************/
#endif /*  */
/****************************************************************** FIN DE ARCHIVO ***************/
