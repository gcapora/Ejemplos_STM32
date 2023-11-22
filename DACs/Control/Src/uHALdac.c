/**************************************************************************************************
 * Archivo:   uHALdac.h
 * Breve:     Interfaz HAL para DACs
 * Autor:     Guillermo Caporaletti
 * Fecha:     creado en agosto 2023
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dac.h"
#include "uOSAL.h"
#include "uHALdac.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define CUENTA_MAX_16B		65535
#define CUENTA_MAX_8B		255
#define U_TIM_DIVISOR		TIM_CLOCKDIVISION_DIV1  // Divisor del reloj: no modifica los 90 MHz
                                                    // La fuente de reloj interna tiene 180 MHz/2
#define U_TIM_PREESCALADO	8  // Pre-escalado que no modificaremos: 90/(8+1) = 10 MHz
#define U_TIM_PERIODO_INI	1  // Período inicial que variaremos: 10/(1+1) = 5 MHz
#define DAC_FRECUENCIA_MUESTREO_MINIMA   (FRECUENCIA_RELOJ / 2 / (U_TIM_PREESCALADO+1) / 65536)

/****** Definiciones privadas de tipos (private typedef) *****************************************/

typedef enum {
	DAC_NO_INICIALIZADO,
	DAC_INICIALIZADO_DMA
} dac_inicializado;

typedef struct {
	double Frecuencia_Deseada;
	double Frecuencia_Configurada;
	uint32_t Prescalado;
	uint32_t Periodo;
	uint32_t * P_Senial;
	uint32_t Muestras;
} dac_config_privada_s;

/****** Definición de datos públicos *************************************************************/


/****** Definición de datos privados *************************************************************/

// Constantes para la config inicial de HW
const uint32_t CANAL_DAC      [UHAL_CANTIDAD_DACS] = { DAC_CHANNEL_1,       DAC_CHANNEL_2 };
const uint32_t DISPARO_DAC    [UHAL_CANTIDAD_DACS] = { DAC_TRIGGER_T4_TRGO, DAC_TRIGGER_T2_TRGO };
const TIM_TypeDef * TEMPO_DAC [UHAL_CANTIDAD_DACS] = { TIM4,                TIM2 };

/*const uint32_t DISPARO_DAC [UHAL_CANTIDAD_DACS] = { DAC_TRIGGER_T4_TRGO, DAC_TRIGGER_T2_TRGO };
const uint32_t TEMPO_DAC [UHAL_CANTIDAD_DACS]   = { (uint32_t) TIM4,     (uint32_t) TIM2     };*/

// Variables para manejo de funciones HAL de STM
static DAC_HandleTypeDef hdac;
static TIM_HandleTypeDef htim [UHAL_CANTIDAD_DACS];
//extern DMA_HandleTypeDef hdma_dac2;	// Variable definida por IDE en main.c para manejo de DMA
//extern DMA_HandleTypeDef hdma_dac1;	// Variable definida por IDE en main.c para manejo de DMA

// Variables para administración de DACs
static dac_inicializado     Inicializacion_DACS = DAC_NO_INICIALIZADO;
static dac_config_privada_s DAC_CONFIG [UHAL_CANTIDAD_DACS]  = {0};

/****** Declaración de funciones privadas ********************************************************/

static void   U_TIM_Inicializar ( dac_id_t );
static void   U_DMA_Inicializar ( dac_id_t );
static double U_CalculaFrecuencia_TIM ( uint32_t PERIODO );

/****** Definición de funciones públicas *********************************************************/

/*-------------------------------------------------------------------------------------------------
 * @brief Inicializa DMA, DAC2 y Timer.
 * @param
 * @retval Frecuencia de muestreo
 */
void uHALdacdmaInicializar ( dac_id_t NUM_DAC )
{
  // Verificamos inicialización general -------------------------------------------------
  if (DAC_NO_INICIALIZADO == Inicializacion_DACS)
  {
	// Inicializamos canales DMA ----------------------------------------------
	U_DMA_Inicializar( UHAL_DAC_TODOS );

	// Inicializamos DAC (que tiene dos canales) ------------------------------
	hdac.Instance = DAC;
	if (HAL_DAC_Init(&hdac) != HAL_OK) uManejaError();

	// Inicializacion de TEMPORIZADORES ---------------------------------------
	U_TIM_Inicializar( UHAL_DAC_TODOS );
	DAC_CONFIG[ UHAL_DAC_2 ].Periodo = U_TIM_PERIODO_INI;
	DAC_CONFIG[ UHAL_DAC_1 ].Periodo = U_TIM_PERIODO_INI;

	// Recordamos inicialización general --------------------------------------
	Inicializacion_DACS = DAC_INICIALIZADO_DMA;
  }

  // Verifico inicialización específica -------------------------------------------------
  if ( UHAL_DAC_TODOS == NUM_DAC ) {
	// Inicializo todos los DACs
	uHALdacdmaInicializar (UHAL_DAC_1);
	uHALdacdmaInicializar (UHAL_DAC_2);

  } else {
	// Inicialización específica de DISPARO de DAC1 o DAC2 --------------------
	DAC_ChannelConfTypeDef sConfig = {0};
	sConfig.DAC_Trigger      = DISPARO_DAC [NUM_DAC];
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // Esto habrá que ver si conviene
	if (HAL_DAC_ConfigChannel( &hdac,
							   &sConfig,
							   CANAL_DAC[NUM_DAC]) != HAL_OK) uManejaError();
  }

  return;
}

/*-------------------------------------------------------------------------------------------------
 * @brief Configura el DAC.
 * @param DAC_NUM
 * @retval Frecuencia de muestreo
 */
double uHALdacdmaConfigurarFrecuenciaMuestreo ( dac_id_t NUM_DAC, double FRECUENCIA )
{
	uint32_t PERIODO = 0;
	double CALCULO = 0;

	// Impongo límites:
	if (FRECUENCIA > DAC_FRECUENCIA_MUESTREO_MAX) FRECUENCIA = DAC_FRECUENCIA_MUESTREO_MAX;
	if (FRECUENCIA < DAC_FRECUENCIA_MUESTREO_MINIMA ) FRECUENCIA = DAC_FRECUENCIA_MUESTREO_MINIMA;

	// ¿Cuánto debe/puede ser PERIODO para nuestra FRECUENCIA deseada?
	CALCULO = (double) (FRECUENCIA_RELOJ/2) / (U_TIM_PREESCALADO+1) / FRECUENCIA;
	if ( CALCULO-1 <= 0 ) CALCULO = 1;
	PERIODO = (uint32_t) CALCULO - 1;
	if ( U_CalculaFrecuencia_TIM (PERIODO) > FRECUENCIA ) PERIODO++; // función ceil improvisada.

	// Almaceno datos
	DAC_CONFIG[NUM_DAC].Frecuencia_Deseada = FRECUENCIA;
	DAC_CONFIG[NUM_DAC].Prescalado = U_TIM_PREESCALADO;
	DAC_CONFIG[NUM_DAC].Periodo = PERIODO;

	// Cargo estructura de configuración
	htim[NUM_DAC].Init.Prescaler = U_TIM_PREESCALADO;
	htim[NUM_DAC].Init.Period = PERIODO;
	htim[NUM_DAC].Instance = (TIM_TypeDef *) TEMPO_DAC[NUM_DAC] ;
	htim[NUM_DAC].Init.CounterMode = TIM_COUNTERMODE_UP;
	htim[NUM_DAC].Init.ClockDivision = U_TIM_DIVISOR;
	htim[NUM_DAC].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	__HAL_TIM_SET_AUTORELOAD( &htim[NUM_DAC], PERIODO);

	// if ( HAL_TIM_Base_Init(&htim[NUM_DAC]) != HAL_OK ) uManejaError();
	// HAL_TIM_Base_Start( &htim[NUM_DAC] );

	DAC_CONFIG[NUM_DAC].Frecuencia_Configurada = uHALdacdmaLeerFrecuenciaMuestreo ( NUM_DAC );
	return DAC_CONFIG[NUM_DAC].Frecuencia_Configurada;

	if (1)
	//( DAC_CONFIG[ UHAL_DAC_1 ].Periodo  == DAC_CONFIG[ UHAL_DAC_2 ].Periodo )
	{
		uHALdacdmaSincronizar ();
	}
}

/*-------------------------------------------------------------------------------------------------
 * @brief Devuelve la frecuencia de muestreo configurada
 * @param DAC_NUM
 * @retval frecuencia de muestreo configurada
 */
double uHALdacdmaLeerFrecuenciaMuestreo ( dac_id_t NUM_DAC)
{
	double FM = 0;
	/*uint32_t Cuenta = 0;
	Cuenta = htim[NUM_DAC].Init.Prescaler+1;
	Cuenta = Cuenta * htim[NUM_DAC].Init.Period+1; */
	FM = U_CalculaFrecuencia_TIM ( htim[NUM_DAC].Init.Period );
	return FM;
}

/*-------------------------------------------------------------------------------------------------
 * @brief Comienza a enviar Datos a salida por DAC
 * @param Puntero a Datos y cantidad de datos Num_Datos
 * @retval None
 */
void uHALdacdmaComenzar ( dac_id_t NUM_DAC, uint32_t * DATOS, uint32_t NUM_DATOS)
{
	DAC_CONFIG[NUM_DAC].P_Senial = DATOS;
	DAC_CONFIG[NUM_DAC].Muestras = NUM_DATOS;
	HAL_DAC_Start_DMA( &hdac,
			           CANAL_DAC[NUM_DAC],
					   DATOS,
					   NUM_DATOS,
					   DAC_ALIGN_12B_R);
}

/*-------------------------------------------------------------------------------------------------
  * @brief Para el envío de Datos a salida por DAC
  * @param None
  * @retval None
  */
void uHALdacdmaParar ( dac_id_t NUM_DAC)
{
	HAL_DAC_Stop_DMA( &hdac,
			          CANAL_DAC[NUM_DAC] );
}

/*-------------------------------------------------------------------------------------------------
  * @brief Reanuda señal
  * @param None
  * @retval None
  */
void uHALdacdmaReanudar ( dac_id_t NUM_DAC)
{
	HAL_DAC_Start_DMA( &hdac,
			           CANAL_DAC[NUM_DAC],
					   DAC_CONFIG[NUM_DAC].P_Senial,
					   DAC_CONFIG[NUM_DAC].Muestras,
					   DAC_ALIGN_12B_R);
}

/*-------------------------------------------------------------------------------------------------
  * @brief Reanuda señal
  * @param None
  * @retval None
  */
void   uHALdacdmaSincronizar ( void )
{
	HAL_TIM_Base_Stop( &htim[ UHAL_DAC_1 ] );
	HAL_TIM_Base_Stop( &htim[ UHAL_DAC_1 ] );
	//uHALdacdmaParar ( UHAL_DAC_1 );
	//uHALdacdmaParar ( UHAL_DAC_2 );
	__HAL_TIM_SET_COUNTER( &htim[UHAL_DAC_1] , 0 );
	__HAL_TIM_SET_COUNTER( &htim[UHAL_DAC_2] , 0 );
	//uHALdacdmaReanudar ( UHAL_DAC_1 );
	//uHALdacdmaReanudar ( UHAL_DAC_2 );
	HAL_TIM_Base_Start( &htim[ UHAL_DAC_1 ] );
	HAL_TIM_Base_Start( &htim[ UHAL_DAC_2 ] );
}


/****** Definición de funciones privadas *********************************************************/

/*-------------------------------------------------------------------------------------------------
  * @brief TIM Initialization Function
  * @param Número de DAC al cual está asociado el TEMPO
  * @retval None
  */
static void U_TIM_Inicializar( dac_id_t NUM_DAC )
{
  if ( UHAL_DAC_TODOS == NUM_DAC ) {
	// Inicializo todos los DACs
    U_TIM_Inicializar (UHAL_DAC_1);
    U_TIM_Inicializar (UHAL_DAC_2);

  } else {
	// Inicializo el DAC solicitado

    // Variables locales para configuración
    TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // Cargo estructura de configuración y asigno configuración inicial (siempre igual)
    htim[NUM_DAC].Instance          = (TIM_TypeDef *) TEMPO_DAC[NUM_DAC];  // TEMPOrizador para el DAC
    htim[NUM_DAC].Init.Prescaler    = U_TIM_PREESCALADO;   // Pre-escalado que no modificaremos
    htim[NUM_DAC].Init.CounterMode  = TIM_COUNTERMODE_UP;  // Cuenta ascendente
    htim[NUM_DAC].Init.Period       = U_TIM_PERIODO_INI;   // Valor inicial que modificaremos
    htim[NUM_DAC].Init.ClockDivision = U_TIM_DIVISOR;     // Divisor que no modificaremos
    htim[NUM_DAC].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // Permite cambiar conf
    if ( HAL_TIM_Base_Init(&htim[NUM_DAC]) != HAL_OK ) uManejaError();

    // Fuente de reloj para TEMPORIZADOR
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;     // Freloj/2 = 90 MHz
    if ( HAL_TIM_ConfigClockSource(&htim[NUM_DAC], &sClockSourceConfig) != HAL_OK ) uManejaError();

    // Disparo
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if ( HAL_TIMEx_MasterConfigSynchronization(&htim[NUM_DAC], &sMasterConfig ) != HAL_OK) uManejaError();

    // Lanzamos temporizador directamente
    HAL_TIM_Base_Start( &htim[NUM_DAC] );
  }
  return;
}

/*-------------------------------------------------------------------------------------------------
  * @brief  Inicialización de canales DMA
  * @param
  * @retval None
  */
static void U_DMA_Inicializar( dac_id_t NUM_DAC )
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority ( DMA1_Stream5_IRQn, 0, 0 );
  HAL_NVIC_EnableIRQ   ( DMA1_Stream5_IRQn );
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority ( DMA1_Stream6_IRQn, 0, 0 );
  HAL_NVIC_EnableIRQ   ( DMA1_Stream6_IRQn );
}

/*-------------------------------------------------------------------------------------------------
  * @brief Obtengo la frecuencia de muestro con los parámetros dados
  * @param PERIODO (podrían agregarse más)
  * @retval Frecuencia de muestreo del temporizador
  */
static double U_CalculaFrecuencia_TIM ( uint32_t PERIODO )
{
	return ((double) FRECUENCIA_RELOJ) / 2 / (U_TIM_PREESCALADO+1) / (PERIODO+1);
}

/****************************************************************** FIN DE ARCHIVO ***************/
