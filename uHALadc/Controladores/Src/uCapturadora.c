/**************************************************************************************************
 * Archivo: uCapturadora.c
 * Breve:
 * Fecha:
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "uCapturadora.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define U_LARGO_CAPTURA				50   // Total de muestras a capturar (incluye ...PRE_DISPARO)
#define U_MUESTRAS_PRE_DISPARO	25		// Muestras que guarda previo al disparo (trigger)
#define U_MUESTRAS_POS_DISPARO	((uint32_t)( U_LARGO_CAPTURA - U_MUESTRAS_PRE_DISPARO ))
#define U_MUESTRAS_DESCARTADAS	10

/****** Definiciones privadas de tipos de datos (public typedef) *********************************/

typedef enum {
	CAPTURADORA_NO_INICIALIZADA,
	CAPTURADORA_INACTIVA,
	CAPTURADORA_CAPTURANDO,
	CAPTURADORA_EN_ERROR,
} capturadora_estado_e;

typedef enum {
	ENTRADA_NO_INICIALIZADA,
	ENTRADA_APAGADA,
	ENTRADA_EN_ESPERA,
	ENTRADA_CAPTURANDO,
	ENTRADA_EN_ERROR,
} entrada_estado_e;

typedef struct {
	entrada_config_s	Config;
	entrada_estado_e	Estado;
} entrada_admin_s;

typedef struct {
	capturadora_config_s		Config;
	double						FrecuenciaMuestreo;
	capturadora_estado_e		Estado;
} capturadora_admin_s;

/****** Definición de datos privados **********************************************************/

capturadora_admin_s	Capturadora = {0};
entrada_admin_s		EntradaAdmin [U_ENTRADAS_CANTIDAD] = {0};
senial_s					SenialAdmin  [U_ENTRADAS_CANTIDAD] = {0};
uint32_t 				MuestrasCapturadas12 [U_LARGO_CAPTURA + U_MUESTRAS_DESCARTADAS] = {0};
uint32_t 				MuestrasProcesadas12 [U_LARGO_CAPTURA ] = {0};

/****** Definición de funciones públicas ********************************************************/

bool uCapturadoraInicializar 			( void )
{
	bool control = true;
	adc_config_s ConfigADC = {0};

	// Inicializamos módulo uHALadc:
	if (false == uHALadcInicializar ( UHAL_ADC_TODOS ) ) control = false;

	// Valores predeterminados de Capturadora
	Capturadora.Config.EscalaHorizontal = 25e-6;
	Capturadora.Config.EsperaDisparo = 500;
	Capturadora.Config.ModoCaptura = CAPTURA_UNICA;
	Capturadora.Config.OrigenDisparo = ENTRADA_1;
	Capturadora.FrecuenciaMuestreo = U_LARGO_CAPTURA / Capturadora.Config.EscalaHorizontal;

	// Valores predeterminados Entrada 1 (ADC 1 con dos canales)
	EntradaAdmin [ENTRADA_1].Config.EscalaVertical = 3.3;
	EntradaAdmin [ENTRADA_1].Config.NivelDisparo   = 1.5;
	EntradaAdmin [ENTRADA_1].Config.FlancoDisparo  = SUBIDA;
	EntradaAdmin [ENTRADA_1].Estado = ENTRADA_APAGADA;
	SenialAdmin [ENTRADA_1].Muestras_p   = MuestrasProcesadas12;
	SenialAdmin [ENTRADA_1].LargoMaximo  = U_LARGO_CAPTURA;
	SenialAdmin [ENTRADA_1].Alineacion   = DERECHA_32;
	SenialAdmin [ENTRADA_1].UltimaAccion = E_INICIALIZADA;

	// Configuramos escala y frecuencia de ADC 1
	ConfigADC.Canal = U_ADC_CANAL_1;					// Este canal determina la escala
	ConfigADC.FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
	if ( false == uHALadcConfigurar (UHAL_ADC_1,&ConfigADC) ) uHuboError();
	if ( false == uHALadcObtener    (UHAL_ADC_1,&ConfigADC) ) uHuboError();  // No es imprescindible...
	Capturadora.FrecuenciaMuestreo = ConfigADC.FrecuenciaMuestreo;  // Puede que haya modificado la frecuencia.

	// Valores predeterminados Entrada 2
	EntradaAdmin [ENTRADA_2].Config.EscalaVertical = 3.3;
	EntradaAdmin [ENTRADA_2].Config.NivelDisparo   = 1.5;
	EntradaAdmin [ENTRADA_2].Config.FlancoDisparo  = SUBIDA;
	EntradaAdmin [ENTRADA_2].Estado = ENTRADA_APAGADA;
   SenialAdmin [ENTRADA_2].Muestras_p   = MuestrasProcesadas12;
	SenialAdmin [ENTRADA_2].LargoMaximo  = U_LARGO_CAPTURA;
	SenialAdmin [ENTRADA_2].Alineacion   = IZQUIERDA_32;
	SenialAdmin [ENTRADA_2].UltimaAccion = E_INICIALIZADA;

	// Configuramos escala de ADC 2
	if ( false == uHALadcConfigurar (UHAL_ADC_2,&ConfigADC) ) uHuboError();

	// Cambiamos estado y salimos
	Capturadora.Estado = CAPTURADORA_INACTIVA;
	return control;
}

bool uCapturadoraEntradaConfigurar 	( entrada_id_e ID, entrada_config_s * CONFIG )
{
	// Variables locales
	bool control = false;

	// Verificación de parámetros
	if ( U_ENTRADAS_CANTIDAD < ID && ENTRADAS_TODAS!=ID) return false;
	//debemos verificar tiempo mínimo que podemos muestrear...

	// Configuramos...
	// ----------- ¿Debo configurar CANAL 1? -----------------------------------
	if ( ENTRADA_1==ID || ENTRADAS_TODAS==ID ) {
		// Canal [ID]
		control = true;
	}

	return control;
}

bool uCapturadoraEntradaObtener		( entrada_id_e ID, entrada_config_s * PCONFIG, senial_s * PSENIAL)
{
	return true;
}

bool uCapturadoraActualizar			( void)
// Verifica tareas pendientes y actua.
{
	return true;
}


bool uCapturadoraEntradaEncender		( entrada_id_e ID )
{
	// Variables locales
	bool control = false;
	// Verificación de parámetros
	if ( U_ENTRADAS_CANTIDAD < ID && ENTRADAS_TODAS!=ID) return false;
	// Encendemos
	EntradaAdmin[ID].Estado = ENTRADA_EN_ESPERA;
	EntradaAdmin[ID].Config.Encendida = true;
	control = true;
	// Salimos
	return control;
}

bool uCapturadoraEntradaApagar		( entrada_id_e ID )
{
	// Variables locales
	bool control = false;
	// Verificación de parámetros
	if ( U_ENTRADAS_CANTIDAD < ID && ENTRADAS_TODAS!=ID) return false;
	// Encendemos
	EntradaAdmin[ID].Estado = ENTRADA_APAGADA;
	EntradaAdmin[ID].Config.Encendida = false;
	control = true;
	// Salimos
	return control;
}

bool uCapturadoraIniciar			( void )
{
	bool control = false;

	// Verificamos estados de ambas entradas
	if ( EntradaAdmin[ENTRADA_1].Estado == ENTRADA_EN_ESPERA ) {
		EntradaAdmin[ENTRADA_1].Estado = ENTRADA_CAPTURANDO;
		control = true;
	}
	if ( EntradaAdmin[ENTRADA_2].Estado == ENTRADA_EN_ESPERA ) {
		EntradaAdmin[ENTRADA_2].Estado = ENTRADA_CAPTURANDO;
		control = true;
	}

	// Lanzamos muestreo
	if ( true == control ) {
		control = uHALadcComenzarLectura (	UHAL_ADC_1,					// Lanza muestreo en ADC 1 y 2
														MuestrasCapturadas12,	// Vector donde almaceno lo muestreado
														U_LARGO_CAPTURA + U_MUESTRAS_DESCARTADAS ); // Largo del vector
		uEscribirTexto ("Iniciamos captura... \n\r");
	}

	// Actualizamos estado general y salimos
	Capturadora.Estado = CAPTURADORA_CAPTURANDO;
	return control;
}

bool uCapturadoraParar				( void )
{
	bool control = false;

	// Verificamos estados de ambas entradas
	if ( EntradaAdmin[ENTRADA_1].Estado == ENTRADA_CAPTURANDO ) {
		EntradaAdmin[ENTRADA_1].Estado = ENTRADA_EN_ESPERA;
		control = true;
	}
	if ( EntradaAdmin[ENTRADA_2].Estado == ENTRADA_CAPTURANDO ) {
		EntradaAdmin[ENTRADA_2].Estado = ENTRADA_EN_ESPERA;
		control = true;
	}

	// Lanzamos muestreo
	if ( true == control ) {
		control = uHALadcPararLectura ( UHAL_ADC_1 );
		uEscribirTexto ("MSJ Paramos captura...\n\r");
	}

	// Actualizamos estado general y salimos
	Capturadora.Estado = CAPTURADORA_INACTIVA;
	return control;
}

/****** Definición de funciones privadas ********************************************************/



/****************************************************************** FIN DE ARCHIVO ***************/
