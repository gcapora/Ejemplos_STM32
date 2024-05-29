/**************************************************************************************************
 * Archivo: uOSALconfig.h
 * Breve:	Configuración pública de uOSAL.
 * Fecha:	Creado en agosto 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UOSAL_CONFIG_H_
#define ISPEL_UOSAL_CONFIG_H_

/****** Librerías (includes) *********************************************************************/

#include "stm32f4xx_hal.h"

/****** Definiciones públicas (macros) ***********************************************************/

// Reloj ------------------------------------------------------------------------------------------
#define UOSAL_FRECUENCIA_RELOJ				144000000.0

// Contador de microsegundos ----------------------------------------------------------------------
#define UOSAL_MICROSEGUNDOS_TEMPO         TIM5 //UHAL_MICROSEGUNDOS_TEMPO  // Tempo para contar microsegundos

// Leds incorporados ------------------------------------------------------------------------------
#define UOSAL_PUERTO_LEDS_INCORPORADOS    GPIOB
#define UOSAL_PIN_LED_ROJO_INCORPORADO    GPIO_PIN_14
#define UOSAL_PIN_LED_VERDE_INCORPORADO   GPIO_PIN_0
#define UOSAL_PIN_LED_AZUL_INCORPORADO    GPIO_PIN_7
#define UOSAL_PIN_LEDS_INCORPORADOS_TODOS ( UOSAL_PIN_LED_ROJO_INCORPORADO | UOSAL_PIN_LED_VERDE_INCORPORADO | UOSAL_PIN_LED_AZUL_INCORPORADO )

// Puerto UART ------------------------------------------------------------------------------------
/* Constantes de usuario para comunicación */
#define UOSAL_UART_BAUDIOS                460800  // Predeterminado es 115200
#define UOSAL_UART_LARGO_MAXIMO           256
#define UOSAL_UART_TIEMPO_ESPERA          100    // [ms]
/* Definition for USARTx clock resources */
#define USARTx                            USART3
#define USARTx_CLK_ENABLE()               __HAL_RCC_USART3_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_FORCE_RESET()              __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET()            __HAL_RCC_USART3_RELEASE_RESET()
/* Definition for USARTx Pins */
#define USARTx_TX_PIN                     GPIO_PIN_8
#define USARTx_TX_GPIO_PORT               GPIOD
#define USARTx_TX_AF                      GPIO_AF7_USART3
#define USARTx_RX_PIN                     GPIO_PIN_9
#define USARTx_RX_GPIO_PORT               GPIOD
#define USARTx_RX_AF                      GPIO_AF7_USART3

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/


/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/


/*************************************************************************************************/
#endif /* ISPEL_UOSAL_CONFIG_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
