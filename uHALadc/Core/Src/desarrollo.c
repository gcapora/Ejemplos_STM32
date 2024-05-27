/*******************************************************************************
  * @file           : desarrollo.c
  * @brief          : Funciones en desarrollo
  ******************************************************************************
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Variables importadas ------------------------------------------------------*/

extern senial_s * P_Senial_E1;
extern senial_s * P_Senial_E2;

/* Private function prototypes -----------------------------------------------*/

/* Public function ************************************************************/

void ImprimirSenial32_main (void)
{
	// Variables locales:
	uint32_t i, Muestra, MUESTRA_ENTRADA_1, MUESTRA_ENTRADA_2, Disparo, Tiempo;
	capturadora_config_s ConfigCaptura = {0};
	entrada_config_s     ConfigEntrada = {0};

	// Precondiciones
	if ( NULL == P_Senial_E1 || NULL == P_Senial_E2 ) uHuboErrorTxt ("en Imprimir... ppal.");

	// Asignaciones iniciales
	Disparo = P_Senial_E1->ReferenciaT0;

	// Escribimos última muestra:
	uEscribirTxt ("Senial cargada:");
	uEscribirTxt ("\n\rENT_1 \tENT_2\n\r");

	Tiempo = uMilisegundos();

	for (i=0; i<U_LARGO_CAPTURA; i++) {

		Muestra = P_Senial_E1->Muestras_p[i];
		MUESTRA_ENTRADA_1 = ( Muestra & MASCARA_DERECHA16   );
		MUESTRA_ENTRADA_2 = ( Muestra & MASCARA_IZQUIERDA16 ) >> 16;

		if ( (i==Disparo) && (i>0) ) uEscribirTxt ("---> Disparo <---\n\r");

		uEscribirUint ( MUESTRA_ENTRADA_1 );	// Dato de ENTRADA 1
		uEscribirTxt  ( "\t" );		// Tabulación
		uEscribirUint ( MUESTRA_ENTRADA_2 );	// Dato de ENTRADA 2
		//uEscribirTxt  ( "\t" );		// Tabulación
		//uEscribirUint ( CantidadProcesadas12[i] );	// Dato de ENTRADA 2
		uEscribirTxt  ( "\n\r" );
	}

	uCapturadoraObtener ( &ConfigCaptura );
	uCapturadoraEntradaObtener ( ConfigCaptura.OrigenDisparo, &ConfigEntrada );

	uEscribirTxtUint	( "Capturas sincronizadas \t= ", uCapturadoraLeerSincronizadas() );
	uEscribirTxt		( "\n\r" );
	uEscribirTxtUint	( "Tiempo de captura \t= ", uCapturadoraLeerTiempoCaptura() );
	uEscribirTxt     	( " ms\n\r" );
	uEscribirTxtUint	( "Frecuencia muestreo \t= ", (uint32_t) round(uCapturadoraLeerFrecuenciaMuestreo()) );
	uEscribirTxt     	( " Hz\n\r");

	switch (ConfigCaptura.OrigenDisparo)
	{
		case ENTRADA_1:
			uEscribirTxt		( "Origen de disparo\t= ENTRADA 1\n\r" );
			uEscribirTxtUint	( "Nivel (10V) \t\t= ", (uint32_t) (10.0*ConfigEntrada.NivelDisparo) );
		   uEscribirTxt		( "\n\r" );
		   break;
		case ENTRADA_2:
			uEscribirTxt 		( "Origen de disparo\t= ENTRADA 2\n\r" );
			uEscribirTxtUint	( "Nivel (10V) \t\t= ", (uint32_t) (10.0*ConfigEntrada.NivelDisparo) );
		   uEscribirTxt		( "\n\r" );
			break;
		case ORIGEN_ASINCRONICO:
			uEscribirTxt 		( "Origen de disparo\t= ASINCRONICO\n\r" );
			break;
		default:
			// nada
			break;
	  }

	uCapturadoraEntradaObtener ( ENTRADA_1, &ConfigEntrada );
	uEscribirTxtUint ( "Escala Entrada 1\t= ", (uint32_t) (uint32_t) round(10.0*ConfigEntrada.EscalaVertical) );
	uEscribirTxt     ( "\n\r" );
	uCapturadoraEntradaObtener ( ENTRADA_2, &ConfigEntrada );
	uEscribirTxtUint ( "Escala Entrada 2\t= ", (uint32_t) (uint32_t) round(10.0*ConfigEntrada.EscalaVertical) );
	uEscribirTxt     ( "\n\r" );

	Tiempo = uMilisegundos()-Tiempo;
	uEscribirTxtUint ( "( Escrito en ", Tiempo );
	uEscribirTxt     ( " ms. )\n\r" );

}
