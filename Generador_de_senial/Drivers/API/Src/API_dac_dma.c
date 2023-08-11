/*******************************************************************************
  * @file		main.c
  * @brief      Generador de señales con entrada por UART
  * @author		Guillermo F. Caporaletti <gfcaporaletti@undav.edu.ar>
  * @detail		Recibe una entrada por UART y la saca por DAC.
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "API_dac_dma.h"

/* Private define ------------------------------------------------------------*/
#define N_MUESTRAS			105		// Muestras en un período de señal

/* Private variables HAL ------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac2;
TIM_HandleTypeDef htim2;

/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM2_Init(void);

/* Funciones públicas --------------------------------------------------------*/

/**
  * @brief Inicializa DMA, DAC2 y Timer 2.
  *        Inicia el conteo de Timer 2
  * @param None
  * @retval None
  */
void Inicializar_DAC_DMA(void){
	MX_DMA_Init();
	MX_DAC_Init();
	MX_TIM2_Init();
    HAL_TIM_Base_Start(&htim2);
}

/**
  * @brief Comienza a enviar Datos a salida por DAC
  * @param Puntero a Datos y cantidad de datos Num_Datos
  * @retval None
  */
void Comenzar_DAC_DMA(uint32_t * Datos, uint32_t Num_Datos) {
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, Datos, Num_Datos, DAC_ALIGN_12B_R);
}

/**
  * @brief Para el envío de Datos a salida por DAC
  * @param None
  * @retval None
  */
void Parar_DAC_DMA(void) {
	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_2);
}

/* Funciones privadas --------------------------------------------------------*/

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{
  DAC_ChannelConfTypeDef sConfig = {0};

  /** DAC Initialization   */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config   */
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 7;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

