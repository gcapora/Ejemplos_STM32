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
const tick_t ventanaAntirrebote = 40;		// El tiempo que debe pasar (en milisegundos)
											// para validar un cambio de estado del botón.

/* Private variables ---------------------------------------------------------*/
static debounceState_t estadoActual = BUTTON_UP;	// Estado del boton
static delay_t debounceDelay;						// Estructura para el retardo antirrebote
static bool_t FlancoAscendente = false;
static bool_t FlancoDescendente = false;

/* Private function prototypes -----------------------------------------------*/
static void buttonPressed();			// Acción de salida = togglear el LED1
static void buttonReleased();			// Acción de salida = togglear el LED3

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* @brief  Me devuelve si hubo flanco ascendente y se resetea.
* @param  None
* @retval None
*/
bool_t readKey() {
	bool_t salida = FlancoAscendente;
	if (FlancoAscendente == true) {
		// Reseteo registro de flanco ascendente
		FlancoAscendente = false;
	}
	FlancoDescendente = false;
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
	// Initialize BSP PB for BUTTON_USER
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
	// También inicializo los leds que uso dentro del módulo
	BSP_LED_Init(LED1);
	BSP_LED_Init(LED3);
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
			if (LecturaBoton == false) {
				// Debo cambiar de estado...
				estadoActual = BUTTON_FALLING;
		}
		break;

		case BUTTON_FALLING:
			if (delayRead(&debounceDelay) == true) {
				// Terminó el retardo y por lo tanto
				// debo evaluar cambio de estado...
				if (LecturaBoton == true) {
					// ...en definitiva fue un falso descenso
					estadoActual = BUTTON_UP;
				} else {
					// Hay un cambio de estado!!!
					estadoActual = BUTTON_DOWN;
					buttonReleased();	// Acción de flanco descendente.
					FlancoDescendente = true;	// Dejo registrado que hubo flanco descendente
				}
			}
		break;

		case BUTTON_DOWN:
			if (LecturaBoton == true) {
				// Debo cambiar de estado...
				estadoActual = BUTTON_RISING;
			}
		break;

		case BUTTON_RISING:
			if (delayRead(&debounceDelay) == true) {
				// Terminó el retardo y por lo tanto
				// debo evaluar cambio de estado...
				if (LecturaBoton == false) {
					// ...en definitiva fue un falso descenso
					estadoActual = BUTTON_DOWN;
				} else {
					// Hay un cambio de estado!!!
					estadoActual = BUTTON_UP;
					buttonPressed();	// Acción de flanco ascendente.
					FlancoAscendente = true;	// Dejo registrado que hubo flanco ascendente
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
	  BSP_LED_Toggle(LED1);
}

/*******************************************************************************
* @brief  Acción de flanco descendente: cambia LED3.
* @param  None
* @retval None
*/
void buttonReleased()  {
	  BSP_LED_Toggle(LED3);
}

/***************************************************************END OF FILE****/
