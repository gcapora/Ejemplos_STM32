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
#define U_LARGO_CAPTURA								100	// Total de muestras a capturar (incluye PRE_DISPARO y POS_DISPARO)

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
	BAJADA //,
	// AMBOS <--- Esto se implementará en otra etapa porque dede analizarse que hacer cuando se promedia
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

bool   	uCapturadoraInicializar			( void );							// Inicialización general.
bool   	uCapturadoraConfigurar			( capturadora_config_s * );	// Configuración de base de tiempo y disparo.
bool		uCapturadoraObtener				( capturadora_config_s * );
senial_s*uCapturadoraSenialObtener		( entrada_id_e );					// Obtener el puntero a la senial
double	uCapturadoraLeerFrecuenciaMuestreo ( void );
float		uCapturadoraLeerEscalaVertical( float );							// Devuelve la escala correspondiente
uint8_t	uCapturadoraLeerSincronizadas	( void );							// Devuelve la cantidad de capturas que se lograron sincronizar
uint32_t	uCapturadoraLeerTiempoCaptura	( void );

bool   	uCapturadoraEntradaConfigurar	( entrada_id_e, entrada_config_s * ); // Configuración de un canal.
bool   	uCapturadoraEntradaObtener		( entrada_id_e, entrada_config_s * );
bool   	uCapturadoraEntradaEncender	( entrada_id_e );					// Encendido y apagado de un canal.
bool   	uCapturadoraEntradaApagar		( entrada_id_e );

bool   	uCapturadoraIniciar				( void );							// Para que el inicio sea exitoso. Debe haber al menos un canal encendido.
bool   	uCapturadoraParar					( void );
bool   	uCapturadoraActualizar			( void ); 							// Verifica tareas pendientes y actua.
bool   	uCapturadoraSenialCargada    	( void ); 							// Indica si hay una señal cargada.

/*************************************************************************************************/
#endif /*  */
/****************************************************************** FIN DE ARCHIVO ***************/
