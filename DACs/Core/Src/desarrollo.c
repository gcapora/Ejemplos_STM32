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

void ImprimirDatos( dac_id_t NUM_DAC )
{
	char Cadena[32] = {0};
	uint16_t Muestras = 0;

	if ( NUM_DAC == UHAL_DAC_1 ) {
			uartSendString((uint8_t *) "\n\rDAC N1:\n\r");
			Muestras = MUESTRAS1;
	} else {
			uartSendString((uint8_t *) "\n\rDAC N2:\n\r");
			Muestras = MUESTRAS2;
	}

	uint32_t FM = (uint32_t) uHALdacdmaLeerFrecuenciaMuestreo ( NUM_DAC );
	sprintf(Cadena, "%lu", FM);
	// sprintf(Cadena, "%lu", sizeof(double));
	uartSendString((uint8_t *) "Frecuencia de muestreo = ");
	uartSendString((uint8_t *) Cadena);
	uartSendString((uint8_t *) " Hz\n\r");

	float FS = round((float) FM / (float) Muestras);
	sprintf(Cadena, "%lu", (uint32_t) FS);
	uartSendString((uint8_t *) "Frecuencia de senial = ");
	uartSendString((uint8_t *) Cadena);
	uartSendString((uint8_t *) " Hz\n\r");

}
