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
#include "stdbool.h"
#include "uOSAL.h"
#include "uSeniales.h"

/****** Definiciones privadas (macros) ***********************************************************/


/****** Definiciones privadas de tipos (private typedef) *****************************************/


/****** Definición de datos públicos *************************************************************/

uint32_t MAXIMO_DAC[UHAL_CANTIDAD_DACS]        = { 3950, 4010 };
uint32_t MINIMO_DAC[UHAL_CANTIDAD_DACS]        = { 50, 100 };
double   TRANSFERENCIA_DAC[UHAL_CANTIDAD_DACS] = { 805.861e-6, 805.861e-6 };

/****** Declaración de funciones privadas ********************************************************/

bool ConfigSenialVerificada (senial_config_s * ConfigDeseada);

/****** Definición de funciones privadas *********************************************************/


/****** Definición de funciones públicas *********************************************************/

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal triangular
* @param  Vector donde almacenar señal
*         Cantidad de muestras del vector
* @retval nada
*/
void uGenerarTriangular ( uint32_t * Senial, uint16_t Muestras, senial_config_s * ConfigDeseada )
{
	if (Muestras > MAX_N_MUESTRAS) uManejaError();
	if (Muestras < 2) uManejaError();
    if ( false == ConfigSenialVerificada (ConfigDeseada) ) uManejaError();

	// Variables usadas...
    uint32_t CicloM = ConfigDeseada->Ciclo * Muestras;
    uint32_t PicoPico = ConfigDeseada->Maximo - ConfigDeseada->Minimo;
	uint16_t i = 0;

	// Cargo primer medio período
	for ( i=0; i<=CicloM; i++)
	{
		Senial[i] = ConfigDeseada->Minimo + PicoPico * i / CicloM;
	}

	// Cargo segunda mitad
	for ( i=CicloM+1; i<Muestras; i++)
	{
		Senial[i] = ConfigDeseada->Maximo - PicoPico * (i-CicloM) / (Muestras-CicloM);
		//Senial[Muestras-i];
	}

	// Fin
	return;
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal senoidal
* @param  Vector donde almacenar señal
*         Cantidad de muestras del vector
*         Parámetros deseados (no utilizamos Ciclo)
* @retval nada
*/
void uGenerarSenoidal ( uint32_t * Senial, uint16_t Muestras, senial_config_s * ConfigDeseada )
{
    if (Muestras > MAX_N_MUESTRAS) uManejaError();
    if (Muestras < 2) uManejaError();
    if ( false == ConfigSenialVerificada (ConfigDeseada) ) uManejaError();

	// Variables locales
	uint16_t i = 0;
	double ValorMedio = ((double) ( ConfigDeseada->Maximo + ConfigDeseada->Minimo )) /2;
	double Amplitud   = ((double) ( ConfigDeseada->Maximo - ConfigDeseada->Minimo )) /2;
	double FaseRadian = ConfigDeseada->Fase * M_PI / 180;

	// Cargo senial seno
	for ( i=0; i<=Muestras; i++)
	{
		Senial[i] = ValorMedio + Amplitud * sin( (double) i * 2 * M_PI / Muestras + FaseRadian );
	}
}

/**------------------------------------------------------------------------------------------------
* @brief  Genera una señal cuadrada
* @param  Vector donde almacenar señal
*         Cantidad de muestras del vector
* @retval nada
*/
void uGenerarCuadrada ( uint32_t * Senial, uint16_t Muestras, senial_config_s * ConfigDeseada )
{
    if (Muestras > MAX_N_MUESTRAS) uManejaError();
    if (Muestras < 2) uManejaError();
    if ( false == ConfigSenialVerificada (ConfigDeseada) ) uManejaError();

    // Variables locales
    uint32_t CicloM = ConfigDeseada->Ciclo * Muestras;
	uint16_t i = 0;

	// Cargo ciclo activo
	for ( i=0; i<CicloM; i++)
	{
		Senial[i] = ConfigDeseada->Maximo;
	}

	// Cargo ciclo apagado
	for ( i=CicloM; i<Muestras; i++)
	{
		Senial[i] = ConfigDeseada->Minimo;
	}
}

/**------------------------------------------------------------------------------------------------
* @brief  Modifica los niveles de una señal ya almacenada
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/
void uModificarNiveles ( uint32_t * Senial, uint16_t Muestras, double Ganancia, uint32_t Continua)
{

}

/**------------------------------------------------------------------------------------------------
* @brief  Defasa una señal
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/
void uDefasar ( uint32_t * Senial, uint16_t Muestras, double Defasaje)
{

}

/**------------------------------------------------------------------------------------------------
* @brief  Defasa una señal
* @param  Vector con señal a modificar
*         Cantidad de muestras del vector
* @retval nada
*/

bool ConfigSenialVerificada (senial_config_s * ConfigDeseada)
{
    if (ConfigDeseada->Ciclo > 1) ConfigDeseada->Ciclo = 1;
    if (ConfigDeseada->Ciclo < 0) ConfigDeseada->Ciclo = 0;
    if (ConfigDeseada->Maximo > MAXIMO_12B) ConfigDeseada->Maximo = MAXIMO_12B;
    if (ConfigDeseada->Minimo < MINIMO_12B) ConfigDeseada->Minimo = MINIMO_12B;

    int32_t Multiplo = ConfigDeseada->Fase / 360; // trunca el valor
    ConfigDeseada->Fase = ConfigDeseada->Fase - 360 * Multiplo;
    if (ConfigDeseada->Fase < 0) ConfigDeseada->Fase += 360;

	return true;
}

/****************************************************************** FIN DE ARCHIVO ***************/
