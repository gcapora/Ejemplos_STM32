/**************************************************************************************************
 * Archivo: uHALadc.h
 * Breve:
 * Fecha:
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_UCAPTURADORA_H_
#define ISPEL_UCAPTURADORA_H_

/****** Librerías (includes) *********************************************************************/

#include "uOSAL.h"
#include "uHAL.h"
#include "uSeniales.h"

/****** Definiciones públicas (macros) ***********************************************************/

#define U_ENTRADAS_CANTIDAD       				2
#define U_NUM_MAX_CAPTURAS_PROMEDIAR			16

#define CAPTURA_UNICA								0b00000001
#define CAPTURA_CONTINUA							0b00000010
#define CAPTURA_PROMEDIADA_4						0b01000000
#define CAPTURA_PROMEDIADA_16						0b10000000
#define MASCARA_CAPTURA								0b00000011
#define MASCARA_PROMEDIO							0b11000000

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	ENTRADA_1,
	ENTRADA_2,
	ENTRADAS_TODAS,
	MODO_ALTERNADO = ENTRADAS_TODAS,
	MODO_ASINCRONICO
} entrada_id_e;

typedef enum {
	SUBIDA,
	BAJA,
	AMBOS
} flanco_e;

typedef struct {
	float    				EscalaHorizontal;	// [s]  Intervalo a capturar y mostrar en pantalla.
														//      Este valor determina la frecuencia de muestreo
	entrada_id_e			OrigenDisparo;		//      Incluye Origen Modo Alternado y Modo Asincronico
	uint8_t					ModoCaptura;		//      Ver Definiciones públicas (macros)
	uint16_t 				EsperaDisparo;		// [ms] Tiempo máximo de espera del disparo
} capturadora_config_s;

typedef struct {
	float		  	  			EscalaVertical;	// [V]  Máxima tensión positiva
														//      Determina si la escala queda en 4 V o 12 V.
	float	   				NivelDisparo;
	flanco_e 				FlancoDisparo;
	//senial_s * 				Senial;			   // Donde almaceno mi señal.
	bool						Encendida;
	//entrada_estado_e		Estado;
} entrada_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool uCapturadoraInicializar			( void );							// Inicialización general.
bool uCapturadoraConfigurar			( capturadora_config_s * );	// Configuración de base de tiempo y disparo.
bool uCapturadoraObtener				( capturadora_config_s * );
bool uCapturadoraActualizar			( void); 							// Verifica tareas pendientes y actua.
bool uCapturadoraEntradaConfigurar	( entrada_id_e,					// Configuración de un canal.
												  entrada_config_s *,
												  senial_s * );					// Aquí debo recibir la señal);
bool uCapturadoraEntradaObtener		( entrada_id_e,
											  	  entrada_config_s *,
												  senial_s * );					// Aquí debo recibir la señal
bool uCapturadoraEntradaEncender		( entrada_id_e );					// Encendido y apagado de un canal.
bool uCapturadoraEntradaApagar		( entrada_id_e );
bool uCapturadoraIniciar				(void);								// Para que el inicio sea exitoso
																						// debe haber al menos un canal encendido.
bool uCapturadoraParar					(void);

/*************************************************************************************************/
#endif /*  */
/****************************************************************** FIN DE ARCHIVO ***************/
