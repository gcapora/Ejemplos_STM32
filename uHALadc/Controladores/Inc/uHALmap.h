/**************************************************************************************************
 * Archivo: uHALmap.h
 * Breve:
 * Fecha:
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UHAL_MAP_H_
#define ISPEL_UHAL_MAP_H_

/****** Librerías (includes) *********************************************************************/

#include "uHAL.h"

/****** Definiciones públicas (macros) ***********************************************************/


/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	UHAL_MAP_PE5,
	UHAL_MAP_2,
	UHAL_MAP_3,
	UHAL_MAP_TODOS
} map_id_e;

typedef struct {
	double   Frecuencia;
	float    CicloDeTrabajo;
} map_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool		uHALmapInicializar    			( map_id_e );                 // Config estática en uHALconfig.h
bool		uHALmapDesInicializar 			( map_id_e );                 // Falta implementar
bool		uHALmapEncender     				( map_id_e );
bool		uHALmapApagar         			( map_id_e );
bool		uHALmapConfigurar     			( map_id_e, map_config_s * ); // Config dinámica
double	uHALmapConfigurarFrecuencia	( map_id_e, double );
double	uHALmapObtenerFrecuencia		( map_id_e );

/*************************************************************************************************/
#endif /* ISPEL_UHAL_MAP_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
