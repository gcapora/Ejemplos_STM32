/**************************************************************************************************
 * Archivo: uHALadc.c
 * Breve:
 * Fecha:	Creado en abril 2024
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include <math.h>
#include "stm32f4xx_hal.h"
#include "uHAL.h"
#include "uHALadc.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define PREESCALER_TEMPO_DMA		6	// No puede modificarse.
#define PERIODO_TEMPO_DMA_0		5	// Valor inicial, que se ajusta con uHALadcConfigurar()
#define PERIODO_TEMPO_DMA_MINIMO	5	//

/****** Definiciones privadas de tipos (private typedef) *****************************************/

typedef enum {
	ADC_NO_INICIALIZADO,
	ADC_INACTIVO,
	ADC_CORRIENDO,
	ADC_SUBORDINADO,
	ADC_ERROR,
} adc_estado_e;

typedef struct {
	volatile adc_estado_e Estado;
	adc_canal_e           Canal;
} adc_admin_s;

/****** Definición de datos privados *************************************************************/

ADC_HandleTypeDef admin_adc1;
ADC_HandleTypeDef admin_adc2;
DMA_HandleTypeDef admin_dma_adc1;
DMA_HandleTypeDef admin_dma_adc2;
TIM_HandleTypeDef tempo_dma_adc12;
adc_admin_s admin_adc [UHAL_CANTIDAD_ADCS] = {0};

/****** Importación de datos públicos ************************************************************/


/****** Declaración de funciones privadas ********************************************************/

bool TEMPO_DMA_INICIALIZAR (void);
void GPIO_ADC_INICIALIZAR  (GPIO_TypeDef * GPIOx, uint32_t PIN);
void PARAR_ADC_12          (void);

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
	  admin_adc1.Instance = ADC1;
	  admin_adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	  admin_adc1.Init.Resolution = ADC_RESOLUTION_12B;
	  admin_adc1.Init.ScanConvMode = DISABLE;
	  admin_adc1.Init.ContinuousConvMode = DISABLE;
	  admin_adc1.Init.DiscontinuousConvMode = DISABLE;
	  admin_adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	  admin_adc1.Init.ExternalTrigConv = UHAL_ADC1_ORIGEN;   // Definido en uHALconfig.h
	  admin_adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  admin_adc1.Init.NbrOfConversion = 1;
	  admin_adc1.Init.DMAContinuousRequests = ENABLE;
	  admin_adc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	  if (HAL_ADC_Init(&admin_adc1) != HAL_OK) control = false;

	  /* Configure the ADC multi-mode */
	  multimode.Mode = ADC_DUALMODE_REGSIMULT;
	  multimode.DMAAccessMode = ADC_DMAACCESSMODE_2;
	  multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
	  if (HAL_ADCEx_MultiModeConfigChannel(&admin_adc1, &multimode) != HAL_OK) control = false;

	  /* Configure for the selected ADC regular channel its corresponding rank
	   * in the sequencer and its sample time. */
	  sConfig.Channel = UHAL_ADC1_CANAL_1;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	  if (HAL_ADC_ConfigChannel(&admin_adc1, &sConfig) != HAL_OK) control = false;

	  /* Configure the global features of the ADC 2 ------------------------------------------------
	   * (Clock, Resolution, Data Alignment and number of conversion)
	   */
	  admin_adc2.Instance = ADC2;
	  admin_adc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	  admin_adc2.Init.Resolution = ADC_RESOLUTION_12B;
	  admin_adc2.Init.ScanConvMode = DISABLE;
	  admin_adc2.Init.ContinuousConvMode = DISABLE;
	  admin_adc2.Init.DiscontinuousConvMode = DISABLE;
	  admin_adc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  admin_adc2.Init.NbrOfConversion = 1;
	  admin_adc2.Init.DMAContinuousRequests = ENABLE;
	  admin_adc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	  if (HAL_ADC_Init(&admin_adc2) != HAL_OK) control = false;

	  /* Configure for the selected ADC regular channel its corresponding rank
	   * in the sequencer and its sample time. */
	  sConfig.Channel = UHAL_ADC2_CANAL_1;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	  if (HAL_ADC_ConfigChannel(&admin_adc2, &sConfig) != HAL_OK) control = false;

	  /* Actualizo estructura admin de ADC's */
	  admin_adc[UHAL_ADC_1].Canal  = U_ADC_CANAL_1;
	  admin_adc[UHAL_ADC_2].Canal  = U_ADC_CANAL_1;
	  admin_adc[UHAL_ADC_1].Estado = ADC_INACTIVO;
	  admin_adc[UHAL_ADC_2].Estado = ADC_SUBORDINADO;

  }
  return control;
}

bool uHALadcDesInicializar ( adc_id_e ID)
{
	  return true;  // falta implementar
}

bool uHALadcConfigurar ( adc_id_e ID, adc_config_s * CONFIG )
{
	// Variables locales
	bool control = false;
	uint32_t periodo, canal;
	ADC_ChannelConfTypeDef sConfig = {0};

	// Control de parámetros
	if ( UHAL_ADC_TODOS == ID ) {
		control = false;
		return control;
	}
	if ( ADC_NO_INICIALIZADO == admin_adc[ID].Estado ) {
		control = false;
		return control;
	}
	if ( CONFIG->FrecuenciaMuestreo > UHAL_ADC_FRECUENCIA_MUESTREO_MAXIMA ) {
		CONFIG->FrecuenciaMuestreo = UHAL_ADC_FRECUENCIA_MUESTREO_MAXIMA;
	}
	if ( CONFIG->FrecuenciaMuestreo < UHAL_ADC_FRECUENCIA_MUESTREO_MINIMA ) {
		CONFIG->FrecuenciaMuestreo = UHAL_ADC_FRECUENCIA_MUESTREO_MINIMA;
	}

	// ADC seleccionado y control

	/*** ¿ADC1 seleccionado? ***----------------------------------------------*/
	if ( UHAL_ADC_1 == ID && admin_adc[ID].Estado == ADC_INACTIVO ){
		/* Sólo se puede configurar en estado ADC_INACTIVO */
		control = true;

		// Frecuencia de muestreo
		periodo   = (uint32_t) round (	UOSAL_FRECUENCIA_RELOJ / 2 /
				      							( PREESCALER_TEMPO_DMA ) /
													CONFIG->FrecuenciaMuestreo );
		if ( periodo < PERIODO_TEMPO_DMA_MINIMO ) periodo = PERIODO_TEMPO_DMA_MINIMO;
		__HAL_TIM_SET_AUTORELOAD( &tempo_dma_adc12, periodo-1 );

		// Canal
		switch ( CONFIG->Canal ) {
			case U_ADC_CANAL_1:
				canal = UHAL_ADC1_CANAL_1;
				break;
			case U_ADC_CANAL_2:
				canal = UHAL_ADC1_CANAL_2;
				break;
			default:
				uHuboErrorTxt("Canal invalido en Configurar ADC.");
		}
		sConfig.Channel = canal;
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		if (HAL_ADC_ConfigChannel(&admin_adc1, &sConfig) != HAL_OK) uHuboErrorTxt("en configuracion de canal de ADC.");

	/*** ¿ADC2 seleccionado? ***----------------------------------------------*/
	} else if ( UHAL_ADC_2 == ID ) {
		if ( admin_adc[ID].Estado == ADC_SUBORDINADO &&
			admin_adc[UHAL_ADC_1].Estado == ADC_INACTIVO ) {
			/* Sólo admitimos estado subordinado y con ADC1 inactivo */
			/* Sólo se puede configurar el canal, no la frecuencia */
			control = true;
			switch ( CONFIG->Canal ) {
				case U_ADC_CANAL_1:
					canal = UHAL_ADC2_CANAL_1;
					break;
				case U_ADC_CANAL_2:
					canal = UHAL_ADC2_CANAL_2;
					break;
				default:
					uHuboError();
			}
			sConfig.Channel = canal;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
			if (HAL_ADC_ConfigChannel(&admin_adc2, &sConfig) != HAL_OK) uHuboError();
		}
	}

	// Fin!!!
	return control;
}

bool uHALadcObtener ( adc_id_e ID, adc_config_s * CONFIG)
{
	// Variables locales
	bool control = false;

	// ADC seleccionado y control
	if ( UHAL_ADC_1 == ID || UHAL_ADC_2 == ID) {  				// Es ADC válido?
		if ( admin_adc[ID].Estado != ADC_NO_INICIALIZADO ) {	// Es Estado válido?
			control = true;
			CONFIG->Canal = admin_adc[ID].Canal;
			CONFIG->FrecuenciaMuestreo =	UOSAL_FRECUENCIA_RELOJ / 2 /
													( tempo_dma_adc12.Init.Prescaler + 1 ) /
													( tempo_dma_adc12.Init.Period + 1 );
		}
	}
	return control;
}

bool uHALadcComenzarLectura ( adc_id_e ID , uint32_t * VECTOR, uint32_t LARGO)
{
   bool control = false;
   //HAL_ADC_Start_DMA(&hadc1, (uint32_t *) ADC_CONVERTIDO, MUESTRAS);  <-- Esto sería para DMA mono.

   if ( UHAL_ADC_TODOS == ID ) {
   	// Esta función no debe admitir este parámetro porque
   	// no tiene sentido almacenar todx en un mismo vector
   	control = false;
   }

   if ( ID==UHAL_ADC_1 && ADC_INACTIVO == admin_adc[ID].Estado) {
   	// ADC seleccionado y verificación de estado
   	control = true;
   	HAL_ADC_Start (&admin_adc2);
   	HAL_ADCEx_MultiModeStart_DMA (&admin_adc1, VECTOR, LARGO);
   	//uint32_t ESPERA = uOSALmicrosegundos();
   	//do {} while (uOSALmicrosegundos() - ESPERA < 5 );
   	HAL_TIM_Base_Start (&tempo_dma_adc12);
   	admin_adc[ID].Estado = ADC_CORRIENDO;
   }

   return control;
}

bool uHALadcPararLectura ( adc_id_e ID )
{
	bool control = false;
	if ( ID == UHAL_ADC_1 || ID == UHAL_ADC_2 || ID == UHAL_ADC_TODOS ) {
		if (admin_adc[UHAL_ADC_1].Estado == ADC_CORRIENDO ) {
			PARAR_ADC_12 ();	// Paramos ADC1 y ADC2.
									// También actualiza Estado.
	   	control = true;
		}
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
// Esta función está implementada sólo para ADC1
{
	if ( hadc->Instance == ADC1 ) {
		// Paramos ADC1 y ADC2:
		PARAR_ADC_12 ();
		// Llamamos a fn de la aplicación con ADC identificado:
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

void HAL_ADC_MspInit (ADC_HandleTypeDef* adcHandle)
/*-----------------------------------------------------------------------------
                       ADC12 GPIO Configuration
-------------------------------------------------------------------------------
PA0   ------> ADC123_IN0:  Disponible
PA1   ------> ADC123_IN1:  No incuido en ST Zio
PA2   ------> ADC123_IN2:  No incuido en ST Zio
PA3   ------> ADC123_IN3:  UHAL_ADC2_CANAL_0 <-------------------- :-)
PA4   ------> ADC12_IN4:   Usado por DAC_OUT1 <---------- (!!!)
PA5   ------> ADC12_IN5:   Usado por DAC_OUT2 <---------- (!!!)
PA6   ------> ADC12_IN6:   Disponible
PA7   ------> ADC12_IN7:   Disponible
PB0   ------> ADC12_IN8:   Disponible
PB1   ------> ADC12_IN9:   UHAL_ADC1_CANAL_1 <-------------------- :-)
PC0   ------> ADC123_IN10: UHAL_ADC2_CANAL_1 <-------------------- :-)
PC1   ------> ADC123_IN11: No incuido en ST Zio
PC2   ------> ADC123_IN12: UHAL_ADC1_CANAL_0 <-------------------- :-)
PC3   ------> ADC123_IN13: Disponible
PC4   ------> ADC12_IN14:  No incuido en ST Zio
PC5   ------> ADC12_IN15:  No incuido en ST Zio
-------------------------------------------------------------------------------
   - Se utilizan 4 pines.
   - Otros 2 se reservan para los DACs.
   - Hay 5 que no están incluidos en el conector ST Zio. Se descartan.
   - Hay otros 5 que podrían utilizarse. De esos, 2 pueden ser usados por ADC3.
-----------------------------------------------------------------------------*/
{

  if(adcHandle->Instance==ADC1) {
	 /*** ADC1 ***------------------------------------------------------------*/
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* GPIOs de dos canales */
    GPIO_ADC_INICIALIZAR (GPIOB, GPIO_PIN_1);
    GPIO_ADC_INICIALIZAR (GPIOC, GPIO_PIN_2);

    /* ADC1 DMA Init */
    admin_dma_adc1.Instance       = DMA2_Stream0;
    admin_dma_adc1.Init.Channel   = DMA_CHANNEL_0;
    admin_dma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    admin_dma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    admin_dma_adc1.Init.MemInc    = DMA_MINC_ENABLE;
    admin_dma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    admin_dma_adc1.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    admin_dma_adc1.Init.Mode      = DMA_CIRCULAR;
    admin_dma_adc1.Init.Priority  = DMA_PRIORITY_VERY_HIGH;
    admin_dma_adc1.Init.FIFOMode  = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&admin_dma_adc1) != HAL_OK) uHuboError();
    __HAL_LINKDMA(adcHandle,DMA_Handle,admin_dma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

  } else if(adcHandle->Instance==ADC2) {
	 /*** ADC2 ***------------------------------------------------------------*/
	 __HAL_RCC_ADC2_CLK_ENABLE();

    /* GPIOs de dos canales */
    GPIO_ADC_INICIALIZAR (GPIOA, GPIO_PIN_3);
    GPIO_ADC_INICIALIZAR (GPIOC, GPIO_PIN_0);

    /* ADC2 DMA Init */
    admin_dma_adc2.Instance       = DMA2_Stream2;
    admin_dma_adc2.Init.Channel   = DMA_CHANNEL_1;
    admin_dma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    admin_dma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    admin_dma_adc2.Init.MemInc    = DMA_MINC_ENABLE;
    admin_dma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    admin_dma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    admin_dma_adc2.Init.Mode      = DMA_CIRCULAR;
    admin_dma_adc2.Init.Priority  = DMA_PRIORITY_HIGH;
    admin_dma_adc2.Init.FIFOMode  = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&admin_dma_adc2) != HAL_OK) uHuboError();
    __HAL_LINKDMA(adcHandle,DMA_Handle,admin_dma_adc2);

    /* ADC2 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

  }
} /*** fin HAL_ADC_MspInit() ***---------------------------------------------*/

void HAL_ADC_MspDeInit (ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1) {
	 /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC1 interrupt Deinit */
    /**
    * Uncomment the line below to disable the "ADC_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(ADC_IRQn); */

  } else if(adcHandle->Instance==ADC2) {
	 /* Peripheral clock disable */
    __HAL_RCC_ADC2_CLK_DISABLE();

    /**ADC2 GPIO Configuration */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);

    /* ADC2 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC2 interrupt Deinit */
    /**
    * Uncomment the line below to disable the "ADC_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(ADC_IRQn); */

  }
}

bool TEMPO_DMA_INICIALIZAR (void)
{
  bool control = true;
  TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  tempo_dma_adc12.Instance = UHAL_ADC1_TEMPO;   // Definido en uHALconfig.h
  tempo_dma_adc12.Init.Prescaler = PREESCALER_TEMPO_DMA-1;
  tempo_dma_adc12.Init.CounterMode = TIM_COUNTERMODE_UP;
  tempo_dma_adc12.Init.Period = PERIODO_TEMPO_DMA_MINIMO-1;
  tempo_dma_adc12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tempo_dma_adc12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&tempo_dma_adc12) != HAL_OK) control = false;

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tempo_dma_adc12, &sClockSourceConfig) != HAL_OK) control = false;

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tempo_dma_adc12, &sMasterConfig) != HAL_OK) control = false;

  //HAL_TIM_Base_Start (&tempo_dma_s);

  return control;
}

void GPIO_ADC_INICIALIZAR  (GPIO_TypeDef * GPIOx, uint32_t PIN)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if (GPIOx==GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();

	} else if (GPIOx==GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();

	} else if (GPIOx==GPIOC) {
		__HAL_RCC_GPIOC_CLK_ENABLE();

	} else if (GPIOx==GPIOF) {
		__HAL_RCC_GPIOF_CLK_ENABLE();
	}

	GPIO_InitStruct.Pin  = PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PARAR_ADC_12 (void)
{
	// Paramos base de tiempo de muestreo:
	HAL_TIM_Base_Stop(&tempo_dma_adc12);  // Estrictamente, esto no es imprescindible
	// Paramos ADCs:
	HAL_ADCEx_MultiModeStop_DMA (&admin_adc1);
	HAL_ADC_Stop (&admin_adc2);
	// Actualizamos Estado:
	admin_adc[UHAL_ADC_1].Estado = ADC_INACTIVO;
}

/****************************************************************** FIN DE ARCHIVO ***************/
