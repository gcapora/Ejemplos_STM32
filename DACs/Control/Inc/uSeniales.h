/**************************************************************************************************
 * Archivo: uSeniales.h
 * Breve:	Generador y manejador de seniales. Proyecto ISPEL.
 * Fecha:	Creado en noviembre 2023
 * Autor:	Guillermo F. Caporaletti
 *
 *************************************************************************************************/

#ifndef ISPEL_USENIALES_H_
#define ISPEL_USENIALES_H_

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>
#include "uHALdac.h" // Hace falta para el enum dac_id_t

/****** Definiciones públicas (macros) ***********************************************************/

#define MAX_N_MUESTRAS 4096
#define MINIMO_12B     0
#define MAXIMO_12B     4095
#define MEDIO_12B      2048

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

// Estructura para configurar señal deseada
typedef struct {
	uint32_t Maximo;   // en cuentas
	uint32_t Minimo;
	float Ciclo;       // número entre 0 y 1
	float Fase;        // en grados, entre 0º y 360º
} senial_config_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

void uGenerarTriangular ( uint32_t * Senial, uint16_t Muestras, senial_config_s * ConfigDeseada );
void uGenerarSenoidal   ( uint32_t * Senial, uint16_t Muestras, senial_config_s * ConfigDeseada );
void uGenerarCuadrada   ( uint32_t * Senial, uint16_t Muestras, senial_config_s * ConfigDeseada );
void uModificarNiveles  ( uint32_t * Senial, uint16_t Muestras, double Ganancia, uint32_t Continua);
void uDefasar           ( uint32_t * Senial, uint16_t Muestras, double Defasaje);

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
