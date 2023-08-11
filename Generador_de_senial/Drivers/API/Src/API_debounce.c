/*******************************************************************************
* @file    API_debounce.c
* @author  Guillermo Caporaletti
* @brief   CESE_Co18 - Práctica 4 - Punto 2:
*          Implementación de módulo anti-rebote de pulsador.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "API_debounce.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RISING
} debounceState_t;		// Estados posibles de mi MEF antirrebote

/* Private define y const ----------------------------------------------------*/
const tick_t ventanaAntirrebote = 30;		// El tiempo que debe pasar (en milisegundos)
											// para validar un cambio de estado del botón.
const tick_t ventanaPresionadoLargo = 500; 	// Tiempo para considerar que hubo una presión larga

/* Private variables ---------------------------------------------------------*/
static debounceState_t estadoActual = BUTTON_UP;	// Estado del boton
static delay_t debounceDelay;						// Estructura para el retardo antirrebote
static delay_t retardoLargo;						// Estructura para verificar presionado largo
static bool_t FlancoAscendente = false;
static bool_t FlancoDescendente = false;
static bool_t PresionadoLargo = false;

/* Private function prototypes -----------------------------------------------*/
static void buttonPressed();			// Acción de salida = togglear el LED1
static void buttonReleased();			// Acción de salida = togglear el LED3

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* @brief  Me devuelve si hubo flanco ascendente y se resetea.
* @param  None
* @retval None
*/
bool_t readKeyRelease() {
	bool_t salida = FlancoDescendente;
	if (FlancoDescendente == true) {
		// Reseteo registro de flancos
		FlancoAscendente = false;
		FlancoDescendente = false;
	}
	return salida;
}

/*******************************************************************************
* @brief  Me devuelve si hubo flanco descendente y se resetea.
* @param  None
* @retval None
*/
bool_t readKeyPush() {
	bool_t salida = FlancoAscendente;
	if (FlancoAscendente == true) {
		// Reseteo registro de flancos
		FlancoAscendente = false;
		FlancoDescendente = false;
	}
	return salida;
}

bool_t readPresionadoLargo() {
	bool_t salida = PresionadoLargo;
	PresionadoLargo = false;
	return salida;
}

/*******************************************************************************
* @brief  Inicializa MEF antirrebote en BUTTON_UP
* @param  None
* @retval None
*/
void debounceFSM_init() {
	// Inicializo estado, flanco y retardo de validación
	estadoActual = BUTTON_UP;
	FlancoAscendente = false;
	FlancoDescendente = false;
	delayInit(&debounceDelay, ventanaAntirrebote);
	delayInit(&retardoLargo, ventanaPresionadoLargo);
	// Initialize BSP PB for BUTTON_USER
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
	// También inicializo los leds que uso dentro del módulo
	//BSP_LED_Init(LED1);
	//BSP_LED_Init(LED3);
}

/*******************************************************************************
* @brief  Actualiza MEF antirrebote:
*         Lee las entradas, resuelve la lógica de transición
*         y actualizar las salidas.
* @param  None
* @retval None
*/
void debounceFSM_update() {
	bool LecturaBoton = BSP_PB_GetState(BUTTON_USER);

	switch (estadoActual) { // -------------------------------------------------

		case BUTTON_UP:
			if (LecturaBoton == true) {
				// Debo cambiar de estado...
				estadoActual = BUTTON_FALLING;
		}
		break;

		case BUTTON_FALLING:
			if (delayRead(&debounceDelay) == true) {
				// Terminó el retardo y por lo tanto
				// debo evaluar cambio de estado...
				if (LecturaBoton == false) {
					// ...en definitiva fue un falso descenso
					estadoActual = BUTTON_UP;
				} else {
					// Hay un cambio de estado!!!
					estadoActual = BUTTON_DOWN;
					buttonPressed();	// Acción de flanco ascendente de señal.
				}
			}
		break;

		case BUTTON_DOWN:
			if (LecturaBoton == false) {
				// Debo cambiar de estado...
				estadoActual = BUTTON_RISING;
			}
		break;

		case BUTTON_RISING:
			if (delayRead(&debounceDelay) == true) {
				// Terminó el retardo y por lo tanto
				// debo evaluar cambio de estado...
				if (LecturaBoton == true) {
					// ...en definitiva fue un falso ascenso
					estadoActual = BUTTON_DOWN;
				} else {
					// Hay un cambio de estado!!!
					estadoActual = BUTTON_UP;
					buttonReleased();	// Acción de flanco descendente de señal (de 1 a 0).
				}
			}
		break;

		default:
			Error_Handler();
		break;
	} // -------------------------- Fin del switch ----------------------------
}

/*******************************************************************************
* @brief  Acción de flanco ascendente: cambia LED1.
* @param  None
* @retval None
*/
void buttonPressed() {
	//BSP_LED_Toggle(LED1);
	FlancoAscendente = true;	// Dejo registrado que hubo flanco ascendente en la señal (de 0 a 1)
	PresionadoLargo = false;
	delayRead(&retardoLargo);	// En realidad, no leo: lo hago comenzar a contar
}

/*******************************************************************************
* @brief  Acción de flanco descendente: cambia LED3.
* @param  None
* @retval None
*/
void buttonReleased()  {
	// BSP_LED_Toggle(LED3);
	FlancoDescendente = true;	// Dejo registrado que hubo flanco descendente en la señal (de 1 a 0)
	if (delayRead(&retardoLargo) == true) PresionadoLargo = true;
	delayReset(&retardoLargo);
}

/***************************************************************END OF FILE****/
