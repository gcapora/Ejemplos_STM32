/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Ejemplo de ADC con Analog Watch Dog (AWD)
  *                   (No veo diferencia entre continuo y único)
  ******************************************************************************
  *
  * El ejemplo se desarrolló en base a:
  * https://deepbluembedded.com/stm32-adc-read-example-dma-interrupt-polling/
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uOSAL.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DELTA      100
#define MUESTRAS   2000
#define SOBREMUESTRAS 400
#define COLUMNAS   25
#define NIVEL      1200
#define HISTERESIS 20
#define N_CAPTURAS 16


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t ADC_CONVERTIDO [ MUESTRAS ] = {0}; //0;
uint32_t ADC_SUMA       [ MUESTRAS ] = {0};
uint8_t  ADC_CANTIDAD   [ MUESTRAS ] = {0};
float    PROMEDIO = 0;
float    PROMEDIO_TOTAL = 0;
volatile uint32_t Num_Conversiones = 0;
bool     Escribimos = false;
bool     Nueva_Lectura = false;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void EscribirDatos(uint32_t);
void PromediarConDisparo(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  //-----------------------------------------------------------------------------------------------
  uint32_t Tiempo_ms = 0;
  uartInit();

  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14; // ;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  uEscribirTexto ("\n\r-----------------------------------------------------------------------------");
  uEscribirTexto ("\n\rADC disparado por trigger con DMA (marzo 2024)");
  uEscribirTexto ("\n\r-----------------------------------------------------------------------------\n\r");

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim3); // Start Timer3 (Trigger Source For ADC1)
  //HAL_ADC_Start_IT(&hadc1);   // Start ADC Conversion

  Tiempo_ms = uOSALmiliseg ();
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) ADC_CONVERTIDO, MUESTRAS);  // Inicia la conversión continua del ADC



  //-----------------------------------------------------------------------------------------------
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	//---------------------------------------------------------------------------------------------
	//TIM2->CCR1 = (AD_RES<<4); // Update The PWM Duty Cycle With Latest ADC Conversion Result


	if (Nueva_Lectura) {
		Nueva_Lectura = false;
		//PROMEDIO = 0;
		for (int j=0; j<MUESTRAS; j++) {
					PROMEDIO += (float) ADC_CONVERTIDO[j];
		}
		PROMEDIO = PROMEDIO / MUESTRAS;
		PROMEDIO_TOTAL += PROMEDIO / N_CAPTURAS;
		TIM2->CCR1 = ( ((uint32_t) PROMEDIO & 0xFFFF) <<4 );
		PromediarConDisparo();
		//EscribirDatos(Num_Conversiones);

		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // led rojo que titila
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // led rojo que titila

		//HAL_Delay(10);
		if ( Num_Conversiones <  N_CAPTURAS ) HAL_ADC_Start_DMA(&hadc1, (uint32_t *) ADC_CONVERTIDO, MUESTRAS);
		if ( Num_Conversiones == N_CAPTURAS ) {
			Tiempo_ms = uOSALmiliseg () - Tiempo_ms;
			EscribirDatos(Num_Conversiones);
		    uEscribirTextoEnteroSS ("Tardamos ", Tiempo_ms);
		}

	}
  }
  //-----------------------------------------------------------------------------------------------
  /* USER CODE END 3 */
}

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

/* USER CODE BEGIN 4 */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	Num_Conversiones++;
	Nueva_Lectura = true;

	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
	//AD_RES = HAL_ADC_GetValue(&hadc1);
	// for (int i=0; i< 0x6FF; i++) { 		}
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	static uint32_t contando = 0;
	contando++;
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	if ( contando & 0x0700 ) HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

void PromediarConDisparo(void)
{
	bool bajo=true;
	int inicio = 0;
	int final  = MUESTRAS;
	uEscribirTexto (".");
	if (ADC_CONVERTIDO[0] >= NIVEL) bajo = false;

	for (int disparo=0; disparo<MUESTRAS; disparo++) {

		// supondremos por ahora siempre flanco de subida
		if ( bajo && (ADC_CONVERTIDO[disparo] >= NIVEL) ) {

    		if ( disparo > SOBREMUESTRAS ) {
    			// No puedo cargar todas las muestras anteriores
    			inicio = disparo - SOBREMUESTRAS;
    		}

            if ( disparo < SOBREMUESTRAS ) {
            	final = MUESTRAS + disparo - SOBREMUESTRAS;
            }

	    	// hubo disparo!!!
	    	for (int i=inicio; i<final; i++) {
	    		ADC_SUMA     [ SOBREMUESTRAS + i - disparo] = ADC_SUMA [ SOBREMUESTRAS + i - disparo] + ADC_CONVERTIDO[i];
				ADC_CANTIDAD [ SOBREMUESTRAS + i - disparo] ++;
	    	}
	    	disparo = MUESTRAS;
	    }

	    // Esto es por si empezó alto
	    if (ADC_CONVERTIDO[disparo] < (NIVEL-HISTERESIS)) bajo = true;
	}
	uEscribirTexto ("-");
}

void EscribirDatos(uint32_t Actual)
{
	//static uint32_t Anterior = 0;
	uEscribirTextoEnteroSS ("\n\r============================================================\n\rLectura # ", Actual);
	//uEscribirTextoEnteroSS (" (Frecuencia de Muestreo = ", (Actual-Anterior) * MUESTRAS / DELTA);
	//uEscribirTexto (" kHz)\n\r");
	uEscribirTexto (" \n\r");

	int j=0;
	for (int i=0; i<MUESTRAS; i++) {
		if (j>=COLUMNAS) {
			uEscribirTexto ("\n\r");
			j=0;
		}
		uEscribirEnteroSS ( ADC_CONVERTIDO[i] & 0xFFFF );
		uEscribirTexto ("; ");
		j++;
	}
	uEscribirTexto (" \n\r------------------------------------------------------------\n\r");

	j=0;
	for (int i=0; i<MUESTRAS; i++) {

		if (j>=COLUMNAS) {
			uEscribirTexto ("\n\r");
			j=0;
		}

		if (i==SOBREMUESTRAS) uEscribirTexto ("---disparo---\n\r");


		if (ADC_CANTIDAD[i] > 0) {
			uEscribirEnteroSS ( ADC_SUMA[i] / ADC_CANTIDAD[i] );
			uEscribirTexto ("; ");
		} else {
			uEscribirTexto ("-; ");
		}
		j++;
	}
	uEscribirTexto (" \n\r------------------------------------------------------------\n\r");

	j=0;
	for (int i=0; i<MUESTRAS; i++) {
		if (j>=COLUMNAS) {
			uEscribirTexto ("\n\r");
			j=0;
		}
		uEscribirEnteroSS ( ADC_CANTIDAD[i] );
		uEscribirTexto ("; ");
		j++;
	}

	uEscribirTextoEnteroSS ("\n\rPromedio ultimo = ", PROMEDIO);
	uEscribirTextoEnteroSS ("\n\rPromedio total  = ", PROMEDIO_TOTAL);
	uEscribirTexto (" \n\r------------------------------------------------------------\n\r");
	//Anterior = Actual;
}


/* USER CODE END 4 */

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
