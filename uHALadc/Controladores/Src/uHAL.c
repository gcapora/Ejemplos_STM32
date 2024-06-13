/**************************************************************************************************
 * Archivo: uHAL.c
 * Breve:
 * Fecha:	Creado en abril 2024
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "stm32f4xx_hal.h"
#include "uHAL.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos privados *************************************************************/


/****** Importación de datos públicos ************************************************************/


/****** Declaración de funciones privadas ********************************************************/


/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
  * @brief
  * @param
  * @retval
  */
bool uHALinicializar ( void )
{
  bool control = true;

  // Empezamos por inicializar uOSAL:
  if ( false == uoInicializar () ) control = false;

  // Acá estarán todos los módulos que necesiten inicialización general.
  //if ( uHALadcInicializar () != true ) control = false;

  return control;
}

/****************************************************************** FIN DE ARCHIVO ***************/
