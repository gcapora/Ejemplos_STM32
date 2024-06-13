/***************************************************************************************************
 * Archivo: uOSAL.c
 * Breve:	Capa de abstracción de sistema operativo (OSAL) para PC.
 *
 * 			USO ACOTADO A USENIALES.H
 *
 **************************************************************************************************/

/****** Librerías (includes) **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "uOSAL.h"

/****** Definiciones privadas (macros) ************************************************************/


/****** Definiciones privadas de tipos (private typedef) ******************************************/

//#define UOSAL_UART_TIEMPO_ESPERA_US		UOSAL_UART_TIEMPO_ESPERA*1000  // Mismo valor pero en us

/****** Definición de datos privados **************************************************************/

bool	uOSALinicializada = false;

/****** Importación de datos públicos **************************************************************/


/****** Declaración de funciones privadas *********************************************************/


/****** Definición de funciones públicas **********************************************************/

/*****************************************************************************************
* @brief	Inicializa el módulo OSAL
* @param	Ninguno
* @retval	true si tuvo éxito
*/
bool uoInicializar ( void )
{
	// Verificación e inicialización...
	if ( false == uOSALinicializada ) {
		// Actualizo estado de uOSAL
		uOSALinicializada = true;
	}
	return uOSALinicializada;
}

/*****************************************************************************************
 * @brief	Maneja el error
 * @param	Ninguno
 * @retval  Ninguno
 */
void uoHuboError (void)   // TODO que coincida con error de freertos
{
	uoHuboErrorTxt ("");
}

/*****************************************************************************************
 * @brief	Maneja el error
 * @param	Texto a escribir cuando sucede este error
 * @retval  Ninguno
 */
void uoHuboErrorTxt ( const char * P_TEXTO )
{
    printf("ERROR: %s\n", P_TEXTO);
	exit(EXIT_FAILURE); 
}


/*****************************************************************************************
* @brief	Me devuelve el tiempo en milisegundos
* @param	Ninguno
* @retval	Milisegundo actual
*/
uint32_t uoMilisegundos( void )
{
	struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_usec / 1000);
}

/*****************************************************************************************
* @brief	Me devuelve el tiempo en microsegundos
* @param	Ninguno
* @retval	Milisegundo actual
*/
uint32_t uoMicrosegundos (void)
{
	struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_usec);
}

/*****************************************************************************************
* @brief		Espero una cantidad dada de milisegundos
* @param		Milisegundos a esperar
* @retval	Ninguno
*/
void uoEsperarMilis ( uint32_t MILIS )
{
	MILIS = ( MILIS>60000 ) ? 60000 : MILIS; // A lo sumo esperamos 1 minuto
	usleep(MILIS * 1000);
}

/*****************************************************************************************
* @brief		Espero una cantidad dada de microsegundos
* @param		microsegundos a esperar
* @retval	ninguno
*/
void uoEsperarMicros ( uint32_t MICRO )
{
	MICRO = ( MICRO>10e6 ) ? 10e6 : MICRO; // A lo sumo 10 segundos con esta función
	usleep(MICRO);
}

/*-------------------------------------------------------------------------------------------------
* @brief	Enciende, apaga o invierte el estado de un led incorporado a la placa
* @param	Led
* @retval  Ninguno
void     uoLedEncender   ( uint16_t PIN )
{
	HAL_GPIO_WritePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN, GPIO_PIN_SET );
}

void     uoLedApagar     ( uint16_t PIN )
{
	HAL_GPIO_WritePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN, GPIO_PIN_RESET );
}

void     uoLedInvertir   ( uint16_t PIN )
{
	HAL_GPIO_TogglePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN );
}
*/

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe texto por UART
* @param		Puntero a texto
* @retval	Ninguno
void uoEscribirTxt ( const char * P_TEXTO)
{
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO );
}
*/

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe un número por UART
* @param		Número entero positivo
* @retval	Ninguno
void uoEscribirUint ( uint32_t ENTEROP)
{
    uint8_t Cadena[16] = {0};  // Suficiente para un uint32_t
    sprintf( (char *) Cadena, "%lu", ENTEROP);
    xSemaphoreTake( MutexEscribirAdmin, portMAX_DELAY );
    UART_ENVIAR_CADENA (Cadena);
 	 xSemaphoreGive( MutexEscribirAdmin );
}
*/

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe texto y texto por UART
* @param		Puntero a texto
* @param		Puntero a texto
* @retval	Ninguno
void uoEscribirTxtTxt ( const char * P_TEXTO, const char * P_TEXTO2 )
{
   xSemaphoreTake( MutexEscribirAdmin, portMAX_DELAY );
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);
  	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO2);
	xSemaphoreGive( MutexEscribirAdmin );
}
*/

/*void uoEscribirTxt2 ( const char * P_TEXTO, const char * P_TEXTO2 )
{
	uoEscribirTxtTxt ( P_TEXTO, P_TEXTO2 );
}*/

/*void uoEscribirTxtTxtTxt ( const char * P_TEXTO, const char * P_TEXTO2, const char * P_TEXTO3 )
{
	xSemaphoreTake( MutexEscribirAdmin, portMAX_DELAY );
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO2);
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO3);
	xSemaphoreGive( MutexEscribirAdmin );
}
*/

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe texto y un número por UART
* @param		Puntero a texto y número entero positivo
* @retval	Ninguno
void uoEscribirTxtUint ( const char * P_TEXTO, uint32_t ENTEROP)
{
	uint8_t Cadena[16] = {0};
	sprintf( (char *) Cadena, "%lu", ENTEROP);
	xSemaphoreTake( MutexEscribirAdmin, portMAX_DELAY );
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO );
   UART_ENVIAR_CADENA ( Cadena );
	xSemaphoreGive( MutexEscribirAdmin );
}
*/

/*******************************************************************************
* @brief		Escribe texto, un número y texto por UART
* @param		Puntero a texto
* @param		Número entero positivo
* @param		Puntero a texto
* @retval	Ninguno
void uoEscribirTxtUintTxt ( const char * P_TEXTO, uint32_t ENTEROP, const char * P_TEXTO2)
{
	uint8_t Cadena[16] = {0};
	sprintf( (char *) Cadena, "%lu", ENTEROP);
	xSemaphoreTake( MutexEscribirAdmin, portMAX_DELAY );
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);
   UART_ENVIAR_CADENA ( Cadena );
  	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO2);
	xSemaphoreGive( MutexEscribirAdmin );
}
*/

/*******************************************************************************
  * @brief  Lee por UART un caracter
  * @param  Puntero donde guardar un char
  * @param  Tiempo de espera [us]
  * @retval true si leyó algo
bool uoLeerChar (char * caracter, uint32_t Espera)
{
	// Variables locales;
	HAL_StatusTypeDef Resultado;
	uint32_t				TiempoInicio;

	// Precondiciones
	if (caracter == NULL) uoHuboErrorTxt("en uLeerChar de uOSAL.");
	Espera = (Espera>UOSAL_UART_TIEMPO_ESPERA_US) ? UOSAL_UART_TIEMPO_ESPERA_US : Espera;

	// Recibo...
	TiempoInicio = uoMicrosegundos();
	do {
		Resultado = HAL_UART_Receive(&uart_e, (uint8_t *) caracter, 1, 0);
	} while ( (uoMicrosegundos()-TiempoInicio < Espera) && (Resultado != HAL_OK) );
	if (Resultado == HAL_OK) return true;
	return false;
}
  */

/*******************************************************************************
  * @brief  Lee por UART una cantidad máxima de caracteres
  * @param  Puntero donde guardar vector de char
  * @param  Cantidad máxima de char
  * @param  Tiempo de espera [us]
  * @retval true si leyó algo
uint32_t uoLeerTxt ( char * TEXTO, uint32_t CANTIDAD, uint32_t ESPERA )
{
	// Variables locales
	uint32_t TiempoInicio = 0;
	uint32_t Indice = 0;

	// Precondiciones
	if (TEXTO == NULL) uoHuboErrorTxt("en uLeerTxt de uOSAL.");
	if (0==CANTIDAD)   return 0;
	CANTIDAD = (CANTIDAD > UOSAL_UART_LARGO_MAXIMO    ) ? UOSAL_UART_LARGO_MAXIMO     : CANTIDAD;
	ESPERA   = (ESPERA   > UOSAL_UART_TIEMPO_ESPERA_US) ? UOSAL_UART_TIEMPO_ESPERA_US : ESPERA;

	// Lectura
	TiempoInicio = uoMicrosegundos();
	do {
		if ( uoLeerChar ( &TEXTO[Indice], 0 ) ) {
			// Leímos un caracter!!!
			Indice++;
		}
	} while ( (uoMicrosegundos()-TiempoInicio < ESPERA) && (Indice < CANTIDAD) );
	if (Indice<CANTIDAD) TEXTO[Indice]='\0';  // Esto es para cumplir con formato de cadena
	return Indice;
}
  */

/****** Definición de funciones privadas *********************************************************/


/****************************************************************** FIN DE ARCHIVO ***************/
