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
#include <stdbool.h>
#include "API_uart.h" // provisorio para prueba

/****** Definiciones públicas (macros) ***********************************************************/

#define MILLON                      		1000000   // 1 millon :-)
#define MHZ                        		 	MILLON    // 1 megahertz
#define KHZ                         		1000      // 1 kilohertz

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/


/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

uint32_t uOSALmiliseg ( void );
void     uHuboError   ( void );
void     uEscribirTexto         ( char * );
void     uEscribirTextoEnteroSS ( char *, uint32_t );
void     uEscribirEnteroSS      ( uint32_t );

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
