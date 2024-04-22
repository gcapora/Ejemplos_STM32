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

extern TIM_HandleTypeDef htim5;   // En este ejemplo, dejamos su definición en tim.c

/****** Declaración de funciones privadas ********************************************************/


/****** Definición de funciones privadas *********************************************************/


/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
  * @brief  Inicializa el módulo
  * @param
  * @retval
  */
void uOSALinicializar ( void )
{
	// Inicialización depuerto serie para mensajes:
	uartInit();

	// Base de tiempo de uOSALmicrosegundos():
	__HAL_TIM_ENABLE(&htim5);	// ¿o HAL_TIM_Base_Start(...)?

	// Inicialización de leds incorporados:
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin = UOSAL_PIN_LED_ROJO_INCORPORADO | UOSAL_PIN_LED_VERDE_INCORPORADO | UOSAL_PIN_LED_AZUL_INCORPORADO;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init( UOSAL_PUERTO_LEDS_INCORPORADOS, &GPIO_InitStruct );

}

/**------------------------------------------------------------------------------------------------
  * @brief  Me devuelve el tiempo en milisegundos
  * @param	Ninguno
  * @retval Milisegundo actual
  */
uint32_t uOSALmilisegundos ( void )
{
	return (uint32_t) HAL_GetTick(); // Esta función está en milisegundos.
}

/**------------------------------------------------------------------------------------------------
  * @brief  Me devuelve el tiempo en milisegundos
  * @param	Ninguno
  * @retval Milisegundo actual
  */
uint32_t uOSALmicrosegundos (void)
{
	return (uint32_t) __HAL_TIM_GET_COUNTER (&htim5);
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
 * @brief	Enciende, apaga o invierte el estado de un led incorporado a la placa
 * @param	Led
 * @retval  Ninguno
 */
void     uOSALledEncender   ( uint16_t PIN )
{
	HAL_GPIO_WritePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN, GPIO_PIN_SET );
}

void     uOSALledApagar     ( uint16_t PIN )
{
	HAL_GPIO_WritePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN, GPIO_PIN_RESET );
}

void     uOSALledInvertir   ( uint16_t PIN )
{
	HAL_GPIO_TogglePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN );
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
void     uEscribirTextoEnteroSS ( char * P_TEXTO, uint32_t ENTEROP)
{
    // Primero texto:
	uartSendString( (uint8_t *) P_TEXTO);

	// Luego número:
    uint8_t Cadena[100] = {0};
    sprintf( (char *) Cadena, "%lu", ENTEROP);
    uartSendString(Cadena);

}

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto y un número por UART
 * @param	Puntero a texto y número entero positivo
 * @retval  Ninguno
 */
void     uEscribirEnteroSS ( uint32_t ENTEROP)
{

	// Luego número:
    uint8_t Cadena[100] = {0};
    sprintf( (char *) Cadena, "%lu", ENTEROP);
    uartSendString(Cadena);

}

/****************************************************************** FIN DE ARCHIVO ***************/
