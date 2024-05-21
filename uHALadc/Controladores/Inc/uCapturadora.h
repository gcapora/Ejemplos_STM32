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
#define U_LARGO_CAPTURA								200	// Total de muestras a capturar (incluye PRE_DISPARO y POS_DISPARO)

#define CAPTURA_UNICA								0b00000001
#define CAPTURA_CONTINUA							0b00000010  // Sin habilitar por ahora
#define CAPTURA_PROMEDIADA_4						0b01000000
#define CAPTURA_PROMEDIADA_16						0b10000000
#define MASCARA_CAPTURA								0b00000011
#define MASCARA_PROMEDIO							0b11000000
#define MASCARA_DERECHA16							0x0000FFFF
#define MASCARA_IZQUIERDA16						0xFFFF0000

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

typedef enum {
	ENTRADA_1,
	ENTRADA_2,
	ENTRADAS_TODAS,
	ORIGEN_ALTERNADO = ENTRADAS_TODAS,
	ORIGEN_ASINCRONICO
} entrada_id_e;

typedef enum {
	SUBIDA,
	BAJA,
	AMBOS
} flanco_e;

typedef struct {
	double    				EscalaHorizontal;	// [s]  Intervalo a capturar y mostrar en pantalla.
														//      Este valor determina la frecuencia de muestreo
	entrada_id_e			OrigenDisparo;		//      Incluye Origen Modo Alternado y Modo Asincronico
	uint8_t					ModoCaptura;		//      Ver Definiciones públicas (macros)
} capturadora_config_s;

typedef struct {
	float		  	  			EscalaVertical;	// [V]  Máxima tensión positiva, que se adecuará a escalas disponibles
	float	   				NivelDisparo;
	flanco_e 				FlancoDisparo;
	//senial_s * 				Senial;			   // Donde almaceno mi señal.
	bool						Encendida;
} entrada_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

bool   uCapturadoraInicializar		( void );							// Inicialización general.
bool   uCapturadoraConfigurar			( capturadora_config_s * );	// Configuración de base de tiempo y disparo.
bool   uCapturadoraObtener				( capturadora_config_s * );
double uCapturadoraLeerFrecuenciaMuestreo ( void );
bool   uCapturadoraEntradaConfigurar( entrada_id_e,					// Configuración de un canal.
												  entrada_config_s * );
bool   uCapturadoraEntradaObtener	( entrada_id_e,
											  	  entrada_config_s * );
bool   uCapturadoraSenialObtener    ( entrada_id_e,
											  	  senial_s * );
float  uCapturadoraEscalaVertical	( float );
bool   uCapturadoraEntradaEncender	( entrada_id_e );					// Encendido y apagado de un canal.
bool   uCapturadoraEntradaApagar		( entrada_id_e );
bool   uCapturadoraIniciar				( void );							// Para que el inicio sea exitoso
																						// debe haber al menos un canal encendido.
bool   uCapturadoraParar				( void );
bool   uCapturadoraActualizar			( void ); 							// Verifica tareas pendientes y actua.
bool   uCapturadoraSenialCargada    ( void ); 							// Indica si hay al menos una señal cargada.

/*************************************************************************************************/
#endif /*  */
/****************************************************************** FIN DE ARCHIVO ***************/
