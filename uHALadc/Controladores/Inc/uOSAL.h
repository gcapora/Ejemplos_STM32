/**************************************************************************************************
 * Archivo: uOSAL.h
 * Breve:	Capa de abstracción de sistema operativo (OSAL).
 *          Incluye un mínimo de funciones de soporte.
 *          Creado para proyecto ISPEL.
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UOSAL_H_
#define ISPEL_UOSAL_H_

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "uOSALconfig.h"

/****** Definiciones públicas (macros) ***********************************************************/


/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef bool bool_t;   // Esto es una finura... :-)

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool     uOSALinicializar       	( void );
uint32_t uMilisegundos      		( void );
uint32_t uMicrosegundos   			( void );
void     uLedEncender       		( uint16_t );
void     uLedApagar         		( uint16_t );
void     uLedInvertir       		( uint16_t );
void     uEscribirTexto         	( char * );
void     uEscribirTextoEnteroSS	( char *, uint32_t );
void     uEscribirEnteroSS      	( uint32_t );
void     uHuboError             	( void );

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
