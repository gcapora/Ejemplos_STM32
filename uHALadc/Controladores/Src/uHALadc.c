/**************************************************************************************************
 * Archivo: uHALadc.c
 * Breve:
 * Fecha:	Creado en abril 2024
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "stm32f4xx_hal.h"
#include "uHAL.h"
#include "uHALadc.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos privados *************************************************************/

ADC_HandleTypeDef h_adc1;
ADC_HandleTypeDef h_adc2;
DMA_HandleTypeDef h_dma_adc1;
DMA_HandleTypeDef h_dma_adc2;
TIM_HandleTypeDef tempo_dma_s;

/****** Importación de datos públicos ************************************************************/


/****** Declaración de funciones privadas ********************************************************/

bool TEMPO_DMA_INICIALIZAR (void);

/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
  * @brief   Configura una ADC específico
  * @param
  * @retval
  */
bool uHALadcInicializar ( adc_id_e ID )
{
  bool control = false;
  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  // Verificamos parámetro:

  if ( ID >= UHAL_CANTIDAD_ADCS && ID != UHAL_ADC_TODOS ) return false;
  if ( ID == UHAL_ADC_2 ) return false;

  // Posible(s) ADC's para inicializar

  if ( ID == UHAL_ADC_1 || ID == UHAL_ADC_TODOS ) {
	/***** ADC Principal *****---------------------------------------------------------------------
	 * Configurado en modo DMA dual regular disparado con temporizador */

	control = true;

	/* Inicializar temporizador del DMA */
	if ( TEMPO_DMA_INICIALIZAR () == false ) control = false;

    /* Activar reloj DMA */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA2_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	/* DMA2_Stream2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	/* Configure the global features of the ADC 1 -------------------------------------------------
	 * (Clock, Resolution, Data Alignment and number of conversion) */
	h_adc1.Instance = ADC1;
	h_adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	h_adc1.Init.Resolution = ADC_RESOLUTION_12B;
	h_adc1.Init.ScanConvMode = DISABLE;
	h_adc1.Init.ContinuousConvMode = DISABLE;
	h_adc1.Init.DiscontinuousConvMode = DISABLE;
	h_adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	h_adc1.Init.ExternalTrigConv = UHAL_ADC1_ORIGEN;   // Definido en uHALconfig.h
	h_adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	h_adc1.Init.NbrOfConversion = 1;
	h_adc1.Init.DMAContinuousRequests = ENABLE;
	h_adc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	if (HAL_ADC_Init(&h_adc1) != HAL_OK) control = false;

	/* Configure the ADC multi-mode */
	multimode.Mode = ADC_DUALMODE_REGSIMULT;
	multimode.DMAAccessMode = ADC_DMAACCESSMODE_2;
	multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
	if (HAL_ADCEx_MultiModeConfigChannel(&h_adc1, &multimode) != HAL_OK) control = false;

	/* Configure for the selected ADC regular channel its corresponding rank
	 * in the sequencer and its sample time. */
	sConfig.Channel = UHAL_ADC1_CANAL0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&h_adc1, &sConfig) != HAL_OK) control = false;

	/* Configure the global features of the ADC 2 ------------------------------------------------
	 * (Clock, Resolution, Data Alignment and number of conversion)
	 */
	h_adc2.Instance = ADC2;
	h_adc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	h_adc2.Init.Resolution = ADC_RESOLUTION_12B;
	h_adc2.Init.ScanConvMode = DISABLE;
	h_adc2.Init.ContinuousConvMode = DISABLE;
	h_adc2.Init.DiscontinuousConvMode = DISABLE;
	h_adc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	h_adc2.Init.NbrOfConversion = 1;
	h_adc2.Init.DMAContinuousRequests = ENABLE;
	h_adc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	if (HAL_ADC_Init(&h_adc2) != HAL_OK) control = false;

	/* Configure for the selected ADC regular channel its corresponding rank
	 * in the sequencer and its sample time. */
	sConfig.Channel = UHAL_ADC2_CANAL0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&h_adc2, &sConfig) != HAL_OK) control = false;

  }
  return control;
}

bool uHALadcDesInicializar ( adc_id_e ID)
{
	  return true;  // falta implementar
}

bool uHALadcConfigurar ( adc_id_e ID, adc_config_s * CONFIG )
{
	  return true;
}

bool uHALadcComenzarLectura ( adc_id_e ID , uint32_t * VECTOR, uint32_t LARGO)
{
   bool control = false;
   //HAL_ADC_Start_DMA(&hadc1, (uint32_t *) ADC_CONVERTIDO, MUESTRAS);  <-- Esto sería para DMA mono.

   if ( ID==UHAL_ADC_1 || ID==UHAL_ADC_TODOS ) {
	   control = true;
	   HAL_ADC_Start (&h_adc2);
	   HAL_ADCEx_MultiModeStart_DMA (&h_adc1, VECTOR, LARGO);
	   HAL_TIM_Base_Start (&tempo_dma_s);
   }

   return control;
}

__weak void uHALadcLecturaCompletada ( adc_id_e ID )
{
	// Esta función debe redefinirse en la aplicación para ejecutar las tareas necesarias.
	UNUSED( ID );
}

__weak void uHALadcMediaLecturaCompletada ( adc_id_e ID )
{
	// Esta función debe redefinirse en la aplicación para ejecutar las tareas necesarias.
	UNUSED( ID );
}

/****** Definición de funciones privadas *********************************************************/

void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef* hadc)
{
	// Paramos base de tiempo de muestreo (aunque no es imprescindible):
	HAL_TIM_Base_Stop(&tempo_dma_s);

	// Llamamos a fn de la aplicación con ADC identificado:
	if ( hadc->Instance == ADC1 ) {
		uHALadcLecturaCompletada ( UHAL_ADC_1 );
	}
}

void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef* hadc)
{
	// Llamamos a fn de la aplicación con ADC identificado:
	if ( hadc->Instance == ADC1 ) {
		uHALadcMediaLecturaCompletada ( UHAL_ADC_1 );
	}
}

bool TEMPO_DMA_INICIALIZAR (void)
{
  bool control = true;
  TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  tempo_dma_s.Instance = UHAL_ADC1_TEMPO;   // Definido en uHALconfig.h
  tempo_dma_s.Init.Prescaler = 5;
  tempo_dma_s.Init.CounterMode = TIM_COUNTERMODE_UP;
  tempo_dma_s.Init.Period = 119;
  tempo_dma_s.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tempo_dma_s.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&tempo_dma_s) != HAL_OK) control = false;

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tempo_dma_s, &sClockSourceConfig) != HAL_OK) control = false;

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tempo_dma_s, &sMasterConfig) != HAL_OK) control = false;

  return control;
}

/****************************************************************** FIN DE ARCHIVO ***************/
