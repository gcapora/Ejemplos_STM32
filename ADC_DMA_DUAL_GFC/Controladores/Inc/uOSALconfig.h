/**************************************************************************************************
 * Archivo: uOSALconfig.h
 * Breve:	Capa de abstracción de sistema operativo (OSAL). Proyecto ISPEL.
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UOSAL_CONFIG_H_
#define ISPEL_UOSAL_CONFIG_H_

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>

/****** Definiciones públicas (macros) ***********************************************************/

// Contador de microsegundos
#define UOSAL_MICROSEGUNDOS_TEMPO        TIM5

// Leds incorporados
#define UOSAL_PUERTO_LEDS_INCORPORADOS   GPIOB
#define UOSAL_PIN_LED_ROJO_INCORPORADO   GPIO_PIN_14
#define UOSAL_PIN_LED_VERDE_INCORPORADO  GPIO_PIN_0
#define UOSAL_PIN_LED_AZUL_INCORPORADO   GPIO_PIN_7

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/


/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/


/*************************************************************************************************/
#endif /* ISPEL_UOSAL_CONFIG_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
