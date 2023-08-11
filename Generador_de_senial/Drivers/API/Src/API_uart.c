/*******************************************************************************
* @file    API_uart.c
* @author  Guillermo Caporaletti
* @brief   Módulo para manejo de puerto UART.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "API_uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define and const --------------------------------------------------*/
const uint32_t uartTimeOut = 0x004F;
const uint16_t maxSize = 0x0FFF;		// Cantidad máxima de símbolos a
										// transmitir/recibir  por vez

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/*******************************************************************************
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

/* Exported functions --------------------------------------------------------*/

/*******************************************************************************
  * @brief  Inicializa la conexión por UART
  * @param  None
  * @retval None
  */
bool_t uartInit() {
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) : 
	                  BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;
  UartHandle.Init.BaudRate     = 9600;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  /*##########################################################################*/
  if (HAL_UART_Init(&UartHandle) == HAL_OK) {
	  char Cadena[32];
	  uartSendString((uint8_t *) "CONEXION UART ESTABLECIDA:\n");

	  uartSendString((uint8_t *) "Baudios = ");
	  sprintf(Cadena, "%ld", UartHandle.Init.BaudRate);
	  //itoa((int) UartHandle.Init.BaudRate, Cadena, 10);
	  uartSendString((uint8_t *) Cadena);
	  uartSendString((uint8_t *) "\n\r");

	  uartSendString((uint8_t *) "Largo de palabra = 8\n\r");
	  uartSendString((uint8_t *) "Bits de parada = 1\n\r");
	  uartSendString((uint8_t *) "Paridad = Ninguna\n\r");

	  // Esto se podría hacer mejor, pero implicaría traducir cada constante.

  }

  return (HAL_UART_Init(&UartHandle) == HAL_OK);
}

/*******************************************************************************
  * @brief  Transmite por UART una cadena completa
  * @param  Puntero a cadena a enviar
  * @retval None
  */
void uartSendString(uint8_t * pstring) {
	// ¿El puntero es válido?
	if (pstring == NULL) Error_Handler();

	// Cuento cantidad de caracteres de la cadena a enviar:
	uint16_t size = (uint16_t) strlen((char *) pstring);
		/* Debido a esta función "strlen" debo incluir <string.h>.
		 * Se podria hacer buscando "\0" con un for.
		 * Atención con el casteo (char *).
		 */

	// Verifico que el largo no supere el máximo configurado:
	size = (size<maxSize) ? size : maxSize;

	// Envío!!!
	HAL_UART_Transmit(&UartHandle, (uint8_t *) pstring, size, uartTimeOut);
}

/*******************************************************************************
  * @brief  Transmite por UART una cadena de largo definido
  * @param  None
  * @retval None
  */
void uartSendStringSize(uint8_t * pstring, uint16_t size) {
	// ¿El puntero es válido?
	if (pstring == NULL) Error_Handler();

	// Verifico que el largo no supere el largo de la cadena:
	uint16_t realSize = (uint16_t) strlen((char *) pstring);
	size = (size>realSize) ? realSize : size;

	// Verifico que el largo no supere el máximo configurado:
	size = (size<maxSize) ? size : maxSize;

	// Envío!!!
	HAL_UART_Transmit(&UartHandle, (uint8_t *) pstring, size, uartTimeOut);
}

/*******************************************************************************
  * @brief  Recibe por UART una cantidad definida de caracteres
  * @param  Puntero a buffer donde gurdar datos
  * @param  Cantidad de datos a recibir
  * @retval None
  */
bool_t uartReceiveStringSize(uint8_t * pstring, uint16_t size) {
	// ¿El puntero es válido?
	if (pstring == NULL) Error_Handler();

	// Verifico que no se supere el máximo:
	size = (size>maxSize) ? maxSize : size;

	// Recibo...
	HAL_StatusTypeDef Resultado;
	Resultado = HAL_UART_Receive(&UartHandle, (uint8_t *)pstring, (uint16_t) size, uartTimeOut);
	if (Resultado == HAL_OK) return true;
	return false;
}

void uartClearBuffer() {
	uint8_t Basura[5];
    while (uartReceiveStringSize(Basura, 1)) {}; // Vacío el buffer de lectura, si tiene algo.

}


/***************************************************************END OF FILE****/
