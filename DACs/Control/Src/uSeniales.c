/**************************************************************************************************
 * Archivo: uSeniales.c
 * Breve:	Generador y manejador de seniales. Proyecto ISPEL.
 * Fecha:	Creado en noviembre 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

/****** Librerías (includes) *********************************************************************/

//#include <stdio.h>
#include "math.h"
#include "uOSAL.h"
#include "uSeniales.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos públicos *************************************************************/

double uSenFrecuenciaMuestrasBase;

/****** Declaración de funciones privadas ********************************************************/

bool   ConfigSenialVerificada (senial_config_s * ConfigDeseada);
double AcotarGrados (double Grados);

/****** Definición de funciones privadas *********************************************************/


/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal según las configuraciones pedidas
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarSenial     ( senial_config_s * Senial )
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
	        	uManejaError();
	    }
	return;
}


/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal triangular
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarTriangular ( senial_config_s * Senial )
{
    // Verificamos precondiciones y hacemos correcciones
    if ( false == ConfigSenialVerificada (Senial) ) uManejaError();

	// Variables locales
    uint32_t CicloM   = Senial->Ciclo * Senial->Largo;
    uint32_t PicoPico = Senial->Maximo - Senial->Minimo;
	uint16_t i = 0;

	// Cargo primer medio período
	for ( i=0; i<=CicloM; i++)
	{
		Senial->Muestra[i] = Senial->Minimo + PicoPico * i / CicloM;
	}

	// Cargo segunda mitad
	for ( i=CicloM+1; i<Senial->Largo; i++)
	{
		Senial->Muestra[i] = Senial->Maximo - PicoPico * (i-CicloM) / (Senial->Largo-CicloM);
	}

	// Adelanto la señal 90º
	uDefasar(Senial, 90+Senial->Fase);

	// Fin
	Senial->Tipo = TRIANGULAR;
	Senial->Cargada = true;
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal senoidal
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarSenoidal ( senial_config_s * Senial )
{
    // Verificamos precondiciones y hacemos correcciones
    if ( false == ConfigSenialVerificada (Senial) ) uManejaError();

	// Variables locales
	uint16_t i = 0;
	double ValorMedio = ((double) ( Senial->Maximo + Senial->Minimo )) /2;
	double Amplitud   = ((double) ( Senial->Maximo - Senial->Minimo )) /2;
	double FaseRadian = Senial->Fase * M_PI / 180;

	// Cargo senial seno
	for ( i = 0; i <= Senial->Largo; i++)
	{
		Senial->Muestra[i] = ValorMedio + Amplitud * sin( (double) i * 2 * M_PI / Senial->Largo + FaseRadian );
	}

	// Fin
	Senial->Tipo = SENOIDAL;
	Senial->Cargada = true;
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal cuadrada
* @param  Estructura de senial con configuración deseada
* @retval nada
*/
void uGenerarCuadrada ( senial_config_s * Senial )
{
    // Verificamos precondiciones y hacemos correcciones
	if ( false == ConfigSenialVerificada (Senial) ) uManejaError();

    // Variables locales
    uint32_t CicloM = Senial->Ciclo * Senial->Largo;
	uint16_t i = 0;

	// Cargo ciclo activo
	for ( i=0; i<CicloM; i++)
	{
		Senial->Muestra[i] = Senial->Maximo;
	}

	// Cargo ciclo apagado
	for ( i=CicloM; i<Senial->Largo; i++)
	{
		Senial->Muestra[i] = Senial->Minimo;
	}

	// Adelanto la señal 90º
	uDefasar(Senial, Senial->Fase);

	// Fin
	Senial->Tipo = CUADRADA;
	Senial->Cargada = true;
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Modifica los niveles de una señal ya almacenada
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/
void uModificarNiveles ( senial_config_s * Senial, double Ganancia, uint32_t Continua)
{

}

/**------------------------------------------------------------------------------------------------
* @brief  Defasa una señal (forma ineficiente pero que no usa mucha memoria)
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/
void uDefasar ( senial_config_s * Senial, double Defasaje)
{
    // Verificamos precondiciones y hacemos correcciones
	if ( false == ConfigSenialVerificada (Senial) ) uManejaError();
    Defasaje = AcotarGrados (Defasaje);
    if ( Defasaje == 0) return; // Nada que hacer!!!

    // Variables locales
    uint32_t delta_indice = Defasaje /360 * Senial->Largo;
    uint32_t intermedio, i, j;

    // Operación
    for ( i=0; i<delta_indice; i++) {      // Hay un defasaje que se debe hacer delta_indice veces
    	intermedio = Senial->Muestra[0];   // Guardo el primer valor
    	for ( j=0; j<Senial->Largo-1; j++) {  // Defaso la señal en un índice
    		Senial->Muestra[j] = Senial->Muestra[j+1];
    	}
    	Senial->Muestra[Senial->Largo-1] = intermedio;  // Pongo al final el valor primero
    }
}

/**------------------------------------------------------------------------------------------------
* @brief  Verifica valores de configuración y corrije algunos
* @param  Estructura de la señal con configuración deseada
* @retval true si la operación fue exitosa
*/
bool ConfigSenialVerificada (senial_config_s * Senial)
{
	// Errores graves
	if (Senial->Largo > MAX_N_MUESTRAS) uManejaError();
	if (Senial->Largo < 2) uManejaError();

	// Correcciones de ciclo y márgenes
	if (Senial->Ciclo > 1) Senial->Ciclo = 1;
    if (Senial->Ciclo < 0) Senial->Ciclo = 0;
    if (Senial->Maximo > MAXIMO_12B) Senial->Maximo = MAXIMO_12B;
    if (Senial->Minimo < MINIMO_12B) Senial->Minimo = MINIMO_12B;

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


/****************************************************************** FIN DE ARCHIVO ***************/
