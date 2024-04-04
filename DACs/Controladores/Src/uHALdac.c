/**************************************************************************************************
 * Archivo:   uHALdac.h
 * Breve:     Interfaz HAL para DACs
 * Autor:     Guillermo Caporaletti
 * Fecha:     creado en agosto 2023
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dac.h"
#include "uOSAL.h"
#include "uHALdac.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define CUENTA_MAX_16B		65535
#define CUENTA_MAX_8B		255

#define U_TIM_DIVISOR		TIM_CLOCKDIVISION_DIV1   // Divisor del reloj.
                                // La fuente de reloj interna queda en 180 MHz/2 = 90 MHz.
#define U_TIM_PREESCALADO	8   // Pre-escalado que no modificaremos: 90 MHz / (8+1) = 10 MHz.
#define U_TIM_PERIODO_INI	1   // Período inicial que variaremos: 10 MHz / (1+1) = 5 MHz.
#define U_TIM_PERIODO_MIN   1   // Valor mínimo que puede tomar el período del tempo.

#define U_FRECUENCIA_BASE   ((double) UHAL_FRECUENCIA_RELOJ) / 2 / (U_TIM_PREESCALADO+1)
                                // Frecuencia común para todas las señales.
                                // No es la máxima, dado que en este caso se divide al menos por 2.
                                // Resulta ser 180 MHz / 2 / (8+1) = 10 MHz
#define DAC_FRECUENCIA_MUESTREO_MINIMA   (UHAL_FRECUENCIA_RELOJ / 2 / (U_TIM_PREESCALADO+1) / 65536)

/****** Definiciones privadas de tipos (private typedef) *****************************************/

typedef enum {
	DAC_NO_INICIALIZADO,
	DAC_INICIALIZADO_DMA
} dac_inicializado;			// Para registrar inicialización del módulo.

typedef enum {
	NO_INICIALIZADO,
	ACTIVO_DMA,
	ACTIVO_VALOR_FIJO,
	PARADO
} dac_estado;				// Para registrar estado de cada canal del DAC.

typedef struct {
	double     Frecuencia_Deseada;
	double     Frecuencia_Configurada;
	uint32_t   Prescalado;
	uint32_t   Periodo;
	uint32_t   * P_Senial;
	uint32_t   Muestras;
	dac_estado Estado;
} dac_config_privada_s;

/****** Definición de datos públicos *************************************************************/

const uint32_t MAXIMO_DAC[UHAL_CANTIDAD_DACS]        = { 3950, 4010 };
const uint32_t MINIMO_DAC[UHAL_CANTIDAD_DACS]        = {   50,  100 };
const uint32_t CERO_DAC  [UHAL_CANTIDAD_DACS]        = { 2000, 2050 };
const double   TRANSFERENCIA_DAC[UHAL_CANTIDAD_DACS] = { 805.861e-6, 805.861e-6 };

/****** Definición de datos privados *************************************************************/

// Constantes para la config inicial de HW
const uint32_t CANAL_DAC      [UHAL_CANTIDAD_DACS] = { DAC_CHANNEL_1,       DAC_CHANNEL_2 };
const uint32_t DISPARO_DAC    [UHAL_CANTIDAD_DACS] = { DAC_TRIGGER_T4_TRGO, DAC_TRIGGER_T2_TRGO };
const TIM_TypeDef * TEMPO_DAC [UHAL_CANTIDAD_DACS] = { TIM4,                TIM2 };

// Variables para manejo de funciones HAL de STM
static DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim [UHAL_CANTIDAD_DACS];

// Variables para administración de DACs
static dac_inicializado     Inicializacion_DACS = DAC_NO_INICIALIZADO;
static dac_config_privada_s DAC_CONFIG [UHAL_CANTIDAD_DACS]  = {0};

/****** Declaración de funciones privadas ********************************************************/

static void   U_TIM_Inicializar  ( dac_id_t );
static bool   U_TIM_DetenerTodos (void);
static bool   U_TIM_IniciarTodos (void);
static void   U_DMA_Inicializar  ( dac_id_t );
static double U_CalculaFrecuencia_TIM ( uint32_t PERIODO );
bool          uHALdacdmaReanudar ( dac_id_t );

/****** Definición de funciones públicas *********************************************************/

/*-------------------------------------------------------------------------------------------------
 * @brief Inicializa DMA, DAC2 y Timer.
 * @param
 * @retval Frecuencia de muestreo
 */
bool uHALdacInicializar ( dac_id_t NUM_DAC )
{
  bool RETORNO = false;

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
	// Inicializo todos los DACs (que queden sin inicializar)
	if ( uHALdacInicializar (UHAL_DAC_1) ) RETORNO = true;
	if ( uHALdacInicializar (UHAL_DAC_2) ) RETORNO = true;

  } else {
    // Inicialización específica de DISPARO de DAC1 o DAC2 --------------------

    if ( DAC_CONFIG[ NUM_DAC ].Estado == NO_INICIALIZADO ) {
	  DAC_ChannelConfTypeDef sConfig = {0};
	  sConfig.DAC_Trigger      = DISPARO_DAC [NUM_DAC];
	  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // Esto habrá que ver si conviene
	  if (HAL_DAC_ConfigChannel( &hdac,
								 &sConfig,
								 CANAL_DAC[NUM_DAC]) != HAL_OK) uManejaError();
	  DAC_CONFIG[ NUM_DAC ].Estado = PARADO;
	  RETORNO = true;
    }
  }
  return RETORNO;
}

/*-------------------------------------------------------------------------------------------------
 * @brief   Configura la frecuencia de muestreo del DAC.
 * @param   DAC_NUM
 * @retval  Frecuencia de muestreo
 *          Devuelve -1 si resulta error
 */
double uHALdacdmaConfigurarFrecuenciaMuestreo ( dac_id_t NUM_DAC, double FRECUENCIA )
{
	// Precondición: estar en un estado permitido.
	if ( DAC_CONFIG[NUM_DAC].Estado == NO_INICIALIZADO) {
		// No aplica la función.
		return -1; // Valos -1 significa error.
	}

	// Variables locales
	uint32_t PERIODO = 0;

	// Impongo límites:
	if (FRECUENCIA > UHAL_DAC_FRECUENCIA_MUESTREO_MAX) FRECUENCIA = UHAL_DAC_FRECUENCIA_MUESTREO_MAX;
	if (FRECUENCIA < DAC_FRECUENCIA_MUESTREO_MINIMA )  FRECUENCIA = DAC_FRECUENCIA_MUESTREO_MINIMA;

	// ¿Cuánto debe/puede ser PERIODO para nuestra FRECUENCIA deseada?
	PERIODO = ceil (U_FRECUENCIA_BASE / FRECUENCIA);  // Toma el divisor entero más alto porque
	                                                  // la frecuencia puede subir si bajamos
	                                                  // la cantidad de muestras por período.
	                                                  // Este valor será 1 como mínimo,
	                                                  // porque U_FRECUENCIA_BASE > 0.
	PERIODO--;  // Este -1 tiene que ver con que PERIODO=n significa que cuenta n+1 ciclos.
	if ( PERIODO < U_TIM_PERIODO_MIN ) PERIODO = U_TIM_PERIODO_MIN;  // Límite inferior...
	if ( PERIODO > 0xFFFF ) PERIODO = 0xFFFF;         // Límite superior, en un contador de 16 bits

	// Almaceno datos
	DAC_CONFIG[NUM_DAC].Frecuencia_Deseada = FRECUENCIA;
	DAC_CONFIG[NUM_DAC].Prescalado = U_TIM_PREESCALADO;
	DAC_CONFIG[NUM_DAC].Periodo = PERIODO;

	// Cargamos estructura de configuración y cambiamos PERIODO
    //HAL_TIM_Base_Stop( &htim[NUM_DAC] );
	//U_TIM_DetenerTodos();

	htim[NUM_DAC].Instance = (TIM_TypeDef *) TEMPO_DAC[NUM_DAC] ;
	htim[NUM_DAC].Init.Prescaler = U_TIM_PREESCALADO;
	htim[NUM_DAC].Init.Period = PERIODO;
	htim[NUM_DAC].Init.CounterMode = TIM_COUNTERMODE_UP;
	htim[NUM_DAC].Init.ClockDivision = U_TIM_DIVISOR;
	htim[NUM_DAC].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	__HAL_TIM_SET_AUTORELOAD( &htim[NUM_DAC], PERIODO);
	//HAL_TIM_Base_Start( &htim[NUM_DAC] );
	//U_TIM_IniciarTodos();

	// Evaluamos frecuencia resultante y retornamos
	DAC_CONFIG[NUM_DAC].Frecuencia_Configurada = uHALdacdmaLeerFrecuenciaMuestreo ( NUM_DAC );
	uHALdacdmaSincronizar();
	return DAC_CONFIG[NUM_DAC].Frecuencia_Configurada;

}

/*-------------------------------------------------------------------------------------------------
 * @brief   Devuelve la frecuencia de muestreo configurada
 * @param   DAC_NUM
 * @retval  Frecuencia de muestreo configurada
 *          Devuelve -1 si resulta error
 */
double uHALdacdmaLeerFrecuenciaMuestreo ( dac_id_t NUM_DAC)
{
	// Precondición: estar en un estado permitido.
	if ( DAC_CONFIG[NUM_DAC].Estado == NO_INICIALIZADO) {
		// No aplica la función.
		return -1; // Valor -1 significa error.
	}

	// Cálculo...
	double FM = 0;
	FM = U_CalculaFrecuencia_TIM ( htim[NUM_DAC].Init.Period );
	return FM;
}

/*-------------------------------------------------------------------------------------------------
 * @brief   Devuelve la frecuencia base común a todos los DACs
 * @param   Ninguno
 * @retval  Frecuencia base
 */
double uHALdacdmaLeerFrecuenciaBase (void)
{
	return (double) U_FRECUENCIA_BASE;
}

/*-------------------------------------------------------------------------------------------------
 * @brief   Devuelve la frecuencia máxima de muestreo que puede tener un DAC
 * @param   Ninguno
 * @retval  Frecuencia máxima
 */
double uHALdacdmaLeerFrecuenciaMaxima (void)
{
	return ((double) U_FRECUENCIA_BASE) / (U_TIM_PERIODO_MIN+1);
}

/*-------------------------------------------------------------------------------------------------
 * @brief   Devuelve la frecuencia mínima de muestreo que puede tener un DAC
 * @param   Ninguno
 * @retval  Frecuencia mínima
 */
double uHALdacdmaLeerFrecuenciaMinima (void)
{
	return DAC_FRECUENCIA_MUESTREO_MINIMA;
}

/*-------------------------------------------------------------------------------------------------
 * @brief   Devuelve el divisor de la frecuencia base con el que se logra la frecuencia deseada
 * @param   Ninguno
 * @retval  Divisor configurado
 */
uint32_t uHALdacdmaLeerDivisor (dac_id_t NUM_DAC)
{
	if (DAC_CONFIG [NUM_DAC].Estado == NO_INICIALIZADO) return 0;
	return DAC_CONFIG [NUM_DAC].Periodo+1;
}

/*-------------------------------------------------------------------------------------------------
 * @brief   Comienza a enviar Datos a salida por DAC
 * @param   Canal DAC, puntero a DATOS y cantidad de datos NUM_DATOS
 * @retval  None
 */
bool uHALdacdmaComenzar ( dac_id_t NUM_DAC, uint32_t * DATOS, uint32_t NUM_DATOS)
{
	// Precondición: estar en un estado permitido.
	if ( DAC_CONFIG[NUM_DAC].Estado != PARADO ) {
		// La función no aplica
		return false;
	}

	// Ejecutamos...
	DAC_CONFIG[NUM_DAC].P_Senial = DATOS;
	DAC_CONFIG[NUM_DAC].Muestras = NUM_DATOS;
	HAL_DAC_Start_DMA( &hdac,
			           CANAL_DAC[NUM_DAC],
					   DATOS,
					   NUM_DATOS,
					   DAC_ALIGN_12B_R);
	DAC_CONFIG[NUM_DAC].Estado = ACTIVO_DMA;

	// Sincronizamos (la función decide si actúa) y retornamos
	uHALdacdmaSincronizar();
	return true;
}

/*-------------------------------------------------------------------------------------------------
  * @brief Para el envío de Datos a salida por DAC
  * @param None
  * @retval None
  */
bool uHALdacParar ( dac_id_t NUM_DAC)
{
	bool RETORNO = false;
	// Precondición: estar en un estado permitido.

	if ( DAC_CONFIG[NUM_DAC].Estado == ACTIVO_DMA ) {
		// Venía funcionando con DMA ---------------------------------------
		HAL_DAC_Stop_DMA( &hdac, CANAL_DAC[NUM_DAC] );
		DAC_CONFIG[NUM_DAC].Estado = PARADO;
		RETORNO = true;

	} else if ( DAC_CONFIG[NUM_DAC].Estado == ACTIVO_VALOR_FIJO ) {
		// Tenía un valor fijo ---------------------------------------------
		HAL_DAC_Stop    ( &hdac, CANAL_DAC[NUM_DAC] );
		DAC_CONFIG[NUM_DAC].Estado = PARADO;
		RETORNO = true;
	}
	return RETORNO;
}

/*-------------------------------------------------------------------------------------------------
  * @brief Sincroniza ambas señales, si están activas.
  * @param None
  * @retval None
  */
bool   uHALdacdmaSincronizar ( void )
{
	// Precondiciones:
	// a) Debe estar en estado ACTIVO_DMA (corriendo)
	if ( DAC_CONFIG[ UHAL_DAC_1 ].Estado != ACTIVO_DMA ||
		 DAC_CONFIG[ UHAL_DAC_2 ].Estado != ACTIVO_DMA  ) {
		return false;
	}

	// b) Deben tener misma frecuencia de muestreo
	if ( DAC_CONFIG[ UHAL_DAC_1 ].Frecuencia_Configurada !=
		 DAC_CONFIG[ UHAL_DAC_2 ].Frecuencia_Configurada  ) {
		return false;
	}
	/*
	// c) Deben tener la misma cantidad de muestras
	if ( DAC_CONFIG[ UHAL_DAC_1 ].Muestras !=
		 DAC_CONFIG[ UHAL_DAC_2 ].Muestras  ) {
		return false;
	}
	*/

	// 1) Paramos ambas señales y los temporizadores:
    uHALdacParar ( UHAL_DAC_1 );
    uHALdacParar ( UHAL_DAC_2 );
    U_TIM_DetenerTodos ();

    // 2) Ejecutamos con una muestra (prueba)
    HAL_DAC_Start_DMA( &hdac,
    				   CANAL_DAC[UHAL_DAC_1],
    				   DAC_CONFIG[UHAL_DAC_1].P_Senial,
    				   1,
    				   DAC_ALIGN_12B_R);
    HAL_DAC_Start_DMA( &hdac,
        			   CANAL_DAC[UHAL_DAC_2],
        			   DAC_CONFIG[UHAL_DAC_2].P_Senial,
        			   1,
        			   DAC_ALIGN_12B_R);
    HAL_DAC_Stop_DMA( &hdac,
    		          CANAL_DAC[UHAL_DAC_1] );
    HAL_DAC_Stop_DMA( &hdac,
			          CANAL_DAC[UHAL_DAC_2] );

	// 3) Reanudamos pero iniciando temporizadores a la vez:
	uHALdacdmaReanudar ( UHAL_DAC_1 );
	uHALdacdmaReanudar ( UHAL_DAC_2 );
	U_TIM_IniciarTodos ();

	return true;
}

/*-------------------------------------------------------------------------------------------------
  * @brief TIM Initialization Function
  * @param Número de DAC al cual está asociado el TEMPO
  * @retval None
  */
bool     uHALdacEstablecerValor ( dac_id_t DAC_ID, uint32_t VALOR)
{
	if ( DAC_CONFIG[ DAC_ID ].Estado == PARADO ) {
		// Aplica función --------------------------------------------
		HAL_DAC_SetValue( &hdac,
				          CANAL_DAC[ DAC_ID ],
						  DAC_ALIGN_12B_R,
						  VALOR);
		HAL_DAC_Start   ( &hdac,
				          CANAL_DAC[ DAC_ID ]);
		DAC_CONFIG[ DAC_ID ].Estado = ACTIVO_VALOR_FIJO;
		return true;

	} else {
		// No aplica función -----------------------------------------
		return false;
	}
}

/****** Definición de funciones privadas *********************************************************/


/*-------------------------------------------------------------------------------------------------
  * @brief Reanuda señal
  * @param None
  * @retval None
  */
bool uHALdacdmaReanudar ( dac_id_t NUM_DAC)
{
	// Precondición: estar en un estado permitido.
	if ( DAC_CONFIG[NUM_DAC].Estado == PARADO ) {
		HAL_DAC_Start_DMA( &hdac,
				           CANAL_DAC[NUM_DAC],
						   DAC_CONFIG[NUM_DAC].P_Senial,
						   DAC_CONFIG[NUM_DAC].Muestras,
						   DAC_ALIGN_12B_R);
		DAC_CONFIG[NUM_DAC].Estado = ACTIVO_DMA;
		return true;
	} else {
		return false;
	}
}

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
    htim[NUM_DAC].Instance           = (TIM_TypeDef *) TEMPO_DAC[NUM_DAC];  // TEMPOrizador para el DAC
    htim[NUM_DAC].Init.ClockDivision = U_TIM_DIVISOR;      // Divisor que no modificaremos
    htim[NUM_DAC].Init.Prescaler     = U_TIM_PREESCALADO;  // Pre-escalado que no modificaremos
    htim[NUM_DAC].Init.Period        = U_TIM_PERIODO_INI;  // Valor inicial que modificaremos
    htim[NUM_DAC].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // Permite cambiar conf
    htim[NUM_DAC].Init.CounterMode   = TIM_COUNTERMODE_UP; // Cuenta ascendente

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
  * @brief  Detiene ambos temporizadores del DMA
  * @param
  * @retval true si la operacion fue exitosa
  */
static bool U_TIM_DetenerTodos   (void)
{
    HAL_TIM_Base_Stop( &htim[ UHAL_DAC_1 ] );
	HAL_TIM_Base_Stop( &htim[ UHAL_DAC_2 ] );
	return true;
}

/*-------------------------------------------------------------------------------------------------
  * @brief  Re-iniciamos los tempos de DMA
  * @param
  * @retval true si la operación fue exitosa.
  */
static bool   U_TIM_IniciarTodos (void)
{
   /* Check the parameters */
   assert_param(IS_TIM_INSTANCE(htim[ UHAL_DAC_1 ]->Instance));
   assert_param(IS_TIM_INSTANCE(htim[ UHAL_DAC_2 ]->Instance));

   /* Check the TIM state */
   if (htim[ UHAL_DAC_1 ].State != HAL_TIM_STATE_READY) return false;
   if (htim[ UHAL_DAC_2 ].State != HAL_TIM_STATE_READY) return false;

   /* Variables locales */
   uint32_t Cuenta1 = DAC_CONFIG[UHAL_DAC_1].Periodo;
   uint32_t Cuenta2 = DAC_CONFIG[UHAL_DAC_1].Periodo;
   for (uint8_t i; i<1; i++) {
	   if (Cuenta1 > 0) Cuenta1--;   // Reduce el defasaje.
   }

   /*Cuenta1=0;
   Cuenta2=0;*/

   /* Set the TIM state */
   htim[ UHAL_DAC_1 ].State = HAL_TIM_STATE_BUSY;
   htim[ UHAL_DAC_2 ].State = HAL_TIM_STATE_BUSY;

   /* Enable the Peripheral
    * Atención: en la función HAL original revisa si un temporizador está en modo esclavo para ser activado por otro tempo */
   __HAL_TIM_SET_COUNTER( &htim[UHAL_DAC_1] , Cuenta1 );
   __HAL_TIM_SET_COUNTER( &htim[UHAL_DAC_2] , Cuenta2 );
   __HAL_TIM_ENABLE( &htim[ UHAL_DAC_1 ]);
   __HAL_TIM_ENABLE( &htim[ UHAL_DAC_2 ]);

   //HAL_TIM_Base_Start( &htim[ UHAL_DAC_1 ] );
   //HAL_TIM_Base_Start( &htim[ UHAL_DAC_2 ] );

   return true;
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
	return ((double) UHAL_FRECUENCIA_RELOJ) / 2 / (U_TIM_PREESCALADO+1) / (PERIODO+1);
}

/****************************************************************** FIN DE ARCHIVO ***************/
