/**************************************************************************************************
* Archivo: uCapturadora.c
* Breve:
* Fecha:
* Autor:	Guillermo F. Caporaletti
*
**************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "uCapturadora.h"

/****** Definiciones privadas (macros) ***********************************************************/

#ifndef U_LARGO_CAPTURA
#define U_LARGO_CAPTURA				100
#endif
#define U_MUESTRAS_PRE_DISPARO		((uint32_t)( U_LARGO_CAPTURA / 4 ))	// Muestras que guarda previo al disparo (trigger)
#define U_MUESTRAS_POS_DISPARO		((uint32_t)( U_LARGO_CAPTURA - U_MUESTRAS_PRE_DISPARO ))
#define U_MUESTRAS_DESCARTADAS		10                                	// Muestras iniciales que dan valores erróneos
#define U_SOBREMUESTREO          	((uint32_t)( U_LARGO_CAPTURA ))
#define U_LARGO_CAPTURA_INICIAL 	 	((uint32_t)( U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA + U_SOBREMUESTREO ))
#define U_ESCALA_HORIZONTAL_MINIMA	((double) U_LARGO_CAPTURA / 2.4e6)	// 2.4e6 es lo máximo que admite la placa
#define U_TIEMPO_CAPTURA_MAXIMO		((uint32_t) 1000)                   // [ms] Pasado este tiempo, envío lo que tenga.
																								// Además de este tiempo, se tarda en procesar y enviar.

/****** Definiciones privadas de tipos de datos (public typedef) *********************************/

typedef enum {
	CAPTURADORA_NO_INICIALIZADA,
	CAPTURADORA_INACTIVA,
	CAPTURADORA_CAPTURANDO,
	CAPTURADORA_PROCESANDO,
	CAPTURADORA_CAPTURA_COMPLETADA,
	CAPTURADORA_EN_ERROR,
} capturadora_estado_e;

typedef enum {
	ENTRADA_NO_INICIALIZADA,
	ENTRADA_APAGADA,
	ENTRADA_ENCENDIDA,
	ENTRADA_EN_ERROR,
} entrada_estado_e;

typedef struct {
	entrada_config_s				Config;
	entrada_estado_e				Estado;
} entrada_admin_s;

typedef struct {
	capturadora_config_s			Config;
	double							FrecuenciaMuestreo;
	uint16_t							NivelDisparo;	// Debe traducir el nivel desde fuente
	uint16_t							Histeresis;		// Se configura en u...Inicializar y no varía por el momento
	flanco_e 						FlancoDisparo;
	uint8_t							CapturasRestantes;
	uint32_t							TiempoInicio;	// [ms] Tiempo inicial de la captura
	uint32_t							TiempoCaptura;
	capturadora_estado_e			Estado;
} capturadora_admin_s;

typedef enum {
	ESCALA_VERTICAL_1,
	ESCALA_VERTICAL_2,
	ESCALA_VERTICAL_MAXIMA = ESCALA_VERTICAL_2,
	ESCALAS_VERTICALES_CANTIDAD,
	ESCALA_VERTICAL_DESBORDADA
} escala_vertical_e;

/****** Definición de datos privados **********************************************************/

static capturadora_admin_s	Capturadora = {0};
static entrada_admin_s		EntradaAdmin [U_ENTRADAS_CANTIDAD] = {0};
static senial_s				SenialAdmin  [U_ENTRADAS_CANTIDAD] = {0};
static uint32_t 				MuestrasCapturadas12 [U_LARGO_CAPTURA_INICIAL] = {0};
static uint32_t 				MuestrasProcesadas12 [U_LARGO_CAPTURA] = {0};
static uint8_t					CantidadProcesadas12 [U_LARGO_CAPTURA] = {0};
volatile	bool	LecturaCompletada = false;
const float		EscalasVerticales [ESCALAS_VERTICALES_CANTIDAD] = {3.3, 6.6};

/****** Declaración de funciones privadas ********************************************************/

bool		SumarSenial (void);
void 		ProcesarSenial (void);
void 		ImprimirSenial32 (void);
bool		ValidarOrigenDisparo ( entrada_id_e * );
uint8_t	CapturasObjetivo ( void );
escala_vertical_e EscalaVerticalId ( float ); // Devuelve el índice correspondiente al valor de escala vertical

/****** Definición de funciones públicas ********************************************************/

bool uCapturadoraInicializar ( void )
{
	bool control = true;
	adc_config_s ConfigADC = {0};

	// Precondiciones.
	if ( CAPTURADORA_NO_INICIALIZADA != Capturadora.Estado) uoHuboErrorTxt("Se intento reinicializar Capturadora.");

	// Inicializamos módulo uHALadc:
	if (false == uHALadcInicializar ( UHAL_ADC_TODOS ) ) control = false;

	// Valores predeterminados de Capturadora
	Capturadora.Config.EscalaHorizontal = 25e-6;
	Capturadora.Config.ModoCaptura = CAPTURA_UNICA | CAPTURA_PROMEDIADA_16;
	Capturadora.Config.OrigenDisparo = ENTRADA_1;
	Capturadora.FrecuenciaMuestreo = U_LARGO_CAPTURA / Capturadora.Config.EscalaHorizontal;
	Capturadora.NivelDisparo = 2000;
	Capturadora.Histeresis = 25;
	Capturadora.CapturasRestantes = 0;

	// Valores predeterminados Entrada 1 (ADC 1 con dos canales)
	EntradaAdmin [ENTRADA_1].Config.EscalaVertical = EscalasVerticales [ESCALA_VERTICAL_2];
	EntradaAdmin [ENTRADA_1].Config.NivelDisparo   = EscalasVerticales [ESCALA_VERTICAL_2]/2;
	EntradaAdmin [ENTRADA_1].Config.FlancoDisparo  = SUBIDA;
	EntradaAdmin [ENTRADA_1].Estado = ENTRADA_APAGADA;
	SenialAdmin  [ENTRADA_1].Muestras_p   = MuestrasProcesadas12;
	SenialAdmin  [ENTRADA_1].Inicio		  = 0;
	SenialAdmin  [ENTRADA_1].LargoMaximo  = U_LARGO_CAPTURA;
	SenialAdmin  [ENTRADA_1].Largo        = U_LARGO_CAPTURA;
	SenialAdmin  [ENTRADA_1].Alineacion   = DERECHA_32;
	SenialAdmin  [ENTRADA_1].UltimaAccion = E_INICIALIZADA;

	// Configuramos escala y frecuencia de ADC 1
	ConfigADC.Canal = U_ADC_CANAL_1;					// Este canal determina la escala
	ConfigADC.FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
	if ( false == uHALadcConfigurar (UHAL_ADC_1,&ConfigADC) ) uoHuboErrorTxt("inicialiando Capturadora (ADC1).");
	if ( false == uHALadcObtener    (UHAL_ADC_1,&ConfigADC) ) uoHuboErrorTxt("inicialiando Capturadora (ADC1).");
	Capturadora.FrecuenciaMuestreo = ConfigADC.FrecuenciaMuestreo;  // Puede que haya modificado la frecuencia.

	// Valores predeterminados Entrada 2
	EntradaAdmin [ENTRADA_2].Config.EscalaVertical = EscalasVerticales [ESCALA_VERTICAL_2];
	EntradaAdmin [ENTRADA_2].Config.NivelDisparo   = EscalasVerticales [ESCALA_VERTICAL_2]/2;
	EntradaAdmin [ENTRADA_2].Config.FlancoDisparo  = SUBIDA;
	EntradaAdmin [ENTRADA_2].Estado = ENTRADA_APAGADA;
   SenialAdmin  [ENTRADA_2].Muestras_p   = MuestrasProcesadas12;
   SenialAdmin  [ENTRADA_2].Inicio		  = 0;
	SenialAdmin  [ENTRADA_2].LargoMaximo  = U_LARGO_CAPTURA;
	SenialAdmin  [ENTRADA_2].Largo        = U_LARGO_CAPTURA;
	SenialAdmin  [ENTRADA_2].Alineacion   = IZQUIERDA_32;
	SenialAdmin  [ENTRADA_2].UltimaAccion = E_INICIALIZADA;

	// Configuramos escala de ADC 2
	if ( false == uHALadcConfigurar (UHAL_ADC_2,&ConfigADC) ) uoHuboErrorTxt("inicializando Capturadora (ADC2)");

	// Cambiamos estado y salimos
	Capturadora.Estado = CAPTURADORA_INACTIVA;
	return control;
}

bool uCapturadoraConfigurar ( capturadora_config_s * CONFIG)
{
	// Variables locales
	adc_config_s ConfigADC = {0};

	// Precondiciones y correcciones
	if ( CAPTURADORA_INACTIVA != Capturadora.Estado) {
		uoEscribirTxt("ADVERTENCIA Se intento configurar Capturadora estando activa.");
		return false;
	}
	if ( CONFIG->EscalaHorizontal < U_ESCALA_HORIZONTAL_MINIMA ) {
		CONFIG->EscalaHorizontal = U_ESCALA_HORIZONTAL_MINIMA;
		uoEscribirTxt ("ADVERTENCIA Correccion de ESCALA HORIZONTAL de capturadora.\n\r");
	}
	// TODO Verificar escala máxima posible.
	if ( false==ValidarOrigenDisparo(&CONFIG->OrigenDisparo) ) {
		uoEscribirTxt ("ADVERTENCIA Correccion de ORIGEN de disparo.\n\r");
	}
	CONFIG->ModoCaptura = CAPTURA_UNICA | (CONFIG->ModoCaptura&MASCARA_PROMEDIO);

	// Actualizamos estructura
	Capturadora.Config.EscalaHorizontal = CONFIG->EscalaHorizontal;
	Capturadora.Config.ModoCaptura      = CONFIG->ModoCaptura;
	Capturadora.Config.OrigenDisparo    = CONFIG->OrigenDisparo;

	// Capturadora.Config.EsperaDisparo = 500;       ---> Por ahora no variaremos este parámetro
	// Capturadora.NivelDisparo = 2000;              ---> idem...

	// Actualizamos frecuencia de muestreo
	if ( false == uHALadcObtener    (UHAL_ADC_1,&ConfigADC) ) uoHuboErrorTxt("en Configurar Capturadora (1).");
	ConfigADC.FrecuenciaMuestreo = (double) U_LARGO_CAPTURA / Capturadora.Config.EscalaHorizontal;
	if ( false == uHALadcConfigurar (UHAL_ADC_1,&ConfigADC) ) uoHuboErrorTxt("en Configurar Capturadora (2).");
	Capturadora.FrecuenciaMuestreo = ConfigADC.FrecuenciaMuestreo;  // Puede que se haya modificado la frecuencia.
	Capturadora.Config.EscalaHorizontal = U_LARGO_CAPTURA / Capturadora.FrecuenciaMuestreo;
	CONFIG->EscalaHorizontal = Capturadora.Config.EscalaHorizontal;

	// Salida
	return true;
}

bool uCapturadoraObtener ( capturadora_config_s * CONFIG)
{
	CONFIG->EscalaHorizontal = Capturadora.Config.EscalaHorizontal;
	CONFIG->ModoCaptura      = Capturadora.Config.ModoCaptura;
	CONFIG->OrigenDisparo    = Capturadora.Config.OrigenDisparo;
	return true;
}

double uCapturadoraLeerFrecuenciaMuestreo ( void )
{
	return Capturadora.FrecuenciaMuestreo;
}

uint8_t	uCapturadoraLeerSincronizadas	( void )
{
	return (uint8_t) (CapturasObjetivo() - Capturadora.CapturasRestantes);
}

uint32_t	uCapturadoraLeerTiempoCaptura	( void )
{
	return Capturadora.TiempoCaptura;
}

bool uCapturadoraEntradaConfigurar 	( entrada_id_e ID, entrada_config_s * PCONFIG )
{
	// Variables locales
	bool 				control   = false;
	adc_config_s	ConfigADC = {0};

	// Verificación de parámetros y recursividad
	if ( ENTRADAS_TODAS==ID ) {
		if (uCapturadoraEntradaConfigurar ( ENTRADA_1, PCONFIG )) control = true;
		if (uCapturadoraEntradaConfigurar ( ENTRADA_2, PCONFIG )) control = true;
		return control;
	}
	if ( U_ENTRADAS_CANTIDAD < ID ) return false;

	// Configuramos estructura
	EntradaAdmin[ID].Config.EscalaVertical  = uCapturadoraLeerEscalaVertical(PCONFIG->EscalaVertical);
	EntradaAdmin[ID].Config.NivelDisparo    = PCONFIG->NivelDisparo;
	if ( EntradaAdmin[ID].Config.NivelDisparo > EntradaAdmin[ID].Config.EscalaVertical )
		EntradaAdmin[ID].Config.NivelDisparo = EntradaAdmin[ID].Config.EscalaVertical;
	if ( EntradaAdmin[ID].Config.NivelDisparo < 0 )
		EntradaAdmin[ID].Config.NivelDisparo = 0;  // TODO corregir con escala inferior negativa
	EntradaAdmin[ID].Config.FlancoDisparo   = PCONFIG->FlancoDisparo;
	EntradaAdmin[ID].Config.Encendida       = PCONFIG->Encendida;

	// Configuramos ADC
	switch (	EscalaVerticalId (EntradaAdmin[ID].Config.EscalaVertical) )
	{
		case ESCALA_VERTICAL_1:
			ConfigADC.Canal = U_ADC_CANAL_1;
			break;
		case ESCALA_VERTICAL_2:
			ConfigADC.Canal = U_ADC_CANAL_2;
			break;
		default:
			uoHuboErrorTxt("configurando Entrada.");
	}
	ConfigADC.FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
	if ( false == uHALadcConfigurar ( (adc_id_e)ID, &ConfigADC) ) uoHuboErrorTxt("configurando ADC.");
	Capturadora.FrecuenciaMuestreo = ConfigADC.FrecuenciaMuestreo;

	// Copiamos configuración establecida y salimos:
	control = uCapturadoraEntradaObtener( ID, PCONFIG);
	return control;
}

bool uCapturadoraEntradaObtener ( entrada_id_e ID, entrada_config_s * PCONFIG)
{
	if (ID < U_ENTRADAS_CANTIDAD) {
		PCONFIG->EscalaVertical	= EntradaAdmin[ID].Config.EscalaVertical;
		PCONFIG->NivelDisparo	= EntradaAdmin[ID].Config.NivelDisparo;
		PCONFIG->FlancoDisparo	= EntradaAdmin[ID].Config.FlancoDisparo;
		PCONFIG->Encendida		= EntradaAdmin[ID].Config.Encendida;
		return true;
	} else {
		return false;
	}
}

senial_s * uCapturadoraSenialObtener ( entrada_id_e ID )
{
	return &SenialAdmin[ID];
}

float  uCapturadoraLeerEscalaVertical	( float VOLTIOS )
{
	escala_vertical_e i=ESCALA_VERTICAL_1;
	float retorno = EscalasVerticales[i];
	while ( (retorno<VOLTIOS) && (i<ESCALA_VERTICAL_MAXIMA) ) {
		i++;
		retorno = EscalasVerticales[i];
	}
	return retorno;
}

bool uCapturadoraEntradaEncender	( entrada_id_e ID )
{
	// Variables locales
	bool control = false;
	// Verificación de parámetros
	if ( U_ENTRADAS_CANTIDAD < ID && ENTRADAS_TODAS!=ID) return false;
	if ( Capturadora.Estado != CAPTURADORA_INACTIVA ) return false;
	// Encendemos
	EntradaAdmin[ID].Estado = ENTRADA_ENCENDIDA;
	EntradaAdmin[ID].Config.Encendida = true;
	control = true;
	// Salimos
	return control;
}

bool uCapturadoraEntradaApagar ( entrada_id_e ID )
{
	// Variables locales
	bool control = false;
	// Verificación de parámetros
	if ( U_ENTRADAS_CANTIDAD < ID && ENTRADAS_TODAS!=ID) return false;
	if ( Capturadora.Estado != CAPTURADORA_INACTIVA ) return false;
	// Apagamos
	EntradaAdmin[ID].Estado = ENTRADA_APAGADA;
	EntradaAdmin[ID].Config.Encendida = false;
	control = true;
	// Salimos
	return control;
}

bool uCapturadoraIniciar ( void )
{
	/* Variables locales */
	bool 				control = false;
	entrada_id_e	OrigenDisparo = Capturadora.Config.OrigenDisparo;
	uint32_t 		i;

	/* Precondiciones */
	if ( CAPTURADORA_INACTIVA != Capturadora.Estado ) {
		return false;
	}
	if ( OrigenDisparo <= ENTRADA_2                            &&
		EntradaAdmin[OrigenDisparo].Estado == ENTRADA_ENCENDIDA    ) {
		control = true;
	} else if ( EntradaAdmin[ENTRADA_1].Estado == ENTRADA_ENCENDIDA ||
			      EntradaAdmin[ENTRADA_2].Estado == ENTRADA_ENCENDIDA  ) {
		control = true;
	}
	if (false==control) return control;

	/* Configuramos parámetros y lanzamos muestreo */
	if ( OrigenDisparo <= ENTRADA_2 ) {
		Capturadora.NivelDisparo  = EntradaAdmin[OrigenDisparo].Config.NivelDisparo   /
											 EntradaAdmin[OrigenDisparo].Config.EscalaVertical *
											 MAXIMO_12B;
		Capturadora.FlancoDisparo = EntradaAdmin[OrigenDisparo].Config.FlancoDisparo;
	}
	Capturadora.CapturasRestantes = CapturasObjetivo();
	for (i=0; i<U_LARGO_CAPTURA; i++) {
		MuestrasProcesadas12 [i] = 0;
		CantidadProcesadas12 [i] = 0;
	}
	if ( true == control ) {
		control = uHALadcComenzarLectura (	UHAL_ADC_1,						// Lanza muestreo en ADC 1 y 2
														MuestrasCapturadas12,		// Vector donde almaceno lo muestreado
														U_LARGO_CAPTURA_INICIAL );	// Largo del vector
		uoLedEncender ( UOSAL_PIN_LED_AZUL_INCORPORADO );
	}

	// Actualizamos estado general y salimos
	Capturadora.TiempoInicio = uoMilisegundos();
	Capturadora.Estado = CAPTURADORA_CAPTURANDO;
	return control;
}

bool uCapturadoraParar ( void )
{
	// TODO Esta funcion no analiza los estados de la Capturadora.
	bool control = false;

	// Verificamos estados de ambas entradas
	if ( EntradaAdmin[ENTRADA_1].Estado == ENTRADA_ENCENDIDA ) {
		control = true;
	}
	if ( EntradaAdmin[ENTRADA_2].Estado == ENTRADA_ENCENDIDA ) {
		control = true;
	}

	// Paramos muestreo
	if ( true == control && Capturadora.Estado == CAPTURADORA_CAPTURANDO ) {
		control = uHALadcPararLectura ( UHAL_ADC_1 );
		uoEscribirTxt ("MSJ Paramos captura...\n\r");
		Capturadora.Estado = CAPTURADORA_INACTIVA;
	}

	// Salimos
	return control;
}

/**------------------------------------------------------------------------------------------------
  * @brief	Verifica tareas pendientes de Capturadora y actua.
  * @param	Ninguno
  * @retval	true si está en estado CAPTURADORA_CAPTURANDO.
  * 			Si no está en ese estado, no hay nada para actualizar.
  */
bool uCapturadoraActualizar ( void)
{
	// Variables locales -------------------------------------------------------
	uint32_t Delta, i;
	// Precondiciones ----------------------------------------------------------
	if ( Capturadora.Estado != CAPTURADORA_CAPTURANDO ) return false;
	// Actualizamos ------------------------------------------------------------
	if (true == LecturaCompletada) {
		LecturaCompletada = false;
		// Actualizamos estado y sumamos lo capturado ---------------------------
		Capturadora.Estado = CAPTURADORA_PROCESANDO;
		if ( true == SumarSenial() ) {
			Capturadora.CapturasRestantes--;
			uoLedApagar ( UOSAL_PIN_LED_VERDE_INCORPORADO );
		}
		if (Capturadora.CapturasRestantes > 16) {
			uoHuboErrorTxt ("en Actualizar Capturadora.");
		}
		// Analizamos si volvemos a capturar o procesamos -----------------------
		Delta = uoMilisegundos() - Capturadora.TiempoInicio;
		if ( (Capturadora.CapturasRestantes>0) &&
			  (Delta<U_TIEMPO_CAPTURA_MAXIMO)    ) {
			// Iniciamos una nueva captura para sumar al promedio:
			uHALadcComenzarLectura (	UHAL_ADC_1,						// Lanza muestreo en ADC 1 y 2
												MuestrasCapturadas12,		// Vector donde almaceno lo muestreado
												U_LARGO_CAPTURA_INICIAL );	// Largo del vector
			Capturadora.Estado = CAPTURADORA_CAPTURANDO;
		} else {
			// Paramos capturas y proceso resultado ------------------------------
			if ( Capturadora.CapturasRestantes == CapturasObjetivo()) {
				// No pudimos hacer ninguna captura sincronizada.
				// Cargamos lo que hay antes de procesar.
				for ( i=U_MUESTRAS_DESCARTADAS; i<U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA; i++) {
					MuestrasProcesadas12 [i-U_MUESTRAS_DESCARTADAS] = MuestrasCapturadas12[i];
					CantidadProcesadas12 [i-U_MUESTRAS_DESCARTADAS] = 1;
				}
			}
			// Procesamos lo que hay ---------------------------------------------
			ProcesarSenial();
			if ( ORIGEN_ASINCRONICO == Capturadora.Config.OrigenDisparo ||
				  Capturadora.CapturasRestantes == CapturasObjetivo()     ) {
				SenialAdmin[ENTRADA_1].ReferenciaT0 = 0;
				SenialAdmin[ENTRADA_2].ReferenciaT0 = 0;
			} else if ( ENTRADA_2 >= Capturadora.Config.OrigenDisparo ) {
				SenialAdmin[ENTRADA_1].ReferenciaT0 = U_MUESTRAS_PRE_DISPARO;
				SenialAdmin[ENTRADA_2].ReferenciaT0 = U_MUESTRAS_PRE_DISPARO;
			}
			SenialAdmin[ENTRADA_1].FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
			SenialAdmin[ENTRADA_2].FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
			Capturadora.TiempoCaptura = uoMilisegundos() - Capturadora.TiempoInicio;
			Capturadora.Estado = CAPTURADORA_CAPTURA_COMPLETADA;
		}
	}
	return true;
}

bool uCapturadoraSenialCargada ( void )
// Indica si hay una señal cargada.
{
	bool retorno = false;
	uCapturadoraActualizar();
	if ( CAPTURADORA_CAPTURA_COMPLETADA == Capturadora.Estado ) {
		retorno = true;
		// ImprimirSenial32();
		uoLedApagar ( UOSAL_PIN_LED_AZUL_INCORPORADO );
		Capturadora.Estado = CAPTURADORA_INACTIVA;
	}
	return retorno;
}

/****** Definición de funciones privadas ********************************************************/

void uHALadcLecturaCompletada ( adc_id_e ID )
{
	LecturaCompletada = true;
	uoLedEncender ( UOSAL_PIN_LED_VERDE_INCORPORADO );
}

/**------------------------------------------------------------------------------------------------
  * @brief	Suma una captura al promedio de señal, sincronizando según lo configurado.
  * @param	Ninguno
  * @retval	true si logró sincronizar y sumar la señal.
  */
bool SumarSenial(void)
{
	/* Variables locales -------------------------------------------------------------------------*/
	bool Control = false;
	//bool EstadoPrevio = false;
	bool PrevioEsAlto, ActualEsAlto;
	bool FlancoSubida, FlancoBajada;
	uint32_t Disparo, i, j;
	uint32_t Mascara  = 0;
	uint8_t  Desplaza = 0;
	uint32_t Inicio   = U_MUESTRAS_DESCARTADAS;
	uint32_t Final    = U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA;
	uint32_t LimiteSuperior = (Capturadora.NivelDisparo + Capturadora.Histeresis);
	uint32_t LimiteInferior = (Capturadora.NivelDisparo - Capturadora.Histeresis);
	entrada_id_e Origen = Capturadora.Config.OrigenDisparo;

	/* Precondiciones y verificaciones -----------------------------------------------------------*/
	if ( CAPTURADORA_PROCESANDO != Capturadora.Estado ) return false;
	if ( (Capturadora.NivelDisparo + Capturadora.Histeresis) > MAXIMO_12B ) LimiteSuperior = MAXIMO_12B;
	if ( Capturadora.NivelDisparo < (Capturadora.Histeresis + MINIMO_12B) ) LimiteInferior = MINIMO_12B;
	if ( Origen > ENTRADA_2 && Origen !=ORIGEN_ASINCRONICO) {
		uoHuboErrorTxt (" Origen invalido en SumarSenial de uCapturadora.");
		// TODO preparar este código para que procese modo alternado
	}

	/* Suma de señal capturada de ORIGEN_ASINCRONICO ---------------------------------------------*/
	// Solo debe copiar captura directamente
	if ( ORIGEN_ASINCRONICO == Origen ) {
		Inicio = U_MUESTRAS_DESCARTADAS;
		Final  = U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA;
		for ( i=Inicio; i<Final; i++) {
			MuestrasProcesadas12 [i-Inicio] = MuestrasCapturadas12[i];
			CantidadProcesadas12 [i-Inicio] = 1;
		}
		return true; // Fin de función.
	}

	/* Preparativos para sumar desde otros orígenes ----------------------------------------------*/
	// Asignamos valores para comparación y sincronismo
	if ( ENTRADA_1 == Origen ) {
		Mascara = MASCARA_DERECHA16;
		Desplaza = 0;
	}
	if ( ENTRADA_2 == Origen ) {
		Mascara = MASCARA_IZQUIERDA16;
		Desplaza = 16;
	}
	FlancoSubida = (SUBIDA==Capturadora.FlancoDisparo);
	FlancoBajada = (BAJADA==Capturadora.FlancoDisparo);
	// Se puede agregar flanco AMBOS luego.

	// Evaluamos estado inicial de la muestra previa
	PrevioEsAlto = false;
	PrevioEsAlto = ( ((MuestrasCapturadas12[U_MUESTRAS_DESCARTADAS+U_MUESTRAS_PRE_DISPARO-1]
						  &Mascara)>>Desplaza)
			         >= LimiteSuperior );
	ActualEsAlto = PrevioEsAlto;

	/* Evalúo muestra a muestra ------------------------------------------------------------------*/
	for ( Disparo = (U_MUESTRAS_DESCARTADAS+U_MUESTRAS_PRE_DISPARO); // inicio de ventana
			Disparo < (U_LARGO_CAPTURA_INICIAL-U_LARGO_CAPTURA);       // fin de ventana
			Disparo++)  {

		// Analizamos la muestra actual
		if ( (MuestrasCapturadas12[Disparo]&Mascara)>>Desplaza >= LimiteSuperior ) ActualEsAlto = true;
	   if ( (MuestrasCapturadas12[Disparo]&Mascara)>>Desplaza <= LimiteInferior ) ActualEsAlto = false;

		// Verificamos si hubo un cambio de estado acorde al flanco seleccionado
	   if ( PrevioEsAlto !=  ActualEsAlto                                     &&
	   	  ((FlancoSubida && ActualEsAlto) || (FlancoBajada && PrevioEsAlto))  ) {

		/*if ( ( FlancoSubida & !PrevioEsAlto &  ActualEsAlto ) ||
			  ( FlancoBajada &  PrevioEsAlto & !ActualEsAlto )    ) { */

			/* Iniciamos suma de captura -----------------------------------------------------------*/
			// Muestra inicial que se debe sumar:
			Inicio = Disparo - U_MUESTRAS_PRE_DISPARO;
			if ( Disparo < (U_MUESTRAS_PRE_DISPARO + U_MUESTRAS_DESCARTADAS) ) {
				uoHuboErrorTxt("sincronizando disparo de uCapturadora (Inicial).");
			}

			// Muestra final que se debe sumar:
			Final = Disparo + U_MUESTRAS_POS_DISPARO;
			if ( Final > U_LARGO_CAPTURA_INICIAL) {
				uoHuboErrorTxt("sincronizando disparo de uCapturadora (Final).");
			}

			// Suma en sí...
	    	for ( i=Inicio; i<Final; i++) {
	    		// Acá se supone que suma por separado ambos ADC. Muestras a promediar deben ser igual o menor a 16.
	    		j = i - Inicio;
	    		if (j>=U_LARGO_CAPTURA) uoHuboErrorTxt(" Desborde de indice sincronizando en uCapturadora.");
	    		MuestrasProcesadas12 [j] += MuestrasCapturadas12[i];
	    		//CantidadProcesadas12 [j] ++;
	    	}
	    	Control = true;  // lo logramos!!!
	    	Disparo = U_LARGO_CAPTURA_INICIAL;  // para salir del for
	   }

	   // Actualizar estado previo y volver a empezar
		PrevioEsAlto = ActualEsAlto;
	} // Fin del for
	return Control;
}

void ProcesarSenial(void)
{
	uint32_t i, DERECHA, IZQUIERDA;
	uint8_t Divisor = CapturasObjetivo() - Capturadora.CapturasRestantes;
	if (Divisor > 0) {
		// Si hubo al menos una captura, promediamos:
		for (i=0; i<U_LARGO_CAPTURA; i++) {
			DERECHA   = (MuestrasProcesadas12[i]&MASCARA_DERECHA16)   / Divisor;
			IZQUIERDA = (MuestrasProcesadas12[i]&MASCARA_IZQUIERDA16) / Divisor;
			IZQUIERDA = IZQUIERDA & MASCARA_IZQUIERDA16; // Dejamos la parte entera de la muestra
																		// alineada a la izquierda.
			// Cargamos promedio en senial procesada:
			MuestrasProcesadas12[i] = IZQUIERDA + DERECHA;
		}
	}
}

void ImprimirSenial32 (void)
{
	// Variables locales:
	uint32_t i, MUESTRA_ENTRADA_1, MUESTRA_ENTRADA_2, Disparo;
	Disparo = SenialAdmin[ENTRADA_1].ReferenciaT0;

	// Escribimos última muestra:
	uoEscribirTxt ("Senial cargada:");
	uoEscribirTxt ("\n\rENT_1 \tENT_2\n\r");

	for (i=0; i<U_LARGO_CAPTURA; i++) {

		MUESTRA_ENTRADA_1 = ( MuestrasProcesadas12[i] & MASCARA_DERECHA16   );
		MUESTRA_ENTRADA_2 = ( MuestrasProcesadas12[i] & MASCARA_IZQUIERDA16 ) >> 16;

		if ( (i==Disparo) && (i>0) ) uoEscribirTxt ("---> Disparo <---\n\r");

		uoEscribirUint ( MUESTRA_ENTRADA_1 );	// Dato de ENTRADA 1
		uoEscribirTxt  ( "\t" );		// Tabulación
		uoEscribirUint ( MUESTRA_ENTRADA_2 );	// Dato de ENTRADA 2
		//uEscribirTxt  ( "\t" );		// Tabulación
		//uEscribirUint ( CantidadProcesadas12[i] );	// Dato de ENTRADA 2
		uoEscribirTxt  ( "\n\r" );
	}
	uoEscribirTxtUint ( "Capturas promediadas \t= ", CapturasObjetivo() - Capturadora.CapturasRestantes );
	uoEscribirTxt     ( "\n\r" );
	uoEscribirTxtUint ( "Tiempo de captura \t= ", Capturadora.TiempoCaptura );
	uoEscribirTxt     ( " ms\n\r" );
	uoEscribirTxtUint ( "Nivel (12B) \t\t= ", Capturadora.NivelDisparo );
	uoEscribirTxt     ( "\n\r" );
	uoEscribirTxtUint ( "Escala Entrada 1\t= ", (uint32_t) (EntradaAdmin[0].Config.EscalaVertical*10) );
	uoEscribirTxt     ( "\n\r" );
	uoEscribirTxtUint ( "Escala Entrada 2\t= ", (uint32_t) (EntradaAdmin[1].Config.EscalaVertical*10) );
	uoEscribirTxt     ( "\n\r" );
}

bool ValidarOrigenDisparo ( entrada_id_e * P_ORIGEN )
{
	if ( *P_ORIGEN == ENTRADA_1 ) return true;
	if ( *P_ORIGEN == ENTRADA_2 ) return true;
	if ( *P_ORIGEN == ORIGEN_ASINCRONICO ) return true;
	*P_ORIGEN = ENTRADA_1;
	return false;
}

uint8_t CapturasObjetivo ( void )
{
	uint8_t Retorno=1;
	if ( Capturadora.Config.ModoCaptura & CAPTURA_PROMEDIADA_4 )  Retorno = 4;
	if ( Capturadora.Config.ModoCaptura & CAPTURA_PROMEDIADA_16 ) Retorno = 16;
	if ( Capturadora.Config.OrigenDisparo == ORIGEN_ASINCRONICO )   Retorno = 1;
	return Retorno;
}

/* @brief	Devuelve el índice correspondiente al valor de escala vertical
 * @param	Valor de escala en voltios
 * @retval	Índice que identifica la escala
 */
escala_vertical_e EscalaVerticalId ( float VOLTIOS )
{
	escala_vertical_e i=ESCALA_VERTICAL_1;
	float retorno = EscalasVerticales[i];
	while ( (retorno<VOLTIOS) && (i<ESCALA_VERTICAL_MAXIMA) ) {
		i++;
		retorno = EscalasVerticales[i];
	}
	return i;
}

/****************************************************************** FIN DE ARCHIVO ***************/
