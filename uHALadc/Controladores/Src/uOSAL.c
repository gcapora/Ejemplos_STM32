/**************************************************************************************************
 * Archivo: uOSAL.c
 * Breve:	Capa de abstracción de sistema operativo (OSAL) para bare-metal STM32.
 * Fecha:	Desarrollado en 2024
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "uOSAL.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define UOSAL_UART_TIEMPO_ESPERA_US		UOSAL_UART_TIEMPO_ESPERA*1000  // Mismo valor pero en us

/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos privados *************************************************************/

bool               uOSALinicializada = false;
UART_HandleTypeDef uart_e;
TIM_HandleTypeDef  htim_microsegundos;	// Temporizador para uOSALmicrosegundos().

/****** Importación de datos públicos ************************************************************/


/****** Declaración de funciones privadas ********************************************************/

bool UART_INICIALIZAR (void);
void MICROSEGUNDOS_TEMPO_INICIALIZAR (void);
void UART_ENVIAR_CADENA (uint8_t *);

/****** Definición de funciones públicas *********************************************************/

/*-------------------------------------------------------------------------------------------------
* @brief	Inicializa el módulo OSAL
* @param	Ninguno
* @retval	true si tuvo éxito
*/
bool uoInicializar ( void )
{
	if ( false == uOSALinicializada ) {
		// Inicialización depuerto serie para mensajes:
		UART_INICIALIZAR();

		// Base de tiempo de uOSALmicrosegundos():
		MICROSEGUNDOS_TEMPO_INICIALIZAR();

		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();

		// Inicialización de leds incorporados:
		GPIO_InitTypeDef  GPIO_InitStruct;
		GPIO_InitStruct.Pin = UOSAL_PIN_LEDS_INCORPORADOS_TODOS;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init( UOSAL_PUERTO_LEDS_INCORPORADOS, &GPIO_InitStruct );

		uOSALinicializada = true;
	}
	return uOSALinicializada;
}

/**------------------------------------------------------------------------------------------------
  * @brief  Me devuelve el tiempo en milisegundos
  * @param	Ninguno
  * @retval Milisegundo actual
  */
uint32_t uoMilisegundos ( void )
{
	return (uint32_t) HAL_GetTick(); // Esta función está en milisegundos.
}

/**------------------------------------------------------------------------------------------------
  * @brief  Me devuelve el tiempo en milisegundos
  * @param	Ninguno
  * @retval Milisegundo actual
  */
uint32_t uoMicrosegundos (void)
{
	return (uint32_t) __HAL_TIM_GET_COUNTER (&htim_microsegundos);
}

/**------------------------------------------------------------------------------------------------
* @brief		Espero una cantidad dada de microsegundos
* @param		microsegundos a esperar
* @retval	ninguno
*/
void uoEsperarMicros ( uint32_t MICRO )
{
	uint32_t Tiempo = uoMicrosegundos();
	MICRO = ( MICRO>5e6 ) ? 5e6 : MICRO; // A lo sumo 5 segundos con esta función
	while (uoMicrosegundos()-Tiempo < MICRO ) {
		// Espero...
	}
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief		Espero una cantidad dada de milisegundos
* @param		Milisegundos a esperar
* @retval	Ninguno
*/
void uoEsperarMilis ( uint32_t MILIS )
{
	uint32_t Tiempo = uoMicrosegundos();
	MILIS = ( MILIS>60000 ) ? 60000 : MILIS; // A lo sumo esperamos 1 minuto
	MILIS = 1000 * MILIS; // Adaptación porque preferimos usar uoMicrosegundos()
	while (uoMicrosegundos()-Tiempo < MILIS ) {
		// Espero...
	}
	return;
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Maneja el error
 * @param	Ninguno
 * @retval  Ninguno
 */
void uoHuboErrorTxt (char * P_TEXTO)
{
	const uint32_t intervalo=100000;
	uint32_t delta;
	uoEscribirTxt("ERROR ");
	uoEscribirTxt( P_TEXTO );
	uoEscribirTxt("\n\r");
	__disable_irq();
	//uLedApagar ( UOSAL_PIN_LED_VERDE_INCORPORADO );
	uoLedEncender ( UOSAL_PIN_LED_ROJO_INCORPORADO );
	delta = uoMicrosegundos();
	while (1)
	{
		if ( (uoMicrosegundos()-delta) > intervalo) {
			delta+=intervalo;
			uoLedInvertir ( UOSAL_PIN_LED_ROJO_INCORPORADO );
		}
	}
}

void uoHuboError ( void )
{
	uoHuboErrorTxt ("");
}


/*-------------------------------------------------------------------------------------------------
 * @brief	Enciende, apaga o invierte el estado de un led incorporado a la placa
 * @param	Led
 * @retval  Ninguno
 */
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

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto por UART
 * @param	Puntero a texto
 * @retval  Ninguno
 */
void     uoEscribirTxt ( char * P_TEXTO)
{
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO );
}

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe un número por UART
* @param		Número entero positivo
* @retval	Ninguno
*/
void     uoEscribirUint ( uint32_t ENTEROP)
{
    uint8_t Cadena[16] = {0};  // Suficiente para un uint32_t
    sprintf( (char *) Cadena, "%lu", ENTEROP);
    UART_ENVIAR_CADENA (Cadena);
}

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe texto y un número por UART
* @param		Puntero a texto y número entero positivo
* @retval	Ninguno
*/
void     uoEscribirTxtUint ( char * P_TEXTO, uint32_t ENTEROP)
{
	// Primero texto:
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);

	// Luego número:
   uint8_t Cadena[16] = {0};
   sprintf( (char *) Cadena, "%lu", ENTEROP);
   UART_ENVIAR_CADENA (Cadena);
}

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe texto y texto por UART
* @param		Puntero a texto
* @param		Puntero a texto
* @retval	Ninguno
*/
void uoEscribirTxtTxt ( const char * P_TEXTO, const char * P_TEXTO2)
{
   // Primero texto:
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);

   // Finalmente texto2:
  	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO2);
}

/*-------------------------------------------------------------------------------------------------
* @brief		Escribe texto, un número y texto por UART
* @param		Puntero a texto
* @param		Número entero positivo
* @param		Puntero a texto
* @retval	Ninguno
*/
void uoEscribirTxtUintTxt ( char * P_TEXTO, uint32_t ENTEROP, char * P_TEXTO2)
{
   // Primero texto:
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);

	// Luego número:
   uint8_t Cadena[16] = {0};
   sprintf( (char *) Cadena, "%lu", ENTEROP);
   UART_ENVIAR_CADENA (Cadena);

   // Finalmente texto2:
  	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO2);
}

/*******************************************************************************
  * @brief  Lee por UART un caracter
  * @param  Puntero donde guardar un char
  * @param  Tiempo de espera [us]
  * @retval true si leyó algo
  */
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

/*******************************************************************************
  * @brief  Lee por UART una cantidad máxima de caracteres
  * @param  Puntero donde guardar vector de char
  * @param  Cantidad máxima de char
  * @param  Tiempo de espera [us]
  * @retval true si leyó algo
  */
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


/****** Definición de funciones privadas *********************************************************/

/*-------------------------------------------------------------------------------------------------
 * @brief	Inicializa tempo de uOSALmicrosegundos()
 * @param   Ninguno
 * @retval  Ninguno
 */
void MICROSEGUNDOS_TEMPO_INICIALIZAR(void)
{
  TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim_microsegundos.Instance = UOSAL_MICROSEGUNDOS_TEMPO;
  htim_microsegundos.Init.Prescaler = 71;
  htim_microsegundos.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim_microsegundos.Init.Period = 4294967295;
  htim_microsegundos.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim_microsegundos.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim_microsegundos) != HAL_OK)
  {
	  uoHuboErrorTxt ("inicialiando tempo microsegundos (HAL_TIM_Base_Init).");
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim_microsegundos, &sClockSourceConfig) != HAL_OK)
  {
	  uoHuboErrorTxt ("inicialiando tempo microsegundos (HAL_TIM_ConfigClockSource).");
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim_microsegundos, &sMasterConfig) != HAL_OK)
  {
	  uoHuboErrorTxt ("inicialiando tempo microsegundos (HAL_TIMEx_...).");
  }

  // Iniciamos conteo:
  HAL_TIM_Base_Start (&htim_microsegundos);
}

/*******************************************************************************
 * @brief  Inicializa la conexión por UART
 * @param  None
 * @retval None
 */
bool UART_INICIALIZAR (void) {
  /*## Configure the UART peripheral ########################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) :
	                  BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 115200
      - Hardware flow control disabled (RTS and CTS signals) */
  /*##########################################################################*/

  uart_e.Instance          = USARTx;
  uart_e.Init.BaudRate     = UOSAL_UART_BAUDIOS;
  uart_e.Init.WordLength   = UART_WORDLENGTH_8B;
  uart_e.Init.StopBits     = UART_STOPBITS_1;
  uart_e.Init.Parity       = UART_PARITY_NONE;
  uart_e.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  uart_e.Init.Mode         = UART_MODE_TX_RX;
  uart_e.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&uart_e) != HAL_OK) {
	  uoHuboError();
  }
  return true;
}

/*******************************************************************************
 * @brief  Transmite por UART una cadena completa
 * @param  Puntero a cadena a enviar
 * @retval None
 */
void UART_ENVIAR_CADENA (uint8_t * pstring) {
	// Verificamos puntero:
	if (pstring == NULL) uoHuboError();

	// Cuento cantidad de caracteres de la cadena a enviar:
	uint16_t size = (uint16_t) strlen((char *) pstring);

	// Verifico que el largo no supere el máximo configurado:
	size = (size<UOSAL_UART_LARGO_MAXIMO) ? size : UOSAL_UART_LARGO_MAXIMO;

	// Envío:
	HAL_UART_Transmit ( &uart_e, (uint8_t *) pstring, size, UOSAL_UART_TIEMPO_ESPERA );
}


/****************************************************************** FIN DE ARCHIVO ***************/
