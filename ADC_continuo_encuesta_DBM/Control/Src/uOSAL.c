/**************************************************************************************************
 * Archivo: uOSAL.c
 * Breve:	Capa de abstracción de sistema operativo (OSAL) para bare-metal STM32
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "uOSAL.h"
#include "main.h"				// Acá debe estar Error_handler
#include "stm32f4xx_hal.h"

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
void uHuboError (void)
{
	  __disable_irq();
	  while (1)
	  {
	  }
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto por UART
 * @param	Puntero a texto
 * @retval  Ninguno
 */
void     uEscribirTexto ( char * P_TEXTO)
{
	uartSendString( (uint8_t *) P_TEXTO);
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto y un número por UART
 * @param	Puntero a texto y número entero positivo
 * @retval  Ninguno
 */
void     uEscribirTextoEnteroP ( char * P_TEXTO, uint32_t ENTEROP)
{
    // Primero texto:
	uartSendString( (uint8_t *) P_TEXTO);

	// Luego número:
    uint8_t Cadena[100] = {0};
    sprintf( (char *) Cadena, " %lu\n\r", ENTEROP);
    uartSendString(Cadena);

}

/****************************************************************** FIN DE ARCHIVO ***************/
