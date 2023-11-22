/**************************************************************************************************
 * Archivo: uOSAL.h
 * Breve:	Capa de abstracción de sistema operativo (OSAL). Proyecto ISPEL.
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UOSAL_H_
#define ISPEL_UOSAL_H_

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>

/****** Definiciones públicas (macros) ***********************************************************/


/****** Definiciones públicas de tipos de datos (public typedef) *********************************/


/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

uint32_t uOSALmiliseg( void );
void uManejaError ( void );

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
