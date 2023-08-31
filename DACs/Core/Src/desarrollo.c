
#include "desarrollo.h"


//*** Definición de funciones ******************

void GenerarTriangular ( uint32_t * Senial, uint16_t Muestras)
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
