/*******************************************************************************
* @file    main.c
* @author  Guillermo Caporaletti
* @brief   Práctica 5 - Punto 2
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define y const ----------------------------------------------------*/
#define TIEMPO_ENCENDIDO_1 100
#define TIEMPO_ENCENDIDO_2 500

/* Private variables ---------------------------------------------------------*/
delay_t parpadeoLed;						// Estructura para parpadeo de LED2

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CambiarTiempoParpadeoLed(void);

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Inicialización para el manejo de errores    */
  Error_Handler_Init();

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 180 MHz */
  SystemClock_Config();

  // Comienzo conexión
  if (!uartInit()) Error_Handler();

  // Inicializo MEF antirrebote
  debounceFSM_init();

  // Inicializo parpadeo de LED2
  BSP_LED_Init(LED2);
  delayInit( &parpadeoLed, TIEMPO_ENCENDIDO_1);
  CambiarTiempoParpadeoLed();

  /* Infinite loop */
  while (1)
  {
	  // Reviso el boton de usuario...
	  debounceFSM_update();

	  // Además parpadeo LED2
	  if (delayRead( &parpadeoLed )) {
		  BSP_LED_Toggle(LED2);
	  }

	  // Si hay flanco descendiente cambio parpadeo
	  if (readKey()) {
		  CambiarTiempoParpadeoLed();
		  uartSendString((uint8_t *) "Boton presionado!\n\r");
	  }

	  // Si hay flanco ascendiente mando texto
	  if (readKeyUp()) {
		  uartSendString((uint8_t *) "Boton soltado!\n\r");
	  }
  }
} /* Fin de función main ******************************************************/

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/*******************************************************************************
  * @brief  Cambia el tiempo de parpadeo del LED2
  * @param  None
  * @retval None
  */
static void CambiarTiempoParpadeoLed(void) {
	// Cambio el tiempo de encendido
	static tick_t Encendido = TIEMPO_ENCENDIDO_1;
	delayWrite( &parpadeoLed, Encendido );

	switch (Encendido) {
		case TIEMPO_ENCENDIDO_1:
			Encendido = TIEMPO_ENCENDIDO_2;
		break;

		case TIEMPO_ENCENDIDO_2:
			Encendido = TIEMPO_ENCENDIDO_1;
		break;

		default:
			Encendido = TIEMPO_ENCENDIDO_1;
		break;
	}
}

/***************************************************************END OF FILE****/
