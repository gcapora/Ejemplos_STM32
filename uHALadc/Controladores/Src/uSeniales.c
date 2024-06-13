/**************************************************************************************************
 * Archivo: uSeniales.c
 * Breve:	Generador y manejador de seniales. Proyecto ISPEL.
 * Fecha:	Creado en noviembre 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

#include "math.h"
#include "uOSAL.h"
#include "uSeniales.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos públicos *************************************************************/

double uSenFrecuenciaMuestrasBase;

/****** Declaración de funciones privadas ********************************************************/

bool   ConfigSenialVerificada (senial_s * );
double AcotarGrados (double Grados);
double Triangular   (double Grados, double Simetria);
double Cuadrada     (double Grados, double CicloDeTrabajo);

/****** Definición de funciones privadas *********************************************************/

/**------------------------------------------------------------------------------------------------
* @brief  Verifica valores de configuración y corrije algunos
* @param  Estructura de la señal con configuración deseada
* @retval true si la operación fue exitosa
*/
bool ConfigSenialVerificada (senial_s * Senial)
{
	// Errores graves
	if (Senial->Largo > U_MAX_N_MUESTRAS) uoHuboErrorTxt("verificando senial en uSeniales.");
	if (Senial->Largo < 2) uoHuboErrorTxt("verificando senial en uSeniales.");
	if (Senial->Largo > Senial->LargoMaximo) uoHuboErrorTxt("verificando senial en uSeniales.");

	// Correcciones de ciclo y márgenes
	if (Senial->Simetria > 1)        Senial->Simetria = 1;
    if (Senial->Simetria < 0)        Senial->Simetria = 0;
    if (Senial->Maximo > MAXIMO_12B) Senial->Maximo = MAXIMO_12B;
    if (Senial->Minimo < MINIMO_12B) Senial->Minimo = MINIMO_12B;
    if (Senial->Multiplicador == 0)  Senial->Multiplicador = 1;

    // Llevamos los grados entre 0º y 360º
    Senial->Fase = AcotarGrados( Senial->Fase );

	return true;
}

/**------------------------------------------------------------------------------------------------
* @brief  Acota los grados entre 0 y 360
* @param  Grados origen
* @retval Grados resultantes
*/
double AcotarGrados (double Grados)
{
    int32_t Multiplo = Grados / 360;
    Grados = Grados - 360 * Multiplo;
    if (Grados < 0) Grados += 360;
    return Grados;
}

/**------------------------------------------------------------------------------------------------
* @brief  Devuelve un valor entre -1 y 1 triangular
* @param  Grados origen
* @retval Valor entre -1 y 1
*/
double Triangular (double Grados, double Simetria)
{
	double Salida = 0;
	double CuartoP = 180.0 * Simetria;
	double MedioN  = 360.0 - CuartoP;
	//double PendienteP = (double) 1/45 * Simetria;
	//double PendienteN = (double) 1/45 * (1-Simetria);
	//uEscribirTextoEnteroP ( "[dato] grados ", (uint32_t) Grados*1000 );

	if ( Grados < 0 || Grados >= 360 ) Grados = AcotarGrados(Grados);
	if ( Grados < CuartoP )       {
		// Primer cuarto de onda
		Salida =  0.0 + Grados / Simetria / 180;
	} else if ( Grados < MedioN ) {
		// Segundo y tercer cuarto
		Salida =  1.0 - (Grados - CuartoP) / (1-Simetria) / 180;
	} else if ( Grados < 360 )    {
		// Cuarto cuarto
		Salida = -1.0 + (Grados - MedioN) / Simetria / 180;
	} else {
		uoHuboError();
	}
	return Salida;
}

/**------------------------------------------------------------------------------------------------
* @brief  Devuelve un valor entre -1 y 1 de una señal cuadrada
* @param  Grados origen
*         Ciclo de Trabajo
* @retval Valor entre -1 y 1
*/
double Cuadrada (double Grados, double CicloDeTrabajo)
{
	double Salida = 0;
	if ( CicloDeTrabajo < 0 ) CicloDeTrabajo = 0;
	if ( CicloDeTrabajo > 1 ) CicloDeTrabajo = 1;
	if ( Grados < 0 || Grados >= 360 ) Grados = AcotarGrados(Grados);

	if ( Grados < 360.0 * CicloDeTrabajo ) {
		// Primera media onda encendida (si CdT = 50%)
		Salida =  1;
	} else {
		Salida = -1;
	}
	return Salida;
}

/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal según las configuraciones pedidas
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarSenial     ( senial_s * Senial )
{
	// Elegimos qué otra función utilizar:
	switch (Senial->Tipo) {
	        case CUADRADA:
	        	uGenerarCuadrada (Senial);
	            break;
	        case TRIANGULAR:
	        	uGenerarTriangular (Senial);
	            break;
	        case SENOIDAL:
	        	uGenerarSenoidal (Senial);
	            break;
	        default:
	            // Hubo un error
	        	uoHuboError();
	    }
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal triangular
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarTriangular ( senial_s * Senial )
{
    // Verificamos precondiciones y hacemos correcciones
    if ( false == ConfigSenialVerificada (Senial) ) uoHuboError();

    // Variables locales
   	uint32_t i = 0;
   	double ValorMedio = ((double) ( Senial->Maximo + Senial->Minimo )) /2;
   	double Amplitud   = ((double) ( Senial->Maximo - Senial->Minimo )) /2;
   	double Omega      =  360.0 * Senial->Multiplicador / Senial->Largo;
   	// uEscribirTextoEnteroP ( "[i] Largo implementado ", Senial->Largo );

   	// Cargo senial triangular
    for ( i = 0; i < Senial->Largo; i++)
    {
    		Senial->Muestras_p[i] = ValorMedio +
    				                Amplitud *
									Triangular ( Omega * i + Senial->Fase, Senial->Simetria );
    }

    // Fin
	Senial->Tipo = TRIANGULAR;
	Senial->UltimaAccion = E_CARGADA;
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal senoidal
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarSenoidal ( senial_s * Senial )
{
    // Verificamos precondiciones y hacemos correcciones
    if ( false == ConfigSenialVerificada (Senial) ) uoHuboError();

	// Variables locales
	uint32_t i = 0;
	double ValorMedio = ((double) ( Senial->Maximo + Senial->Minimo )) /2;
	double Amplitud   = ((double) ( Senial->Maximo - Senial->Minimo )) /2;
	double FaseRadian = Senial->Fase * M_PI / 180;
	double Omega      = 2 * M_PI * Senial->Multiplicador / Senial->Largo;
	// uEscribirTextoEnteroP ( "[i] Largo implementado ", Senial->Largo );

	// Cargo senial seno
	for ( i = 0; i < Senial->Largo; i++)
	{
		Senial->Muestras_p[i] = ValorMedio +
				                Amplitud *
								sin( (double) i * Omega + FaseRadian );
	}

	// Fin
	Senial->Tipo = SENOIDAL;
	Senial->UltimaAccion = E_CARGADA;
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal cuadrada
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarCuadrada ( senial_s * Senial )
{
    // Verificamos precondiciones y hacemos correcciones
	if ( false == ConfigSenialVerificada (Senial) ) uoHuboError();

    // Variables locales
	uint32_t i = 0;
	double Omega      = 360.0 * Senial->Multiplicador / Senial->Largo;
	double ValorMedio = ((double) ( Senial->Maximo + Senial->Minimo )) /2;
   double Amplitud   = ((double) ( Senial->Maximo - Senial->Minimo )) /2;

	// Cargo señal cuadrada
	for ( i=0; i<Senial->Largo; i++)
	{

		Senial->Muestras_p[i] = ValorMedio +
                                Amplitud *
				                Cuadrada ( Omega * i + Senial->Fase, Senial->Simetria );
	}

	// Fin
	Senial->Tipo = CUADRADA;
	Senial->UltimaAccion = E_CARGADA;
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Modifica los niveles de una señal ya almacenada
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/
void uModificarNiveles ( senial_s * Senial, double Ganancia, uint32_t Continua)
{

}

/**------------------------------------------------------------------------------------------------
* @brief  Defasa una señal (forma ineficiente pero que no usa mucha memoria)
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/
void uDefasar ( senial_s * Senial, double Defasaje)
{
    // Verificamos precondiciones y hacemos correcciones
	if ( false == ConfigSenialVerificada (Senial) ) uoHuboError();
    Defasaje = AcotarGrados (Defasaje);
    if ( Defasaje == 0) return; // Nada que hacer!!!

    // Variables locales
    uint32_t delta_indice = Defasaje /360 * Senial->Largo;
    uint32_t intermedio, i, j;

    // Operación
    for ( i=0; i<delta_indice; i++) {      // Hay un defasaje que se debe hacer delta_indice veces
    	intermedio = Senial->Muestras_p[0];   // Guardo el primer valor
    	for ( j=0; j<Senial->Largo-1; j++) {  // Defaso la señal en un índice
    		Senial->Muestras_p[j] = Senial->Muestras_p[j+1];
    	}
    	Senial->Muestras_p[Senial->Largo-1] = intermedio;  // Pongo al final el valor primero
    }
}

/**------------------------------------------------------------------------------------------------
* @brief  Evalua la senial
* @param  Senial a evaluar
* @retval nada
*         Debe cargar en la estructura de la señal, el valor máximo y mínimo del vector.
*/
void uEvaluar ( senial_s * Senial)
{

}

/****************************************************************** FIN DE ARCHIVO ***************/
