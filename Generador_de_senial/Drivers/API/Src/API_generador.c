/*******************************************************************************
  * @file		API_generador.c
  * @brief      Maneja acciones de los estados del
  *             Generador de señales con entrada por UART
  * @author		Guillermo F. Caporaletti <gfcaporaletti@undav.edu.ar>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "API_generador.h"

/* Defines privados ----------------------------------------------------------*/
#define TIEMPO_ENCENDIDO_1	100		// <-- tiempos para parpadeo de leds
#define TIEMPO_ENCENDIDO_2	500
#define TIEMPO_PARPADEO_ESPERA		1000
#define TIEMPO_PARPADEO_CARGADO		750
#define TIEMPO_PARPADEO_ENCENDIDO	75

/* Private typedef -----------------------------------------------------------*/

// Cada generador tiene estado y senial almacenada
typedef struct {
	estadosMEF estado;
	bool encendido;			// Este bool es redundante
	bool cargado;			// Este bool es redundante
	uint32_t senial[N_MUESTRAS];
} generador_t;

/* Variables privadas USUARIO -------------------------------------------------*/
generador_t GeneradorDAC2;		// Estructura del generador
delay_t parpadeoLedAzul;		// Parpadeo en estados Cargado, Generando y Pausa
delay_t parpadeoLedVerde;		// Parpadeo en estados Espera y Recibiendo

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
  * @brief  Inicializa Generador
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Init(void) {
	GeneradorDAC2.cargado = false;
	GeneradorDAC2.encendido = false;
	GeneradorDAC2.estado = Espera;
	BSP_LED_Init(LED_BLUE);			// Indicador en estados Cargado en adelante
	BSP_LED_Init(LED_GREEN);		// Indicador en estados Espera y Recibiendo
	delayInit( &parpadeoLedAzul, TIEMPO_PARPADEO_CARGADO);
	delayInit( &parpadeoLedVerde, TIEMPO_PARPADEO_ESPERA);
}

/*******************************************************************************
  * @brief  Establece al generador en espera
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Espera(void) {
	// Apago led azul     = señal cargada
	// Parpadeo led verde = en espera
	BSP_LED_Off(LED_BLUE);
	delayWrite( &parpadeoLedVerde, TIEMPO_PARPADEO_ESPERA );

	// Actualizo estructura
	GeneradorDAC2.cargado = false;
	GeneradorDAC2.estado = Espera;

	// Finalmente corto señal del generador
	Parar_DAC_DMA();

	// Y envío informe a UART
    uartClearBuffer();
    uartSendString((uint8_t *) "Generador vacio y en espera...\n");
}

/*******************************************************************************
  * @brief  Pasa a estado de receción de señal
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Recibir(void) {
	GeneradorDAC2.estado = Recibiendo;
	BSP_LED_On(LED_GREEN);
    uartSendString((uint8_t *) "Listo para recibir...\n");
    uartClearBuffer();
}

/*******************************************************************************
  * @brief  Carga señal en el generador
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Cargar(uint32_t Senial[]) {
	// Chequeo que la Senial tenga todos los datos que necesito
	for (uint16_t i=0; i<N_MUESTRAS; i++) {
		// Verifico que Senial[] sea acorde a 12 bits
		if (Senial[i] > 0x0FFF) Error_Handler();
	}

	// Copio valores
	for (uint16_t i=0; i<N_MUESTRAS; i++) GeneradorDAC2.senial[i] = Senial[i];

	// Actualizo el estado
	GeneradorDAC2.cargado = true;
	GeneradorDAC2.estado = Cargado;

	// Informo con leds y por UART
	BSP_LED_Off(LED_GREEN);
	delayWrite( &parpadeoLedAzul, TIEMPO_PARPADEO_CARGADO );
    uartClearBuffer();
    uartSendString((uint8_t *) "Senial cargada en generador.\n");
}

/*******************************************************************************
  * @brief  Enciende el generador
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Encender(void) {
	// Acelero el parpadeo del led azul
	delayWrite( &parpadeoLedAzul, TIEMPO_PARPADEO_ENCENDIDO );

	// Actualizo estructura
	GeneradorDAC2.estado = Generando;
	GeneradorDAC2.encendido = true;

	// Enciendo generador
	Comenzar_DAC_DMA(GeneradorDAC2.senial, N_MUESTRAS);

	// Reinicializo índice de carga
	//MuestraNro = 0;

	// Muestro mensaje de esto.
	uartSendString((uint8_t *) "Generador 2 encendido.\n\r");

}

/*******************************************************************************
  * @brief  Pone pausa al generador (sale de pausa con boton de usuario)
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Pausar(void) {

	GeneradorDAC2.estado = Pausa;
	GeneradorDAC2.encendido = false;

	Parar_DAC_DMA();

	uartSendString((uint8_t *) "Generador 2 en pausa.\n\r");
	delayWrite( &parpadeoLedAzul, TIEMPO_PARPADEO_CARGADO );
}

/*******************************************************************************
  * @brief  Devuelve el estado del generador
  * @param  Estructura de datos del generador
  * @retval estado del generador
  */
estadosMEF Gen_Estado(void) {
	return GeneradorDAC2.estado;
}

/*******************************************************************************
  * @brief  Actualiza leds según estado del generador
  * @param  Estructura de datos del generador
  * @retval None
  */
void Gen_Actualiza_Leds(void) {
	// Analizo parpadeo LED VERDE:
	if ( (Gen_Estado() == Espera) && delayRead( &parpadeoLedVerde )) BSP_LED_Toggle(LED_GREEN);

	// Analizo parpadeo LED AZUL:
	if ( (Gen_Estado() >= Cargado) && delayRead( &parpadeoLedAzul )) BSP_LED_Toggle(LED_BLUE);
}



