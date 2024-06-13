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

typedef bool bool_t;   // Esto es una finura :-)

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool     uoInicializar     	( void );

uint32_t uoMilisegundos      	( void );
uint32_t uoMicrosegundos   	( void );
void		uoEsperarMilis			( uint32_t );
void		uoEsperarMicros		( uint32_t );

void     uoLedEncender       	( uint16_t );
void     uoLedApagar         	( uint16_t );
void     uoLedInvertir       	( uint16_t );

void     uoEscribirTxt        ( char * );
void     uoEscribirUint      	( uint32_t );
void     uoEscribirTxtUint		( char *, uint32_t );
void     uoEscribirTxtTxt     ( const char *, const char * );
void     uoEscribirTxtUintTxt	( char *, uint32_t, char * );
bool		uoLeerChar 				( char *, uint32_t );
uint32_t uoLeerTxt 				( char *, uint32_t, uint32_t );

void     uoHuboError          ( void );
void     uoHuboErrorTxt       ( char * );

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
