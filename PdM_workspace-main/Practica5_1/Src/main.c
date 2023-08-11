/*******************************************************************************
* @file    main.c
* @author
* @brief   Módulo principal para probar la librería API_uart
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/
char bufferLectura[bufferSize];

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Inicializa la librería de manejo de errores */
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

  /* Initialize BSP Led for LED2 and LED3*/
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

  /* Inicializa conexión por UART */
  if (uartInit() != true) Error_Handler();

  // Vacío la cadena buffer.
  for (size_t i=0; i<bufferSize;i++) {
 	  bufferLectura[i] = '\0';
  }

  /* Mensajes de prueba */
  uartSendString((uint8_t *) "---------------------------------\n\r");
  uartSendString((uint8_t *) "Utilizando funcion uartSendString\n\r");

  for (uint16_t i=0; i<20; i++) {
	  // Pruebo enviar partes de una misma cadena...
	  uartSendStringSize((uint8_t *) "Hola, que tal?", i);
	  uartSendStringSize((uint8_t *) "\n\r",2);
  }

  uartSendString((uint8_t *) "Test exitoso!!!\n\r");
  uartSendString((uint8_t *) "---------------------------------\n\r");

  /* Infinite loop */
  while (1)
  {
	  HAL_Delay(100);
	  BSP_LED_Toggle(LED2);

	  uartReceiveStringSize((uint8_t *) bufferLectura, bufferSize);
	  if (strlen(bufferLectura) > 0) {
		  // Recibí algo!!!
		  // Devuelvo:
		  uartSendStringSize((uint8_t *) bufferLectura,bufferSize);
		  uartSendString((uint8_t *) "\n\r");
		  // Reseteo buffer:
		  for (size_t i=0; i<bufferSize;i++) {
			  bufferLectura[i] = '\0';
		  }
	  }
  }
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
