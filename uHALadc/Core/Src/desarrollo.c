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

void CaptuRTOS_ImprimirSenial32 (void)
{
	// Variables locales:
	uint32_t i, Muestra, MUESTRA_ENTRADA_1, MUESTRA_ENTRADA_2, Disparo, Tiempo;
	capturadora_config_s ConfigCaptura = {0};
	entrada_config_s     ConfigEntrada = {0};

	// Precondiciones
	if ( NULL == P_Senial_E1 || NULL == P_Senial_E2 ) uoHuboErrorTxt ("en Imprimir... ppal.");

	// Asignaciones iniciales
	Disparo = P_Senial_E1->ReferenciaT0;

	// Escribimos última muestra:
	uoEscribirTxt ("Senial cargada:");
	uoEscribirTxt ("\n\rENT_1 \tENT_2\n\r");

	Tiempo = uoMilisegundos();

	for (i=0; i<U_LARGO_CAPTURA; i++) {

		Muestra = P_Senial_E1->Muestras_p[i];
		MUESTRA_ENTRADA_1 = ( Muestra & MASCARA_DERECHA16   );
		MUESTRA_ENTRADA_2 = ( Muestra & MASCARA_IZQUIERDA16 ) >> 16;

		if ( (i==Disparo) && (i>0) ) uoEscribirTxt ("---> Disparo <---\n\r");

		uoEscribirUint ( MUESTRA_ENTRADA_1 );	// Dato de ENTRADA 1
		uoEscribirTxt  ( "\t" );		// Tabulación
		uoEscribirUint ( MUESTRA_ENTRADA_2 );	// Dato de ENTRADA 2
		//uEscribirTxt  ( "\t" );		// Tabulación
		//uEscribirUint ( CantidadProcesadas12[i] );	// Dato de ENTRADA 2
		uoEscribirTxt  ( "\n\r" );
	}

	uCapturadoraObtener ( &ConfigCaptura );
	uCapturadoraEntradaObtener ( ConfigCaptura.OrigenDisparo, &ConfigEntrada );

	uoEscribirTxtUint	( "Capturas sincronizadas \t= ", uCapturadoraLeerSincronizadas() );
	uoEscribirTxt		( "\n\r" );
	uoEscribirTxtUint	( "Tiempo de captura \t= ", uCapturadoraLeerTiempoCaptura() );
	uoEscribirTxt     	( " ms\n\r" );
	uoEscribirTxtUint	( "Frecuencia muestreo \t= ", (uint32_t) round(uCapturadoraLeerFrecuenciaMuestreo()) );
	uoEscribirTxt     	( " Hz\n\r");

	switch (ConfigCaptura.OrigenDisparo)
	{
		case ENTRADA_1:
			uoEscribirTxt		( "Origen de disparo\t= ENTRADA 1\n\r" );
			uoEscribirTxtUint	( "Nivel (10V) \t\t= ", (uint32_t) (10.0*ConfigEntrada.NivelDisparo) );
		   uoEscribirTxt		( "\n\r" );
		   break;
		case ENTRADA_2:
			uoEscribirTxt 		( "Origen de disparo\t= ENTRADA 2\n\r" );
			uoEscribirTxtUint	( "Nivel (10V) \t\t= ", (uint32_t) (10.0*ConfigEntrada.NivelDisparo) );
		   uoEscribirTxt		( "\n\r" );
			break;
		case ORIGEN_ASINCRONICO:
			uoEscribirTxt 		( "Origen de disparo\t= ASINCRONICO\n\r" );
			break;
		default:
			// nada
			break;
	  }

	uCapturadoraEntradaObtener ( ENTRADA_1, &ConfigEntrada );
	uoEscribirTxtUint ( "Escala Entrada 1\t= ", (uint32_t) (uint32_t) round(10.0*ConfigEntrada.EscalaVertical) );
	uoEscribirTxt     ( "\n\r" );
	uCapturadoraEntradaObtener ( ENTRADA_2, &ConfigEntrada );
	uoEscribirTxtUint ( "Escala Entrada 2\t= ", (uint32_t) (uint32_t) round(10.0*ConfigEntrada.EscalaVertical) );
	uoEscribirTxt     ( "\n\r" );

	Tiempo = uoMilisegundos()-Tiempo;
	uoEscribirTxtUint ( "( Escrito en ", Tiempo );
	uoEscribirTxt     ( " ms. )\n\r" );

}
