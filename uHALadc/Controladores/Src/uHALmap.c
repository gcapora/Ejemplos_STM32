/**************************************************************************************************
 * Archivo: uHALadc.c
 * Breve:
 * Fecha:	Creado en abril 2024
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "stm32f4xx_hal.h"
#include <math.h>
#include "uHAL.h"  // Con solo esta librería alcanzaría, pero el resto uHAL... ayuda a la lectura.
#include "uHALconfig.h"
#include "uHALmap.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define Prescaler0      24    // Pre-escalado inicial de los tempos
#define Periodo0        6000  // Período inicial
#define CicloActivo0    3000  // Parte del período que la salida está activa


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos privados *************************************************************/

TIM_HandleTypeDef Tempo_admin        [UHAL_CANTIDAD_MAP] = { 0 };
const uint32_t		Tempo_canal        [UHAL_CANTIDAD_MAP] = { TIM_CHANNEL_1 };
bool					Tempo_inicializado [UHAL_CANTIDAD_MAP] = { 0 };

/****** Importación de datos públicos ************************************************************/


/****** Declaración de funciones privadas ********************************************************/

void HAL_MSP_INICIALIZAR_PIN ( TIM_HandleTypeDef* timHandle );

/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
  * @brief   Configura una MAP específico
  * @param
  * @retval
  */
bool uHALmapInicializar ( map_id_e ID )
{
   // Variables locales
	bool                   control = false;
   TIM_ClockConfigTypeDef sClockSourceConfig = {0};
   TIM_OC_InitTypeDef     sConfigOC = {0};
   TIM_TypeDef *          Instance_seleccionada = NULL;

   // Verificación
   if ( ID != UHAL_MAP_TODOS ) {
		if ( ID >= UHAL_CANTIDAD_MAP ) return false;
	}

   // ¿Inicializamos todos?
   if ( ID == UHAL_MAP_TODOS ) {
   	control = true;
   	for ( map_id_e contador=0; contador<UHAL_CANTIDAD_MAP; contador++ ) {
   		if ( false == uHALmapInicializar ( contador ) ) control = false;
   	}
   	return control;
   }

   // Selección de Instance
   switch (ID) {
   	case UHAL_MAP_PE5:
   		Instance_seleccionada = UHAL_MAP_PE5_TEMPO;
   		break;
   	default:
   		Instance_seleccionada = NULL;
   }

   // Inicializamos!!!
   if ( Instance_seleccionada != NULL ) {
   	// uOSALledEncender ( UOSAL_PIN_LED_ROJO_INCORPORADO );

   	Tempo_admin[ID].Instance = Instance_seleccionada;
   	Tempo_admin[ID].Init.Prescaler = Prescaler0-1;        // Valor inicial que reduce frecuencia
   	Tempo_admin[ID].Init.CounterMode = TIM_COUNTERMODE_UP;
   	Tempo_admin[ID].Init.Period = Periodo0-1;             // Valor inicial que pone período
   	Tempo_admin[ID].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   	Tempo_admin[ID].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
      if (HAL_TIM_Base_Init(&Tempo_admin[ID]) != HAL_OK) uoHuboError();

      sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
      if (HAL_TIM_ConfigClockSource( &Tempo_admin[ID], &sClockSourceConfig) != HAL_OK) uoHuboError();

      if (HAL_TIM_PWM_Init( &Tempo_admin[ID]) != HAL_OK) uoHuboError();

      sConfigOC.OCMode = TIM_OCMODE_PWM1;
      sConfigOC.Pulse = CicloActivo0;                        // Valor inicial para CdT
      sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
      sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
      if (HAL_TIM_PWM_ConfigChannel( &Tempo_admin[ID], &sConfigOC, TIM_CHANNEL_1) != HAL_OK) uoHuboError();

      HAL_MSP_INICIALIZAR_PIN ( &Tempo_admin[ID] );

      Tempo_inicializado [ID] = true;
      control = true;
   }

   return control;
}

bool uHALmapDesInicializar ( map_id_e ID)
{
	  return true;  // falta implementar
}

bool uHALmapConfigurar ( map_id_e ID, map_config_s * CONFIG )
{
	bool control = false;
	uint32_t CUENTA = 0;
	uint32_t CICLO = 0;

	// Verificación
	if ( ID != UHAL_MAP_TODOS ) {
		if ( ID >= UHAL_CANTIDAD_MAP ) return false;
		if ( false == Tempo_inicializado [ID] ) return false;
	}
	if ( CICLO > 1 ) CICLO = 1;
	if ( CICLO < 0 ) CICLO = 0;

   // ¿Configurar todos?
   if ( ID == UHAL_MAP_TODOS ) {
   	for ( map_id_e contador=0 ; contador<UHAL_CANTIDAD_MAP; contador++ ) {
   		if ( true == uHALmapConfigurar ( contador, CONFIG ) ) control = true;
   	}
   	return control;  // Devuelve true si hubo al menos uno configurado.
   }

   // Configuro y salgo
   CUENTA = (uint32_t) round ( UOSAL_FRECUENCIA_RELOJ
										 / CONFIG->Frecuencia
										 / (Tempo_admin[ID].Init.Prescaler+1) );
   if ( CUENTA < 2 )     CUENTA = 2;     // Tiene que dividir por 2 al menos para verse el PWM
   if ( CUENTA > 65536 ) CUENTA = 65536; // Divisor de 16 bits
                                         // Para frecuencias más bajas debe mofificarse Preescaler.
   CICLO  = (uint32_t) round (CONFIG->CicloDeTrabajo * CUENTA );

   __HAL_TIM_SET_AUTORELOAD( &Tempo_admin[ID], CUENTA-1);
   Tempo_admin[ID].Instance->CCR1 = CICLO;

   return true;
}

double	uHALmapConfigurarFrecuencia	( map_id_e ID, double FREC)
{
	map_config_s CONFIG = {0};
	double control = -1;

	// Verificación
	if ( ID != UHAL_MAP_TODOS ) {
		if ( ID >= UHAL_CANTIDAD_MAP ) return control;
		if ( false == Tempo_inicializado [ID] ) return control;
	}
	if ( FREC < 0 ) return control;

	// ¿Configurar todos?
	if ( ID == UHAL_MAP_TODOS ) {
		control = -1;
		for ( map_id_e contador=0 ; contador<UHAL_CANTIDAD_MAP; contador++ ) {
			if ( -1 != uHALmapConfigurarFrecuencia ( contador, FREC ) ) control = 1;
	   }
	   return control;  // Devuelve 1 si hubo al menos uno configurado.
	}

	// Configuro frecuencia
	CONFIG.Frecuencia = FREC;
	CONFIG.CicloDeTrabajo = (double) (Tempo_admin[ID].Instance->CCR1)
			                         / (Tempo_admin[ID].Instance->ARR+1);
	if ( false == uHALmapConfigurar ( UHAL_MAP_PE5, &CONFIG ) ) {
		// No pudo configurar frecuencia...
		control = -1;
	} else {
		// Configuración exitosa: ahora calculo frecuencia resultante
		control = UOSAL_FRECUENCIA_RELOJ
				    / (Tempo_admin[ID].Init.Prescaler+1)
					 / (Tempo_admin[ID].Instance->ARR+1);
	}
	return control;
}

double	uHALmapObtenerFrecuencia		( map_id_e ID )
{
	double control = -1;

	// Verificación
	if ( ID == UHAL_MAP_TODOS ) return control;
	if ( ID >= UHAL_CANTIDAD_MAP ) return control;
	if ( false == Tempo_inicializado [ID] ) return control;

	// Obtengo frecuencia
	control = UOSAL_FRECUENCIA_RELOJ
				 / (Tempo_admin[ID].Init.Prescaler+1)
				 / (Tempo_admin[ID].Instance->ARR+1);
	return control;
}

bool uHALmapEncender ( map_id_e ID )
{
   // Verificación
	if ( ID != UHAL_MAP_TODOS ) {
		if ( ID >= UHAL_CANTIDAD_MAP ) return false;
		if ( false == Tempo_inicializado [ID] ) return false;
	}
	//uOSALledEncender ( UOSAL_PIN_LED_ROJO_INCORPORADO );

   // ¿Encender todos?
   if ( ID == UHAL_MAP_TODOS ) {
   	bool control = false;
   	for ( map_id_e contador=0 ; contador<UHAL_CANTIDAD_MAP; contador++ ) {
   		if ( true == uHALmapEncender ( contador ) ) control = true;
   	}
   	return control;  // Devuelve true si hubo al menos uno inicializado.
   }

   // Enciendo específico y salgo
   HAL_TIM_PWM_Start ( &Tempo_admin[ID], Tempo_canal[ID] );
   return true;
}

bool uHALmapApagar ( map_id_e ID )
{
   // Verificación
	if ( ID != UHAL_MAP_TODOS ) {
		if ( ID >= UHAL_CANTIDAD_MAP ) return false;
		if ( false == Tempo_inicializado [ID] ) return false;
	}
	//uOSALledEncender ( UOSAL_PIN_LED_ROJO_INCORPORADO );

   // ¿Apagar todos?
   if ( ID == UHAL_MAP_TODOS ) {
   	bool control = false;
   	for ( map_id_e contador=0 ; contador<UHAL_CANTIDAD_MAP; contador++ ) {
   		if ( true == uHALmapApagar ( contador ) ) control = true;
   	}
   	return control;  // Devuelve true si hubo al menos uno inicializado.
   }

   // Apagado específico y salgo
   HAL_TIM_PWM_Stop ( &Tempo_admin[ID], Tempo_canal[ID] );
   return true;
}


/****** Definición de funciones privadas *********************************************************/

void HAL_MSP_INICIALIZAR_PIN ( TIM_HandleTypeDef* timHandle )
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(timHandle->Instance==TIM9)
	{
		/* TIM9 GPIO Configuration
		 * PE5 ------> TIM9_CH1
	    */
		__HAL_RCC_GPIOE_CLK_ENABLE();
	   GPIO_InitStruct.Pin = GPIO_PIN_5;
	   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	   GPIO_InitStruct.Pull = GPIO_NOPULL;
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	   GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
	   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
}

/****************************************************************** FIN DE ARCHIVO ***************/
