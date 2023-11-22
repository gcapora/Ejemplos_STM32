/**************************************************************************************************
 * Archivo: uOSAL.c
 * Breve:	Capa de abstracción de sistema operativo (OSAL) para bare-metal STM32
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "stm32f4xx_hal.h"
#include "main.h"				// Acá debe estar Error_handler
//#include "cmsis_os.h"
//#include "FreeRTOS.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos públicos *************************************************************/


/****** Declaración de funciones privadas ********************************************************/


/****** Definición de funciones privadas *********************************************************/


/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
  * @brief  Me devuelve el tiempo en milisegundos
  * @param	Ninguno
  * @retval Milisegundo actual
  */
uint32_t uOSALmiliseg( void )
{
	return (uint32_t) HAL_GetTick(); // Esta función está en milisegundos.
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Maneja el error
 * @param	Ninguno
 * @retval  Ninguno
 */
void uManejaError (void)
{
	Error_Handler();
}




/****************************************************************** FIN DE ARCHIVO ***************/
