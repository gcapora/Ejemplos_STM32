/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Ejemplo de ADC dual con DMA
  ******************************************************************************
  *
  * El ejemplo se desarrolló en base a:
  * https://deepbluembedded.com/stm32-adc-read-example-dma-interrupt-polling/
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "uOSAL.h"
#include "uHAL.h"
#include "main.h"
//#include "adc.h"
//#include "dma.h"
//#include "tim.h"
//#include "gpio.h"

/* Private macro -------------------------------------------------------------*/

#define DELTA       100
#define MUESTRAS    200
#define PREMUESTRAS 40
#define NIVEL       1500
#define HISTERESIS  20
#define N_CAPTURAS  16

/* Private variables ---------------------------------------------------------*/

const adc_id_e ADC_12 = UHAL_ADC_1;  // Identificador para ADC_1 y ADC_2
uint32_t ADC_CONVERTIDO [ MUESTRAS ] = {0};
uint32_t ADC_SUMA       [ MUESTRAS ] = {0};
uint8_t  ADC_CANTIDAD   [ MUESTRAS ] = {0};
float    PROMEDIO = 0;
float    PROMEDIO_TOTAL = 0;
volatile uint32_t Num_Conversiones = 0;
bool     Escribimos = false;
bool     Nueva_Lectura = false;
uint32_t Tiempo_us = 0;

/* Variables importadas -------------------------------------------------------*/

// extern ADC_HandleTypeDef h_adc1;
// extern ADC_HandleTypeDef h_adc2;
// extern TIM_HandleTypeDef tempo_dma_s;
// extern TIM_HandleTypeDef Tempo_admin[UHAL_CANTIDAD_MAP];

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
void EscribirDatos(uint32_t);
void PromediarConDisparo(void);

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  uHALinicializar    ();
  uHALadcInicializar ( ADC_12 );
  //uHALmapInicializar ( UHAL_MAP_PE5 );
  uHALmapInicializar ( UHAL_MAP_TODOS );
  //MX_GPIO_Init();
  //MX_DMA_Init();
  //MX_ADC1_Init();
  //MX_TIM3_Init();
  //MX_ADC2_Init();
  //MX_TIM9_Init();

  //-----------------------------------------------------------------------------

  uLedEncender ( UOSAL_PIN_LED_AZUL_INCORPORADO );
  uEscribirTexto ("\n\r\n\r");
  uEscribirTexto ("============================================================\n\r");
  uEscribirTexto ("ADC dual con DMA (abril 2024)\n\r");
  uEscribirTexto ("============================================================\n\r");

  //uHALmapEncender ( UHAL_MAP_PE5 );				// Inicio señal cuadrada
  uHALmapEncender ( UHAL_MAP_TODOS );
  Tiempo_us = uOSALmicrosegundos();         	// Leo inicio de conteo en us.
  uHALadcComenzarLectura ( ADC_12, 				// Lanzamos primer muestreo en ADC 1 y 2
		                     ADC_CONVERTIDO,	// Vector donde almaceno lo muestreado
									MUESTRAS );			// Largo del vector

  //-----------------------------------------------------------------------------------------------
  while (1)
  {

	  if ( (uOSALmicrosegundos () - Tiempo_us > 500000) && (Num_Conversiones <  N_CAPTURAS) ) {
		  // Si pasó medio segundo, muestro qué tengo:
		  uEscribirTexto ("Sobretiempo... \n\r");
		  Nueva_Lectura = true;
		  Num_Conversiones = N_CAPTURAS;
	  }

	  if (Nueva_Lectura ) {
		  Nueva_Lectura = false;

		  // Promediamos muestra a muestra
		  PromediarConDisparo();

		  // ¿Lanzamos nuevo muestreo?
		  if ( Num_Conversiones <  N_CAPTURAS ) {
			  uHALadcComenzarLectura ( ADC_12, ADC_CONVERTIDO, MUESTRAS );
		  }

		  // ¿Terminamos muestreo?
		  if ( Num_Conversiones == N_CAPTURAS ) {
			  Tiempo_us = uOSALmicrosegundos() - Tiempo_us;
			  EscribirDatos (Num_Conversiones);
			  uEscribirTextoEnteroSS ("Tardamos ", Tiempo_us);
			  uEscribirTexto (" us. \n\r");
			  uLedApagar ( UOSAL_PIN_LED_AZUL_INCORPORADO );

			  uEscribirTextoEnteroSS ("Dejamos PAM en ",
					                    (uint32_t) uHALmapConfigurarFrecuencia ( UHAL_MAP_PE5, 10000 ));
			  uEscribirTexto (" Hz. \n\r");
		  }
	  }
  } // -----> fin de loop
}	 // -----> fin de main

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
	uEscribirTexto ("Error con ADC...\n\r");
}

void uHALadcLecturaCompletada ( adc_id_e ID )
{
	Num_Conversiones++;
	Nueva_Lectura = true;
	uLedEncender ( UOSAL_PIN_LED_VERDE_INCORPORADO );
}

void PromediarConDisparo(void)
{
	bool bajo=true;
	int inicio = 0;
	int final  = MUESTRAS;

	// Evalúo si la señal inicia por encima del nivel buscado:
	// (aplico máscada de ADC1)
	if ( ( ADC_CONVERTIDO[0]&0xFFFF ) >= NIVEL) bajo = false;

	for (int disparo=0; disparo<MUESTRAS; disparo++) {

		// Supondremos por ahora siempre flanco de subida
		if ( bajo && ( (ADC_CONVERTIDO[disparo]&0xFFFF) >= NIVEL) ) {

    		if ( disparo > PREMUESTRAS ) {
    			// No puedo cargar todas las muestras anteriores
    			inicio = disparo - PREMUESTRAS;
    		}

            if ( disparo < PREMUESTRAS ) {
            	final = MUESTRAS + disparo - PREMUESTRAS;
            }

	    	// Hubo disparo!!! :-)
	    	for (int i=inicio; i<final; i++) {
	    		// Acá se supone que suma por separado ambos ADC. Muestras a promediar deben ser igual o menor a 16.
	    		ADC_SUMA     [ PREMUESTRAS + i - disparo] = ADC_SUMA [ PREMUESTRAS + i - disparo] + ADC_CONVERTIDO[i];
	    		ADC_CANTIDAD [ PREMUESTRAS + i - disparo] ++;
	    	}
	    	disparo = MUESTRAS;
	    }

	    // Esto es por si empezó alto
	    if ( (ADC_CONVERTIDO[disparo]&0xFFFF) < (NIVEL-HISTERESIS) ) bajo = true;
	}
}

void EscribirDatos(uint32_t Actual)
{
	uint16_t P_ADC1, P_ADC2;
	uint16_t MAXIMO = 0;
	uint16_t MINIMO = 4095;

	// Escribimos última muestra:
	uEscribirTextoEnteroSS ("Lectura # ", Actual);
	uEscribirTexto         ("\n\rADC_1 \tADC_2\n\r");

	for (int i=0; i<MUESTRAS; i++) {

		P_ADC1 = ( ADC_CONVERTIDO [i] & 0x0000FFFF )      ;
		P_ADC2 = ( ADC_CONVERTIDO [i] & 0xFFFF0000 ) >> 16;

		uEscribirEnteroSS ( P_ADC1 );   // Dato de ADC1
		uEscribirTexto 	  ( ";\t");     // Tabulación
		uEscribirEnteroSS ( P_ADC2 );   // Dato ADC2
		uEscribirTexto    ("\n\r");

		MAXIMO = P_ADC1 > MAXIMO ? P_ADC1 : MAXIMO;
		MINIMO = P_ADC1 < MINIMO ? P_ADC1 : MINIMO;

	}
	uEscribirTextoEnteroSS ("\n\rMaximo ultimo = ", MAXIMO);
	uEscribirTextoEnteroSS ("\n\rMinimo ultimo = ", MINIMO);
	uEscribirTexto 		   ("\n\r------------------------------------------------------------------\n\r");

	// Escribimos promedio:
	uEscribirTexto         ("Promedio");
	uEscribirTexto         ("\n\rADC_1 \tADC_2 \tCantidad\n\r");

	MAXIMO = 0;
	MINIMO = 4095;

	for (int i=0; i<MUESTRAS; i++) {

		if (i==PREMUESTRAS) uEscribirTexto ("Disparo...\n\r");

		P_ADC1 = (( ADC_SUMA [i] & 0x0000FFFF )      ) / ADC_CANTIDAD [i];
		P_ADC2 = (( ADC_SUMA [i] & 0xFFFF0000 ) >> 16) / ADC_CANTIDAD [i];

		uEscribirEnteroSS ( P_ADC1 );
		uEscribirTexto 	  ( ";\t");
		uEscribirEnteroSS ( P_ADC2 );
		uEscribirTexto 	  ( ";\t");
		uEscribirEnteroSS ( ADC_CANTIDAD [i] );
		uEscribirTexto    ( "\n\r");

		MAXIMO = P_ADC1 > MAXIMO ? P_ADC1 : MAXIMO;
		MINIMO = P_ADC1 < MINIMO ? P_ADC1 : MINIMO;

	}
	uEscribirTextoEnteroSS ("\n\rMaximo ultimo = ", MAXIMO);
	uEscribirTextoEnteroSS ("\n\rMinimo ultimo = ", MINIMO);
	uEscribirTexto         (" \n\r------------------------------------------------------------\n\r");

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
