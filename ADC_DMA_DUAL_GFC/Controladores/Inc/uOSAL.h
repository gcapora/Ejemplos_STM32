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
#include "uOSALconfig.h"
#include "API_uart.h" // provisorio para prueba

/****** Definiciones públicas (macros) ***********************************************************/


/****** Definiciones públicas de tipos de datos (public typedef) *********************************/


/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

void     uOSALinicializar   ( void );
uint32_t uOSALmilisegundos  ( void );
uint32_t uOSALmicrosegundos ( void );
void     uHuboError         ( void );
void     uOSALledEncender   ( uint16_t );
void     uOSALledApagar     ( uint16_t );
void     uOSALledInvertir   ( uint16_t );
void     uEscribirTexto         ( char * );
void     uEscribirTextoEnteroSS ( char *, uint32_t );
void     uEscribirEnteroSS      ( uint32_t );

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
