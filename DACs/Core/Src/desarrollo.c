#include <stdio.h>
#include "API_uart.h"
#include "desarrollo.h"
#include "main.h"
#include "math.h"


//*** Definición de funciones ******************

void GenerarTriangular2 ( uint32_t * Senial, uint16_t Muestras)
{
	if (Muestras > MAX_N_MUESTRAS) Error_Handler();
	if (Muestras < 2) Error_Handler();

	// Variables usadas...
	uint16_t i = 0;

	// Cargo primer medio período
	for ( i=0; i<=Muestras/2; i++)
	{
		Senial[i] = 4095 * i * 2 / Muestras;
	}

	// Cargo segunda mitad
	for ( i=(Muestras/2+1); i<Muestras; i++)
	{
		Senial[i] = Senial[Muestras-i];
	}

	// Fin
	return;
}

void ImprimirDatos( gen_id_e NUM_GEN )
{
	gen_conf_s CONFIG = {0};
	char Cadena[50] = {0};
	uint16_t Muestras = 0;
	uint32_t FS = 0;

	if ( NUM_GEN == GENERADOR_1 ) {
			uartSendString((uint8_t *) "Generador Numero 1: \n\r");
	} else {
			uartSendString((uint8_t *) "Generador Numero 2: \n\r");
	}

	if ( uGeneradorLeerConfiguracion (NUM_GEN, &CONFIG) ) {

		FS       = (uint32_t) round(CONFIG.Frecuencia);
	    sprintf(Cadena, "Frecuencia de senial = %lu Hz\n\r", (uint32_t) FS);
	    uartSendString((uint8_t *) Cadena);

	    Muestras = CONFIG.Largo;
	    sprintf(Cadena, "Frecuencia de muestreo = %lu kHz\n\r", (uint32_t) FS*Muestras/1000);
	    uartSendString((uint8_t *) Cadena);

	} else {
		uartSendString((uint8_t *) "(!) No pudimos leer configuración.\n\r");
	}

	//uartSendString((uint8_t *) "------------------------------------------------------------\n\r");

}
