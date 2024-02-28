/**************************************************************************************************
 * Archivo: uSeniales.h
 * Breve:	Generador y manejador de seniales. Proyecto ISPEL.
 * Fecha:	Creado en noviembre 2023
 * Autor:	Guillermo F. Caporaletti
 *
 * Descripción:
 *    Define al objeto señal, con su estructura y funciones asociadas.
 *    Para ser utilizada tanto para generación como captura y análisis de señales.
 *    El objeto generador y capturadora serán desarrollados en otras librerías.
 *
 * Pendiente:
 *    - Incorporar "Ciclo" en señal senoidal.
 *    - Hacer más eficiente la función uDefasar().
 *
 *************************************************************************************************/

#ifndef ISPEL_USENIALES_H_
#define ISPEL_USENIALES_H_

/****** Librerías (includes) *********************************************************************/

#include <stdint.h>
#include <stdbool.h>
//#include "uHALdac.h" // Hace falta para el tipo enum dac_id_t
                     // Incluye stdbool.h y stdint.h

/****** Definiciones públicas (macros) ***********************************************************/

#define MAX_N_MUESTRAS 5000   // Esto debería estar en uSeniales_conf.h si existiera.
#define MINIMO_12B     0
#define MAXIMO_12B     4095
#define MEDIO_12B      2048

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

// Tipos de seniales
typedef enum {
	CUADRADA,
	TRIANGULAR,
	SENOIDAL
} senial_tipo;

// Estructura para configurar y cargar una señal deseada
typedef struct {
	senial_tipo Tipo;
	uint32_t    Maximo;            // en cuentas
    uint32_t    Minimo;            // en cuentas
	float       Fase;              // en grados, entre 0º y 360º
	float       Ciclo;             // número entre 0 y 1 (no aplica en senoidal)
    uint32_t    Largo;             // la cantidad de muestras
	uint32_t    Muestra[MAX_N_MUESTRAS];  // las muestras de la senial
	//float       Amplitud;          // en voltios
	//float       Continua;          // en voltios
	//double      FrecuenciaDeseada; // en Hertz
	//double      FrecuenciaConfigurada;    // en Hertz
	bool        Cargada;
} senial_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

void uGenerarSenial     ( senial_s * Senial );
void uGenerarTriangular ( senial_s * Senial );
void uGenerarSenoidal   ( senial_s * Senial );
void uGenerarCuadrada   ( senial_s * Senial );
void uModificarNiveles  ( senial_s * Senial, double Ganancia, uint32_t Continua);
void uDefasar           ( senial_s * Senial, double Defasaje);

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
