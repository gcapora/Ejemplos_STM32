/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Ejemplo de uso de uCapturadora
  ******************************************************************************
  *
  * El módulo uCapturadora utiliza, a su vez, el módulo uHALadc.h.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Private macro -------------------------------------------------------------*/

#define FREC_TESTIGO 4000.0

/* Private variables ---------------------------------------------------------*/

capturadora_config_s CAPTU_CONFIG    = {0};
entrada_config_s     ENTRADA_CONFIG  = {0};
senial_s * 				P_Senial_E1 = NULL;
senial_s * 				P_Senial_E2 = NULL;
uint8_t    				TareaNro = 0;
uint8_t					Caracter[2] = {0};
uint32_t 				Tiempo_us = 0;

/* Variables importadas -------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

void SystemClock_Config(void);

/**************************************************************************************************
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration --------------------------------------------------------------------------*/

  HAL_Init();
  SystemClock_Config();

  /* Inicializa módulos y objetos ---------------------------------------------------------------*/

  uHALinicializar ();
  uHALmapInicializar ( UHAL_MAP_PE5 );
  uCapturadoraInicializar ();

  /*---------------------------------------------------------------------------------------------*/

  uEscribirTxt ("\n\r\n\r");
  uEscribirTxt ("============================================================\n\r");
  uEscribirTxt ("ADC dual con DMA (mayo 2024)\n\r");
  uEscribirTxt ("============================================================\n\r");

  // Señal cuadrada testigo -----------------------------------------------------------------------

  uEscribirTxtUint	( "Frecuencia de senial cuadrada\t= ",
		  	  	  	  	  	  (uint32_t) round( uHALmapConfigurarFrecuencia ( UHAL_MAP_PE5, FREC_TESTIGO ) ) );
  uEscribirTxt			( " Hz. \n\r");
  uHALmapEncender    ( UHAL_MAP_PE5 );
  Tiempo_us = uMicrosegundos();
  do {} while (uMicrosegundos() - Tiempo_us < 5e5 );
  // Este retardo sirve para que la señal cuadrada inicie.

  // Capturadora ----------------------------------------------------------------------------------

  uCapturadoraObtener	 ( &CAPTU_CONFIG );
  CAPTU_CONFIG.EscalaHorizontal = 1.5/FREC_TESTIGO;
  CAPTU_CONFIG.ModoCaptura      = CAPTURA_PROMEDIADA_16;
  uCapturadoraConfigurar ( &CAPTU_CONFIG );
  P_Senial_E1 = uCapturadoraSenialObtener ( ENTRADA_1 );
  P_Senial_E2 = uCapturadoraSenialObtener ( ENTRADA_2 );

  // Configuramos entradas ------------------------------------------------------------------------

  uCapturadoraEntradaObtener    ( ENTRADA_1, &ENTRADA_CONFIG );
  ENTRADA_CONFIG.EscalaVertical = 3;
  ENTRADA_CONFIG.NivelDisparo   = 1.5;
  ENTRADA_CONFIG.FlancoDisparo  = BAJADA;
  uCapturadoraEntradaConfigurar ( ENTRADA_1, &ENTRADA_CONFIG );
  ENTRADA_CONFIG.EscalaVertical = 3;
  ENTRADA_CONFIG.NivelDisparo   = 0.5;
  ENTRADA_CONFIG.FlancoDisparo  = SUBIDA;
  uCapturadoraEntradaConfigurar ( ENTRADA_2, &ENTRADA_CONFIG );

  // Iniciamos captura 1 --------------------------------------------------------------------------

  uEscribirTxt ("============================================================\n\r");
  TareaNro = 1;
  uEscribirTxt ( "Iniciamos captura #1...\n\r" );
  uCapturadoraEntradaEncender ( ENTRADA_1 );
  uCapturadoraEntradaEncender ( ENTRADA_2 );
  uCapturadoraIniciar ();

  //-----------------------------------------------------------------------------------------------
  while (1)
  {

	  // Verificamos si hay una nueva señal cargada...
	  if ( uCapturadoraSenialCargada() ){
		  ImprimirSenial32_main();
		  uEscribirTxt ("============================================================\n\r");
		  Tiempo_us = uMicrosegundos();
		  TareaNro++;
	  }

	  // Pedimos una nueva captura luego de un tiempo...
	  if (uMicrosegundos () - Tiempo_us > 2e6 && 2==TareaNro)  {
		  Tiempo_us = uMicrosegundos();
		  uEscribirTxt("Iniciamos captura #2.\n\r");

		  uCapturadoraObtener ( &CAPTU_CONFIG );
		  //CAPTU_CONFIG.EscalaHorizontal = 1.0/100;
		  CAPTU_CONFIG.OrigenDisparo    = ENTRADA_2;
		  CAPTU_CONFIG.ModoCaptura      = 0;
		  uCapturadoraConfigurar ( &CAPTU_CONFIG );

		  uCapturadoraEntradaObtener    ( ENTRADA_1, &ENTRADA_CONFIG );
		  ENTRADA_CONFIG.EscalaVertical = 3;
		  ENTRADA_CONFIG.NivelDisparo   = 2;
		  ENTRADA_CONFIG.FlancoDisparo  = SUBIDA;
		  uCapturadoraEntradaConfigurar ( ENTRADA_1, &ENTRADA_CONFIG );

		  uCapturadoraEntradaObtener    ( ENTRADA_2, &ENTRADA_CONFIG );
		  ENTRADA_CONFIG.EscalaVertical = 6;
		  ENTRADA_CONFIG.NivelDisparo   = 2;
		  ENTRADA_CONFIG.FlancoDisparo  = BAJADA;
		  uCapturadoraEntradaConfigurar ( ENTRADA_2, &ENTRADA_CONFIG );

		  uCapturadoraIniciar ();
	  }

	  if (TareaNro > 2 && uLeerChar ( &Caracter[0], 1 )) {
		  uEscribirTxt ( (char *) Caracter );
	  }

  } // -----> fin de loop
}	 // -----> fin de main

/*
void ImprimirSenial32_main (void)
{
	// Variables locales:
	uint32_t i, Muestra, MUESTRA_ENTRADA_1, MUESTRA_ENTRADA_2, Disparo;

	// Precondiciones
	if ( NULL == P_Senial_E1 || NULL == P_Senial_E2 ) uHuboErrorTxt ("en Imprimir... ppal.");

	// Asignaciones iniciales
	Disparo = P_Senial_E1->ReferenciaT0;

	// Escribimos última muestra:
	uEscribirTxt ("Senial cargada:");
	uEscribirTxt ("\n\rENT_1 \tENT_2\n\r");

	for (i=0; i<U_LARGO_CAPTURA; i++) {

		Muestra = P_Senial_E1->Muestras_p[i];
		MUESTRA_ENTRADA_1 = ( Muestra & MASCARA_DERECHA16   );
		MUESTRA_ENTRADA_2 = ( Muestra & MASCARA_IZQUIERDA16 ) >> 16;

		if ( (i==Disparo) && (i>0) ) uEscribirTxt ("---> Disparo <---\n\r");

		uEscribirUint ( MUESTRA_ENTRADA_1 );	// Dato de ENTRADA 1
		uEscribirTxt  ( "\t" );		// Tabulación
		uEscribirUint ( MUESTRA_ENTRADA_2 );	// Dato de ENTRADA 2
		//uEscribirTxt  ( "\t" );		// Tabulación
		//uEscribirUint ( CantidadProcesadas12[i] );	// Dato de ENTRADA 2
		uEscribirTxt  ( "\n\r" );
	}

	uEscribirTxtUint ( "Capturas promediadas \t= ", CapturasObjetivo() - Capturadora.CapturasRestantes );
	uEscribirTxt     ( "\n\r" );
	uEscribirTxtUint ( "Tiempo de captura \t= ", Capturadora.TiempoCaptura );
	uEscribirTxt     ( " ms\n\r" );
	uEscribirTxtUint ( "Nivel (12B) \t\t= ", Capturadora.NivelDisparo );
	uEscribirTxt     ( "\n\r" );
	uEscribirTxtUint ( "Escala Entrada 1\t= ", (uint32_t) (EntradaAdmin[0].Config.EscalaVertical*10) );
	uEscribirTxt     ( "\n\r" );
	uEscribirTxtUint ( "Escala Entrada 2\t= ", (uint32_t) (EntradaAdmin[1].Config.EscalaVertical*10) );
	uEscribirTxt     ( "\n\r" );
}
*/

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	uLedEncender ( UOSAL_PIN_LED_ROJO_INCORPORADO );
	uEscribirTxt ("Error con ADC...\n\r");
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
