/**************************************************************************************************
 * Archivo: uOSAL.c
 * Breve:	Capa de abstracción de sistema operativo (OSAL) para bare-metal STM32
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "uOSAL.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos privados *************************************************************/

TIM_HandleTypeDef  htim_microsegundos;	// Temporizador para uOSALmicrosegundos().
UART_HandleTypeDef uart_e;
bool               uOSALinicializada = false;

/****** Importación de datos públicos ************************************************************/


/****** Declaración de funciones privadas ********************************************************/

void MICROSEGUNDOS_TEMPO_INICIALIZAR (void);
bool UART_INICIALIZAR (void);
void UART_ENVIAR_CADENA (uint8_t *);

/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
  * @brief  Inicializa el módulo
  * @param
  * @retval
  */
bool uOSALinicializar ( void )
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
uint32_t uMilisegundos ( void )
{
	return (uint32_t) HAL_GetTick(); // Esta función está en milisegundos.
}

/**------------------------------------------------------------------------------------------------
  * @brief  Me devuelve el tiempo en milisegundos
  * @param	Ninguno
  * @retval Milisegundo actual
  */
uint32_t uMicrosegundos (void)
{
	return (uint32_t) __HAL_TIM_GET_COUNTER (&htim_microsegundos);
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Maneja el error
 * @param	Ninguno
 * @retval  Ninguno
 */
void uHuboErrorTxt (char * P_TEXTO)
{
	const uint32_t intervalo=100000;
	uint32_t delta;
	uEscribirTxt("ERROR ");
	uEscribirTxt( P_TEXTO );
	uEscribirTxt("\n\r");
	__disable_irq();
	//uLedApagar ( UOSAL_PIN_LED_VERDE_INCORPORADO );
	uLedEncender ( UOSAL_PIN_LED_ROJO_INCORPORADO );
	delta = uMicrosegundos();
	while (1)
	{
		if ( (uMicrosegundos()-delta) > intervalo) {
			delta+=intervalo;
			uLedInvertir ( UOSAL_PIN_LED_ROJO_INCORPORADO );
		}
	}
}

void uHuboError ( void )
{
	uHuboErrorTxt ("");
}


/*-------------------------------------------------------------------------------------------------
 * @brief	Enciende, apaga o invierte el estado de un led incorporado a la placa
 * @param	Led
 * @retval  Ninguno
 */
void     uLedEncender   ( uint16_t PIN )
{
	HAL_GPIO_WritePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN, GPIO_PIN_SET );
}

void     uLedApagar     ( uint16_t PIN )
{
	HAL_GPIO_WritePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN, GPIO_PIN_RESET );
}

void     uLedInvertir   ( uint16_t PIN )
{
	HAL_GPIO_TogglePin ( UOSAL_PUERTO_LEDS_INCORPORADOS, PIN );
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto por UART
 * @param	Puntero a texto
 * @retval  Ninguno
 */
void     uEscribirTxt ( char * P_TEXTO)
{
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO );
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto y un número por UART
 * @param	Puntero a texto y número entero positivo
 * @retval  Ninguno
 */
void     uEscribirTxtUint ( char * P_TEXTO, uint32_t ENTEROP)
{
    // Primero texto:
	UART_ENVIAR_CADENA ( (uint8_t *) P_TEXTO);

	// Luego número:
    uint8_t Cadena[16] = {0};
    sprintf( (char *) Cadena, "%lu", ENTEROP);
    UART_ENVIAR_CADENA (Cadena);
}

/*-------------------------------------------------------------------------------------------------
 * @brief	Escribe texto y un número por UART
 * @param	Puntero a texto y número entero positivo
 * @retval  Ninguno
 */
void     uEscribirUint ( uint32_t ENTEROP)
{
    uint8_t Cadena[16] = {0};  // Suficiente para un uint32_t
    sprintf( (char *) Cadena, "%lu", ENTEROP);
    UART_ENVIAR_CADENA (Cadena);
}

/*******************************************************************************
  * @brief  Recibe por UART una cantidad definida de caracteres
  * @param  Puntero donde guardar char
  * @param  Tiempo de espera
  * @retval true si leyó algo
  */
bool uLeerChar (uint8_t * caracter, uint32_t tiempo) {
	// Variables locales;
	HAL_StatusTypeDef Resultado;

	// Precondiciones
	if (caracter == NULL) uHuboErrorTxt("en uLeerChar de uOSAL.");
	tiempo = (tiempo>1000) ? 1000 : tiempo;

	// Recibo...
	Resultado = HAL_UART_Receive(&uart_e, (uint8_t *) caracter, 1, tiempo);
	if (Resultado == HAL_OK) return true;
	return false;
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
	  uHuboErrorTxt ("inicialiando tempo microsegundos (HAL_TIM_Base_Init).");
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim_microsegundos, &sClockSourceConfig) != HAL_OK)
  {
	  uHuboErrorTxt ("inicialiando tempo microsegundos (HAL_TIM_ConfigClockSource).");
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim_microsegundos, &sMasterConfig) != HAL_OK)
  {
	  uHuboErrorTxt ("inicialiando tempo microsegundos (HAL_TIMEx_...).");
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
  uart_e.Init.BaudRate     = 115200;
  uart_e.Init.WordLength   = UART_WORDLENGTH_8B;
  uart_e.Init.StopBits     = UART_STOPBITS_1;
  uart_e.Init.Parity       = UART_PARITY_NONE;
  uart_e.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  uart_e.Init.Mode         = UART_MODE_TX_RX;
  uart_e.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&uart_e) != HAL_OK) {
	  uHuboError();
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
	if (pstring == NULL) uHuboError();

	// Cuento cantidad de caracteres de la cadena a enviar:
	uint16_t size = (uint16_t) strlen((char *) pstring);

	// Verifico que el largo no supere el máximo configurado:
	size = (size<UOSAL_UART_LARGO_MAXIMO) ? size : UOSAL_UART_LARGO_MAXIMO;

	// Envío:
	HAL_UART_Transmit ( &uart_e, (uint8_t *) pstring, size, UOSAL_UART_TIEMPO_ESPERA );
}


/****************************************************************** FIN DE ARCHIVO ***************/
