/**************************************************************************************************
 * Archivo: uGenerador.c
 * Breve:	Generador de seniales con POO visto desde el usuario. Proyecto ISPEL.
 * Fecha:	Creado en marzo 2024
 * Autor:	Guillermo F. Caporaletti
 *
 * Descripción:
 *    Desarrolla el objeto Generador visto desde el operario.
 *    Básicamente define una estructura gen_config_s con los parámetros de la configuración deseada
 *    y otra estructura con el estado y datos del modo de funcionamiento resultante.
 *    Dentro de esta estructura, hay un puntero a la señal cargada en el generador.
 *    Las funciones inicializan, configuran, encienden y apagan el generador.
 *    También puede devolver información sobre su propia configuración y la señal cargada.
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "math.h"
#include "uOSAL.h"
#include "uHALdac.h"
#include "uGenerador.h"

/****** Definiciones privadas (macros) ***********************************************************/

#define LARGO_INICIAL                100     // Debe ser menor que UG_CANTIDAD_MUESTRAS_SENIAL
#define FRECUENCIA_MUESTREO_INICIAL  100000

/****** Definiciones privadas de tipos de datos (private typedef) ********************************/

// Estructura de manejo para un generador
typedef struct {
	gen_estados_e  Estado;
	senial_s       Senial;
	gen_conf_s     Configurar;
} gen_manejo_s;

/****** Declaraciones de datos públicos **********************************************************/

/****** Declaraciones de datos privados **********************************************************/

static uint32_t     Muestras  [CANTIDAD_GENERADORES][UG_CANTIDAD_MUESTRAS_SENIAL] __attribute__((aligned(4))) = {0};
static gen_manejo_s Generador [CANTIDAD_GENERADORES] = {0};

/****** Declaración de funciones privadas ********************************************************/

float    uGenEstimarVoltios (uint32_t, dac_id_t);
uint32_t uGenEstimarCuentas (float, dac_id_t);
bool     uGenConfiguradoDesdeSenial (dac_id_t);
bool     uGenConfiguracionesSonIguales (gen_conf_s, gen_conf_s);

/****** Definición de funciones privadas *********************************************************/

/**------------------------------------------------------------------------------------------------
* @brief
* @param
* @retval
*/
float    uGenEstimarVoltios (uint32_t NUM32, dac_id_t DAC_ID)
{
	return (float) ( (double) NUM32 - (double) CERO_DAC[DAC_ID] ) * TRANSFERENCIA_DAC[DAC_ID];
}

/**------------------------------------------------------------------------------------------------
* @brief
* @param
* @retval
*/
uint32_t uGenEstimarCuentas (float VOLT, dac_id_t DAC_ID)
{
	return (uint32_t) ( round( VOLT/TRANSFERENCIA_DAC[DAC_ID] ) + (double) CERO_DAC[DAC_ID] );
}

/**------------------------------------------------------------------------------------------------
* @brief
* @param
* @retval
*/
bool  uGenConfiguradoDesdeSenial (dac_id_t GEN)
{
	dac_id_t DAC_N = (dac_id_t) GEN;
	Generador[GEN].Configurar.Tipo   = Generador[GEN].Senial.Tipo;
	Generador[GEN].Configurar.Maximo = uGenEstimarVoltios (Generador[GEN].Senial.Maximo, DAC_N);
	Generador[GEN].Configurar.Minimo = uGenEstimarVoltios (Generador[GEN].Senial.Minimo, DAC_N);
	Generador[GEN].Configurar.Largo  = Generador[GEN].Senial.Largo;
	Generador[GEN].Configurar.Fase   = Generador[GEN].Senial.Fase;
	Generador[GEN].Configurar.Simetria  = Generador[GEN].Senial.Simetria;
	return true;
}

/**------------------------------------------------------------------------------------------------
* @brief
* @param
* @retval
*/
bool  uGenConfiguracionesSonIguales (gen_conf_s S1, gen_conf_s S2)
{
	bool RETORNO = false;
	RETORNO =            ( S1.Acople     == S2.Acople );
	RETORNO = RETORNO && ( S1.Simetria      == S2.Simetria );
	RETORNO = RETORNO && ( S1.Divisor    == S2.Divisor );
	RETORNO = RETORNO && ( S1.Fase       == S2.Fase );
	RETORNO = RETORNO && ( S1.Frecuencia == S2.Frecuencia );
	RETORNO = RETORNO && ( S1.Largo      == S2.Largo );
	RETORNO = RETORNO && ( S1.Maximo     == S2.Maximo );
	RETORNO = RETORNO && ( S1.Minimo     == S2.Minimo );
	RETORNO = RETORNO && ( S1.Tipo       == S2.Tipo );
	return RETORNO;
}

/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
* @brief  Inicializa un generador (o todos)
* @param  Generador a inicializar
*         Con TODOS_GENERADORES inicializa todos
* @retval true si la operación fue exitosa
*/
bool uGeneradorInicializar (gen_id_e GEN)
{
	// Valido parámetro
	if ( GEN > CANTIDAD_GENERADORES ) uManejaError();
	if ( GEN < CANTIDAD_GENERADORES && Generador[GEN].Estado != GENERADOR_NO_INICIALIZADO ) return false;

	// Variables locales
	bool RETORNO = false;
	dac_id_t DAC_N = (dac_id_t) GEN;  // Esto vale si cantidad de generadores y DACs es la misma.
	uint32_t LARGO0 = LARGO_INICIAL;
	if ( GEN == GENERADOR_1 ) LARGO0 = LARGO0 * 3;
	if (LARGO0 > UG_CANTIDAD_MUESTRAS_SENIAL) LARGO0 = UG_CANTIDAD_MUESTRAS_SENIAL;
	   // UG_CANTIDAD_MUESTRAS_SENIAL es lo que reservó la librería uGenerador
	double FREC_MUESTREO0;

    // Evalúo si se solicita inicializar todos:
	if ( GEN == CANTIDAD_GENERADORES ) {
		RETORNO = uGeneradorInicializar (GENERADOR_1);
		RETORNO = RETORNO && uGeneradorInicializar (GENERADOR_2);
		// Ambas inicializaciones deberían ser exitosas.
		return RETORNO;
	}

	// Inicializo HD
	uHALdacInicializar ( DAC_N );

	// Cargo senial inicial
	Generador[GEN].Senial.Tipo = SENOIDAL;
	Generador[GEN].Senial.Maximo = MAXIMO_DAC[DAC_N];
	Generador[GEN].Senial.Minimo = MINIMO_DAC[DAC_N];
	Generador[GEN].Senial.Largo = LARGO0;
	Generador[GEN].Senial.Multiplicador = 3;
	Generador[GEN].Senial.Fase = 0;
	Generador[GEN].Senial.Simetria = 0.5;
	Generador[GEN].Senial.LargoMaximo = UG_CANTIDAD_MUESTRAS_SENIAL;
	Generador[GEN].Senial.Muestras_p = Muestras[GEN]; // FUNDAMENTAL!!!
	                                                  // Acá conectamos la memoria reservada
	                                                  // para muestras con la estructura.
	uGenerarSenial ( &Generador[GEN].Senial );
	// TODO implementar una función que evalúe la configuración resultante

	// Asigno frecuencia
	FREC_MUESTREO0 = uHALdacdmaConfigurarFrecuenciaMuestreo (DAC_N, FRECUENCIA_MUESTREO_INICIAL);
	Generador[GEN].Configurar.Frecuencia = FREC_MUESTREO0 / LARGO0;
	Generador[GEN].Configurar.Divisor = uHALdacdmaLeerFrecuenciaBase () / FREC_MUESTREO0;  // Metodo mejorable.

	// Otras configuraciones
	uGenConfiguradoDesdeSenial (GEN);
	//Generador[GEN].Configurado.Tipo   = Generador[GEN].Senial.Tipo;
	//Generador[GEN].Configurado.Maximo = uGenEstimarVoltios (Generador[GEN].Senial.Maximo, DAC_N);
	//Generador[GEN].Configurado.Minimo = uGenEstimarVoltios (Generador[GEN].Senial.Minimo, DAC_N);
	//Generador[GEN].Configurado.Largo  = Generador[GEN].Senial.Largo;
	//Generador[GEN].Configurado.Fase   = Generador[GEN].Senial.Fase;
	//Generador[GEN].Configurado.Ciclo  = Generador[GEN].Senial.Ciclo;
	Generador[GEN].Configurar.Acople = DC;
	Generador[GEN].Estado = GENERADOR_APAGADO;

    // Fin...
	return true;
}

/**------------------------------------------------------------------------------------------------
* @brief  Configura un generador
* @param  Generador a inicializar (admite TODOS_GENERADORES)
*         Configuracion deseada
* @retval true Si la operación fue exitosa
*/
bool uGeneradorConfigurar  (gen_id_e GEN, gen_conf_s * CONFIG)
{
    // Valido parámetros  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	if ( GEN > CANTIDAD_GENERADORES ) uManejaError();
	if ( GEN < CANTIDAD_GENERADORES ) {
		if ( Generador[GEN].Estado == GENERADOR_NO_INICIALIZADO ||
			 Generador[GEN].Estado == GENERADOR_EN_ERROR ) return false;
	}

	// Variables locales y llamado recursivo  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    bool      RETORNO = false;
    if ( GEN == GENERADORES_TODOS ) {
		RETORNO = uGeneradorConfigurar (GENERADOR_1, CONFIG);
		RETORNO = RETORNO && uGeneradorConfigurar (GENERADOR_2, CONFIG);
		// Ambas inicializaciones deben ser exitosas para devolver true.
		return RETORNO;
	}

	dac_id_t  DAC_N = (dac_id_t) GEN;  // Esto vale si cantidad de generadores y DACs es la misma.
	double    FrecMuestreo = 0;
	double    PeriodoFraccional = 1;
	uint32_t  delta = 0;

	// Evalúo si hubo cambio de configuración - - - - - - - - - - - - - - - - - - - - - - - - - (1)
	if ( !uGenConfiguracionesSonIguales( *CONFIG, Generador[GEN].Configurar )) {
		// Hay un cambio en alguna configuración...

		// Cargo datos básicos para rehacer señal
		Generador[GEN].Senial.Tipo   = CONFIG->Tipo;
		Generador[GEN].Senial.Maximo = uGenEstimarCuentas ( CONFIG->Maximo, GEN);
		                               // Pasa de voltios a cuentas
		Generador[GEN].Senial.Minimo = uGenEstimarCuentas ( CONFIG->Minimo, GEN);  // Idem
		Generador[GEN].Senial.Fase   = CONFIG->Fase;
		Generador[GEN].Senial.Simetria  = CONFIG->Simetria;

		// Calculo frecuencia de muestreo, periodo y multiplicador
		FrecMuestreo = uHALdacdmaConfigurarFrecuenciaMuestreo ( DAC_N, CONFIG->Frecuencia *
				                                                UG_CANTIDAD_MUESTRAS_SENIAL);
		                               // Es la máxima frecuencia posible
		PeriodoFraccional = FrecMuestreo / CONFIG->Frecuencia;
		                               // Un PeríodoFraccional que pueda implementarse en las
		                               // muestras disponibles, teniendo en cuenta que puede haber
		                               // N períodos, y no sólo uno entero.
		Generador[GEN].Senial.Multiplicador = floor ( UG_CANTIDAD_MUESTRAS_SENIAL / PeriodoFraccional);
		if (Generador[GEN].Senial.Multiplicador == 0) Generador[GEN].Senial.Multiplicador = 1;
		                               // El Multiplicador es la cantidad entera de ciclos que entran en la señal a general.
		Generador[GEN].Senial.Largo  = (uint32_t) round(Generador[GEN].Senial.Multiplicador * PeriodoFraccional );
		//Generador[GEN].Senial.Largo  = (uint32_t) round(FrecMuestreo / CONFIG->Frecuencia);
		                               // Ahora el largo total puede incluir más de un período.

		// Regenero senial:
		delta = uOSALmiliseg ();
		uGenerarSenial ( &Generador[GEN].Senial );
		delta = uOSALmiliseg () - delta;
		uEscribirTextoEnteroP ( "[info] Generar senial demoro (ms) ", delta );

		// Copio configuracion establecida desde senial: (pudo variar...)
		//uGenConfiguradoDesdeSenial (GEN); // en lugar de esto debería evaluarse de nuevo la señal
		                                    // Dejamos en Configurar a la configuración solicitada.
		Generador[GEN].Configurar.Acople = CONFIG->Acople;

		// Calculo frecuencia de senial y divisor del tempo:
		Generador[GEN].Configurar.Frecuencia = FrecMuestreo / Generador[GEN].Senial.Largo;
		Generador[GEN].Configurar.Divisor    = (uHALdacdmaLeerFrecuenciaBase () / FrecMuestreo);  // Metodo mejorable.

		// Evaluo posibidades de salida:
		if ( Generador[GEN].Estado == GENERADOR_ENCENDIDO ) {
			// Como hubo modificación, debo parar y arrancar para que los cambios se efectivicen:
			uHALdacParar ( DAC_N );
			if ( CONFIG->Acople  == CERO) {
				// Salida equivalente a cero voltios:
				// uHALdacdmaComenzar ( DAC_N, &CERO_DAC[DAC_N], 1);
				uHALdacEstablecerValor ( DAC_N, CERO_DAC[DAC_N]);
			} else {
				// La senial a la salida:
				//uHALdacdmaComenzar ( DAC_N, Generador[GEN].Senial.Muestra, Generador[GEN].Senial.Largo );
				uHALdacdmaComenzar ( DAC_N, Generador[GEN].Senial.Muestras_p, Generador[GEN].Senial.Largo );
				Generador[GEN].Configurar.Acople = DC;

				/*
				// Veo si debo sincronizar
				if (uHALdacdmaSincronizar ()) {
					//uartSendString((uint8_t *) "Seniales sincronizadas!!! (en ...Configurar)\n\r");
				}
				*/
			}
		}
	} //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - (1)
    return true;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal según las configuraciones pedidas
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
bool uGeneradorEncender (gen_id_e GEN)
{
	// Valido parámetro
	if ( GEN > CANTIDAD_GENERADORES ) uManejaError();
	if ( GEN < CANTIDAD_GENERADORES && Generador[GEN].Estado !=  GENERADOR_APAGADO ) return false;

	// Variables locales
	bool RETORNO = false;
	dac_id_t DAC_N = (dac_id_t) GEN;  // Esto lo puedo hacer porque la cantidad de
	                                  // generadores y DACs es la misma.

    // Evalúo si se solicita encender todos:
	if ( GEN == GENERADORES_TODOS ) {
		RETORNO = uGeneradorEncender (GENERADOR_1);
		RETORNO = RETORNO && uGeneradorEncender (GENERADOR_2);
		// Ambas inicializaciones deberían ser exitosas.
		return RETORNO;
	}

	// Enciendo
	if (Generador[GEN].Estado == GENERADOR_APAGADO) {
		uHALdacdmaComenzar ( DAC_N,
				             Generador[GEN].Senial.Muestras_p,
							 Generador[GEN].Senial.Largo    );
		Generador[GEN].Estado = GENERADOR_ENCENDIDO;
		RETORNO = true;
	}

	// Veo si debo sincronizar
	if (uHALdacdmaSincronizar ()) {
		// uartSendString((uint8_t *) "Seniales sincronizadas!!!\n\r");
	}

	return true;
}

/**------------------------------------------------------------------------------------------------
* @brief
* @param
* @retval
*/
bool uGeneradorApagar      (gen_id_e GEN)
{
	// Valido parámetros
	if ( GEN > CANTIDAD_GENERADORES ) uManejaError();

	// Variables locales
    bool   RETORNO = false;
    dac_id_t DAC_N = (dac_id_t) GEN;  // Esto vale si cantidad de generadores y DACs es la misma.

	// Evalúo si se solicita configurar todos:
	if ( GEN == GENERADORES_TODOS ) {
		RETORNO = uGeneradorApagar (GENERADOR_1);
		RETORNO = RETORNO && uGeneradorApagar (GENERADOR_2);
		// Ambas inicializaciones deberían ser exitosas.
		return RETORNO;
	}

	// Actuo...
	uHALdacParar ( DAC_N );
	Generador[GEN].Estado = GENERADOR_APAGADO;
	return true;
}

/**------------------------------------------------------------------------------------------------
* @brief   Modifica * CONFIG con la configuracion actual del generador
*          No admite GENERADORES_TODOS
* @param   gen_id_e     del generador
*          gen_config * donde se almacenará la configuración
* @retval  true si la operación fue exitosa
*/
bool uGeneradorLeerConfiguracion (gen_id_e GEN, gen_conf_s * CONFIG)
{
   // Valido parámetro
   if ( GEN > CANTIDAD_GENERADORES ) uManejaError();
   if ( GEN == GENERADORES_TODOS ) return false;
   if ( Generador[GEN].Estado == GENERADOR_NO_INICIALIZADO ||
		Generador[GEN].Estado == GENERADOR_EN_ERROR ) return false;

   // Asigno
   *CONFIG = Generador[GEN].Configurar;
   return true;
}

/**------------------------------------------------------------------------------------------------
* @brief
* @param
* @retval nada
*/
gen_estados_e uGeneradorLeerEstado  (gen_id_e GEN)
{
	return Generador[GEN].Estado;
}

/****************************************************************** FIN DE ARCHIVO ***************/
