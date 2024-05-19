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

#ifndef U_LARGO_CAPTURA
#define U_LARGO_CAPTURA				50
#endif
#define U_MUESTRAS_PRE_DISPARO	((uint32_t)( U_LARGO_CAPTURA / 2 ))	// Muestras que guarda previo al disparo (trigger)
#define U_MUESTRAS_POS_DISPARO	((uint32_t)( U_LARGO_CAPTURA - U_MUESTRAS_PRE_DISPARO ))
#define U_MUESTRAS_DESCARTADAS	10 // Muestras iniciales que dan valores erróneos
#define U_SOBREMUESTREO          ((uint32_t)( U_LARGO_CAPTURA )) /// 2 )) // Un 50% de sobremuestreo
#define U_LARGO_CAPTURA_INICIAL  ((uint32_t)( U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA + U_SOBREMUESTREO ))
#define U_ESCALA_HORIZONTAL_MINIMA	((double) U_LARGO_CAPTURA / 2.4e6) // 2.4e6 es lo máximo que admite la placa

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
	entrada_config_s					Config;
	entrada_estado_e					Estado;
} entrada_admin_s;

typedef struct {
	capturadora_config_s				Config;
	double								FrecuenciaMuestreo;
	uint16_t								NivelDisparo; // Debe traducir el nivel desde fuente
	uint16_t								Histeresis;
	uint8_t								CapturasRestantes;
	capturadora_estado_e				Estado;
} capturadora_admin_s;

/****** Definición de datos privados **********************************************************/

static capturadora_admin_s	Capturadora = {0};
static entrada_admin_s		EntradaAdmin [U_ENTRADAS_CANTIDAD] = {0};
static senial_s				SenialAdmin  [U_ENTRADAS_CANTIDAD] = {0};
static uint32_t 				MuestrasCapturadas12 [U_LARGO_CAPTURA_INICIAL] = {0};
static uint32_t 				MuestrasProcesadas12 [U_LARGO_CAPTURA] = {0};
static uint8_t					CantidadProcesadas12 [U_LARGO_CAPTURA] = {0};
volatile bool LecturaCompletada = false;

/****** Declaración de funciones privadas ********************************************************/

bool SumarSenial (void);
void ProcesarSenial (void);
void ImprimirSenial32 (void);
bool ValidarOrigenDisparo ( entrada_id_e * );

/****** Definición de funciones públicas ********************************************************/

bool uCapturadoraInicializar 			( void )
{
	bool control = true;
	adc_config_s ConfigADC = {0};

	// Precondiciones.
	if ( CAPTURADORA_NO_INICIALIZADA != Capturadora.Estado) uHuboErrorTxt("Se intento reinicializar Capturadora.");

	// Inicializamos módulo uHALadc:
	if (false == uHALadcInicializar ( UHAL_ADC_TODOS ) ) control = false;

	// Valores predeterminados de Capturadora
	Capturadora.Config.EscalaHorizontal = 25e-6;
	Capturadora.Config.EsperaDisparo = 500;
	Capturadora.Config.ModoCaptura = CAPTURA_UNICA | CAPTURA_PROMEDIADA_16;
	Capturadora.Config.OrigenDisparo = ENTRADA_1;
	Capturadora.FrecuenciaMuestreo = U_LARGO_CAPTURA / Capturadora.Config.EscalaHorizontal;
	Capturadora.NivelDisparo = 2000;
	Capturadora.Histeresis = 25;
	Capturadora.CapturasRestantes = 0;

	// Valores predeterminados Entrada 1 (ADC 1 con dos canales)
	EntradaAdmin [ENTRADA_1].Config.EscalaVertical = 3.3;
	EntradaAdmin [ENTRADA_1].Config.NivelDisparo   = 1.5;
	EntradaAdmin [ENTRADA_1].Config.FlancoDisparo  = SUBIDA;
	EntradaAdmin [ENTRADA_1].Estado = ENTRADA_APAGADA;
	SenialAdmin [ENTRADA_1].Muestras_p   = MuestrasProcesadas12;
	SenialAdmin [ENTRADA_1].Inicio		 = 0;
	SenialAdmin [ENTRADA_1].LargoMaximo  = U_LARGO_CAPTURA;
	SenialAdmin [ENTRADA_1].Largo        = U_LARGO_CAPTURA;
	SenialAdmin [ENTRADA_1].Alineacion   = DERECHA_32;
	SenialAdmin [ENTRADA_1].UltimaAccion = E_INICIALIZADA;

	// Configuramos escala y frecuencia de ADC 1
	ConfigADC.Canal = U_ADC_CANAL_1;					// Este canal determina la escala
	ConfigADC.FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
	if ( false == uHALadcConfigurar (UHAL_ADC_1,&ConfigADC) ) uHuboErrorTxt("inicialiando Capturadora (ADC1).");
	if ( false == uHALadcObtener    (UHAL_ADC_1,&ConfigADC) ) uHuboErrorTxt("inicialiando Capturadora (ADC1).");
	Capturadora.FrecuenciaMuestreo = ConfigADC.FrecuenciaMuestreo;  // Puede que haya modificado la frecuencia.

	// Valores predeterminados Entrada 2
	EntradaAdmin [ENTRADA_2].Config.EscalaVertical = 3.3;
	EntradaAdmin [ENTRADA_2].Config.NivelDisparo   = 1.5;
	EntradaAdmin [ENTRADA_2].Config.FlancoDisparo  = SUBIDA;
	EntradaAdmin [ENTRADA_2].Estado = ENTRADA_APAGADA;
   SenialAdmin [ENTRADA_2].Muestras_p   = MuestrasProcesadas12;
   SenialAdmin [ENTRADA_2].Inicio		 = 0;
	SenialAdmin [ENTRADA_2].LargoMaximo  = U_LARGO_CAPTURA;
	SenialAdmin [ENTRADA_2].Largo        = U_LARGO_CAPTURA;
	SenialAdmin [ENTRADA_2].Alineacion   = IZQUIERDA_32;
	SenialAdmin [ENTRADA_2].UltimaAccion = E_INICIALIZADA;

	// Configuramos escala de ADC 2
	if ( false == uHALadcConfigurar (UHAL_ADC_2,&ConfigADC) ) uHuboErrorTxt("inicializando Capturadora (ADC2)");

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
		uEscribirTxt("ADVERTENCIA Se intento configurar Capturadora estando activa.");
		return false;
	}
	if ( CONFIG->EscalaHorizontal < U_ESCALA_HORIZONTAL_MINIMA ) {
		CONFIG->EscalaHorizontal = U_ESCALA_HORIZONTAL_MINIMA;
		uEscribirTxt ("ADVERTENCIA Correccion de ESCALA HORIZONTAL de capturadora.\n\r");
	}
	// TODO Verificar escala máxima posible.
	if ( false==ValidarOrigenDisparo(&CONFIG->OrigenDisparo) ) {
		uEscribirTxt ("ADVERTENCIA Correccion de ORIGEN de disparo.\n\r");
	}
	CONFIG->ModoCaptura = CAPTURA_UNICA | (CONFIG->ModoCaptura&MASCARA_PROMEDIO);

	// Actualizamos estructura
	Capturadora.Config.EscalaHorizontal = CONFIG->EscalaHorizontal;
	Capturadora.Config.ModoCaptura      = CONFIG->ModoCaptura;
	Capturadora.Config.OrigenDisparo    = CONFIG->OrigenDisparo;

	// Capturadora.Config.EsperaDisparo = 500;       ---> Por ahora no variaremos este parámetro
	// Capturadora.NivelDisparo = 2000;              ---> idem...

	// Actualizamos frecuencia de muestreo
	if ( false == uHALadcObtener    (UHAL_ADC_1,&ConfigADC) ) uHuboErrorTxt("en Configurar Capturadora (1).");
	ConfigADC.FrecuenciaMuestreo = (double) U_LARGO_CAPTURA / Capturadora.Config.EscalaHorizontal;
	if ( false == uHALadcConfigurar (UHAL_ADC_1,&ConfigADC) ) uHuboErrorTxt("en Configurar Capturadora (2).");
	Capturadora.FrecuenciaMuestreo = ConfigADC.FrecuenciaMuestreo;  // Puede que se haya modificado la frecuencia.
	Capturadora.Config.EscalaHorizontal = U_LARGO_CAPTURA / Capturadora.FrecuenciaMuestreo;
	CONFIG->EscalaHorizontal = Capturadora.Config.EscalaHorizontal;

	// Salida
	return true;
}

bool uCapturadoraObtener ( capturadora_config_s * CONFIG)
{
	CONFIG->EscalaHorizontal = Capturadora.Config.EscalaHorizontal;
	CONFIG->EsperaDisparo    = Capturadora.Config.EsperaDisparo;
	CONFIG->ModoCaptura      = Capturadora.Config.ModoCaptura;
	CONFIG->OrigenDisparo    = Capturadora.Config.OrigenDisparo;
	return true;
}

double uCapturadoraLeerFrecuenciaMuestreo ( void )
{
	return Capturadora.FrecuenciaMuestreo;
}

bool uCapturadoraEntradaConfigurar 	( entrada_id_e ID, entrada_config_s * CONFIG, senial_s * PSENIAL )
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

bool uCapturadoraEntradaObtener ( entrada_id_e ID, entrada_config_s * PCONFIG, senial_s * PSENIAL)
{
	return true;
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
	bool control = false;
	uint32_t i;
	//adc_config_s ConfigADC = {0};

	/* Precondiciones */
	if ( CAPTURADORA_INACTIVA != Capturadora.Estado ) {
		return false;
	}
	if ( Capturadora.Config.OrigenDisparo <= ENTRADA_2                             &&
		  EntradaAdmin[Capturadora.Config.OrigenDisparo].Estado == ENTRADA_ENCENDIDA ) {
		control = true;
	} else if ( EntradaAdmin[ENTRADA_1].Estado == ENTRADA_ENCENDIDA ||
			      EntradaAdmin[ENTRADA_2].Estado == ENTRADA_ENCENDIDA  ) {
		control = true;
	}
	if (false==control) return control;

	/* Configuramos parámetros y lanzamos muestreo */
	Capturadora.CapturasRestantes = 1; // Si no se promedia queda este valor.
	if ( Capturadora.Config.ModoCaptura & CAPTURA_PROMEDIADA_4 )  Capturadora.CapturasRestantes = 4;
	if ( Capturadora.Config.ModoCaptura & CAPTURA_PROMEDIADA_16 ) Capturadora.CapturasRestantes = 16;
	if ( Capturadora.Config.OrigenDisparo == MODO_ASINCRONICO )   Capturadora.CapturasRestantes = 1;
	for (i=0; i<U_LARGO_CAPTURA; i++) {
		MuestrasProcesadas12 [i] = 0;
		CantidadProcesadas12 [i] = 0;
	}

	if ( true == control ) {
		control = uHALadcComenzarLectura (	UHAL_ADC_1,						// Lanza muestreo en ADC 1 y 2
														MuestrasCapturadas12,		// Vector donde almaceno lo muestreado
														U_LARGO_CAPTURA_INICIAL );	// Largo del vector
		uLedEncender ( UOSAL_PIN_LED_AZUL_INCORPORADO );
	}

	// Actualizamos estado general y salimos
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
		uEscribirTxt ("MSJ Paramos captura...\n\r");
		Capturadora.Estado = CAPTURADORA_INACTIVA;
	}

	// Salimos
	return control;
}

bool uCapturadoraActualizar ( void)
// Verifica tareas pendientes y actua.
{
	if (true == LecturaCompletada) {
		LecturaCompletada = false;

		// Actualizamos estado de Capturadora
		Capturadora.Estado = CAPTURADORA_PROCESANDO;

		// Sumamos lo capturado
		if ( true == SumarSenial() ) {
			Capturadora.CapturasRestantes--;
			uLedApagar ( UOSAL_PIN_LED_VERDE_INCORPORADO );

		}
		if (Capturadora.CapturasRestantes > 16) {
			uHuboErrorTxt ("en Actualizar Capturadora.");
		}

		// Analizamos si volvemos a capturar o procesamos
		if (Capturadora.CapturasRestantes > 0) {
			// Iniciamos una nueva captura para sumar al promedio:
			uHALadcComenzarLectura (	UHAL_ADC_1,						// Lanza muestreo en ADC 1 y 2
												MuestrasCapturadas12,		// Vector donde almaceno lo muestreado
												U_LARGO_CAPTURA_INICIAL );	// Largo del vector
			Capturadora.Estado = CAPTURADORA_CAPTURANDO;
		} else {
			// Procesamos lo que hay:
			uEscribirTxt("Procesando...\n\r");
			ProcesarSenial();
			if ( MODO_ASINCRONICO == Capturadora.Config.OrigenDisparo ) {
				SenialAdmin[ENTRADA_1].ReferenciaT0 = 0;
				SenialAdmin[ENTRADA_2].ReferenciaT0 = 0;
			} else if ( ENTRADA_2 >= Capturadora.Config.OrigenDisparo ) {
				SenialAdmin[ENTRADA_1].ReferenciaT0 = U_MUESTRAS_PRE_DISPARO;
				SenialAdmin[ENTRADA_2].ReferenciaT0 = U_MUESTRAS_PRE_DISPARO;
			}
			SenialAdmin[ENTRADA_1].FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
			SenialAdmin[ENTRADA_2].FrecuenciaMuestreo = Capturadora.FrecuenciaMuestreo;
			Capturadora.Estado = CAPTURADORA_CAPTURA_COMPLETADA;
		}
	}
	return true;
}

bool uCapturadoraSenialCargada ( void )
// Indica si hay al menos una señal cargada.
{
	bool retorno = false;
	uCapturadoraActualizar();
	if ( CAPTURADORA_CAPTURA_COMPLETADA == Capturadora.Estado ) {
		retorno = true;
		ImprimirSenial32();
		uLedApagar ( UOSAL_PIN_LED_AZUL_INCORPORADO );
		Capturadora.Estado = CAPTURADORA_INACTIVA;
	}
	return retorno;
}

/****** Definición de funciones privadas ********************************************************/

void uHALadcLecturaCompletada ( adc_id_e ID )
{
	LecturaCompletada = true;
	uLedEncender ( UOSAL_PIN_LED_VERDE_INCORPORADO );
}

bool SumarSenial(void)
{
	/* Variables locales */
	bool Control = false;
	bool Bajo = true;
	uint32_t Disparo, i, j;
	uint32_t Mascara  = 0;
	uint8_t  Desplaza = 0;
	uint32_t Inicio  = U_MUESTRAS_DESCARTADAS;
	uint32_t Final   = U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA;
	uint32_t LimiteSuperior = (Capturadora.NivelDisparo + Capturadora.Histeresis);
	uint32_t LimiteInferior = (Capturadora.NivelDisparo - Capturadora.Histeresis);
	entrada_id_e Origen = Capturadora.Config.OrigenDisparo;

	/* Precondiciones y verificaciones */
	if ( CAPTURADORA_PROCESANDO != Capturadora.Estado ) return false;
	if ( LimiteSuperior           > MAXIMO_12B )                            LimiteSuperior = MAXIMO_12B;
	if ( Capturadora.NivelDisparo < (Capturadora.Histeresis + MINIMO_12B) ) LimiteInferior = MINIMO_12B;
	if ( Origen > ENTRADA_2 && Origen !=MODO_ASINCRONICO) {
		uHuboErrorTxt (" Origen invalido en SumarSenial de uCapturadora."); // TODO preparar este código para que procese modo alternado
	}

	/* Suma de señal capturada */
	if ( MODO_ASINCRONICO == Origen ) {
		// No hay nada para sincronizar y copio directamente:
		Inicio = U_MUESTRAS_DESCARTADAS;
		Final  = U_MUESTRAS_DESCARTADAS + U_LARGO_CAPTURA;
		for ( i=Inicio; i<Final; i++) {
			MuestrasProcesadas12 [i-Inicio] = MuestrasCapturadas12[i];
			CantidadProcesadas12 [i-Inicio] = 1;
		}
		return true;
		// Fin de función.
	}

	if ( ENTRADA_1 == Origen ) {
		Mascara = MASCARA_DERECHA16;
		Desplaza = 0;
	}
	if ( ENTRADA_2 == Origen ) {
		Mascara = MASCARA_IZQUIERDA16;
		Desplaza = 16;
	}

	// Evalúo si la señal inicia por debajo del nivel buscado:
	Bajo = ( (MuestrasCapturadas12[U_MUESTRAS_DESCARTADAS]&Mascara)>>Desplaza <= LimiteInferior );

	// Evalúo muestra la muestra...
	//for ( Disparo=U_MUESTRAS_DESCARTADAS; Disparo<U_LARGO_CAPTURA_INICIAL; Disparo++) {
	for ( Disparo=U_MUESTRAS_DESCARTADAS; Disparo<(U_MUESTRAS_DESCARTADAS+U_LARGO_CAPTURA); Disparo++) {

		// Supondremos por ahora siempre flanco de subida
		if ( ( true == Bajo )                                                      &&
			  ( Disparo >= (U_MUESTRAS_DESCARTADAS + U_MUESTRAS_PRE_DISPARO) )      &&
			  ( (MuestrasCapturadas12[Disparo]&Mascara)>>Desplaza >= LimiteSuperior) ) {
	    	// Hubo disparo!!! -------------------------------------------------------------------------------------------

			// Muestra inicial que se debe sumar:
			if ( Disparo >= (U_MUESTRAS_PRE_DISPARO + U_MUESTRAS_DESCARTADAS) ) {
				// Verificación redundante que debiera verificarse siempre
				Inicio = Disparo - U_MUESTRAS_PRE_DISPARO;
			} else {
				uHuboErrorTxt("sincronizando disparo de uCapturadora.");
				Inicio = U_MUESTRAS_DESCARTADAS;
			}
			// Inicio = (Inicio > U_MUESTRAS_DESCARTADAS) ? Inicio : U_MUESTRAS_DESCARTADAS;
			// Puede haber menos muestras para sumar?

			// Muestra final que se debe sumar:
			Final = Disparo + U_MUESTRAS_POS_DISPARO;
			Final = (Final > U_LARGO_CAPTURA_INICIAL) ? U_LARGO_CAPTURA_INICIAL : Final;
				// Puede haber menos muestras para sumar

	    	for ( i=Inicio; i<Final; i++) {
	    		// Acá se supone que suma por separado ambos ADC. Muestras a promediar deben ser igual o menor a 16.
	    		j = i - (Disparo-U_MUESTRAS_PRE_DISPARO) ;
	    		if (j>=U_LARGO_CAPTURA) uHuboErrorTxt(" Desborde de indice sincronizando en uCapturadora.");
	    		MuestrasProcesadas12 [j] += MuestrasCapturadas12[i];
	    		CantidadProcesadas12 [j] ++;
	    	}
	    	Control = true;
	    	Disparo = U_LARGO_CAPTURA_INICIAL;
	    }

	    // Actualizar Bajo antes del tiempo de Disparo
	    if ( (MuestrasCapturadas12[Disparo]&Mascara)>>Desplaza <= LimiteInferior ) Bajo = true;
	    if ( (MuestrasCapturadas12[Disparo]&Mascara)>>Desplaza >= LimiteSuperior ) Bajo = false;
	}
	return Control;
}

void ProcesarSenial(void)
{
	uint32_t i, DERECHA, IZQUIERDA;
	for (i=0; i<U_LARGO_CAPTURA; i++) {
		if ( CantidadProcesadas12[i] > 0 ) {
			// Si hubo muestra capturada en i, promediamos:
			DERECHA   = (MuestrasProcesadas12[i]&MASCARA_DERECHA16)   / CantidadProcesadas12[i];
			IZQUIERDA = (MuestrasProcesadas12[i]&MASCARA_IZQUIERDA16) / CantidadProcesadas12[i];
			IZQUIERDA = IZQUIERDA & MASCARA_IZQUIERDA16; // Dejamos la parte entera de la muestra
																		// alineada a la izquierda.
			// Cargamos promedio en senial procesada:
			MuestrasProcesadas12[i] = IZQUIERDA + DERECHA;
		} else {
			MuestrasProcesadas12[i] = 0; // Esto es redundante.
		}
	}
}

void ImprimirSenial32 (void)
{
	// Variables locales:
	uint32_t i, MUESTRA_ENTRADA_1, MUESTRA_ENTRADA_2, Disparo;
	Disparo = SenialAdmin[ENTRADA_1].ReferenciaT0;

	// Escribimos última muestra:
	uEscribirTxt ("Senial cargada:");
	uEscribirTxt ("\n\rENT_1 \tENT_2\n\r");

	for (i=0; i<U_LARGO_CAPTURA; i++) {

		MUESTRA_ENTRADA_1 = ( MuestrasProcesadas12[i] & MASCARA_DERECHA16   );
		MUESTRA_ENTRADA_2 = ( MuestrasProcesadas12[i] & MASCARA_IZQUIERDA16 ) >> 16;

		if ( (i==Disparo) && (i>0) ) uEscribirTxt ("---> Disparo <---\n\r");

		uEscribirUint ( MUESTRA_ENTRADA_1 );	// Dato de ENTRADA 1
		uEscribirTxt 	( "\t" );		// Tabulación
		uEscribirUint ( MUESTRA_ENTRADA_2 );	// Dato de ENTRADA 2
		uEscribirTxt 	( "\t" );		// Tabulación
		uEscribirUint ( CantidadProcesadas12[i] );	// Dato de ENTRADA 2
		uEscribirTxt    ( "\n\r" );

		//MAXIMO = P_ADC1 > MAXIMO ? P_ADC1 : MAXIMO;
		//MINIMO = P_ADC1 < MINIMO ? P_ADC1 : MINIMO;

	}
	//uEscribirTextoEnteroSS ("\n\rMaximo ultimo = ", MAXIMO);
	//uEscribirTextoEnteroSS ("\n\rMinimo ultimo = ", MINIMO);
	//uEscribirTexto 		   ("\n\r------------------------------------------------------------------\n\r");
}

bool ValidarOrigenDisparo ( entrada_id_e * P_ORIGEN )
{
	if ( *P_ORIGEN == ENTRADA_1 ) return true;
	if ( *P_ORIGEN == ENTRADA_2 ) return true;
	if ( *P_ORIGEN == MODO_ASINCRONICO ) return true;
	*P_ORIGEN = ENTRADA_1;
	return false;
}

/****************************************************************** FIN DE ARCHIVO ***************/
