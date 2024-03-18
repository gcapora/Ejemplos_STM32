/**************************************************************************************************
 * Archivo: uSeniales.h
 * Breve:	Generador y manejador de seniales. Proyecto ISPEL.
 * Fecha:	Creado en noviembre 2023
 * Autor:	Guillermo F. Caporaletti
 *
 * Descripción:
 *    Define al objeto señal, con su estructura y funciones asociadas.
 *    Para ser utilizada tanto para generación como captura y análisis de señales.
 *
 * Pendiente:
 *    - Lograr especificar el alineamiento de los datos dentro del vector, de modo de almacenar dos
 *      señales en el vector de 32 bits.
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

#define U_MAX_N_MUESTRAS	0xFFFF  // Límite acorde a 16 bits de profundidad
#define MINIMO_12B			0
#define MAXIMO_12B			4095
#define MEDIO_12B			2048

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

// Tipos de seniales
typedef enum {
	CUADRADA,
	TRIANGULAR,
	SENOIDAL
} senial_tipo;

// Procesamiento último realizado a la estructura
typedef enum {
	E_NO_INICIALIZADA,
	E_INICIALIZADA,
	E_CARGADA,
	E_EVALUADA
} operacion_e;

// Estructura para configurar y cargar una señal deseada
typedef struct {
	uint32_t *  Muestras_p;        // Puntero a las muestras de la señal.
	uint32_t    LargoMaximo;       // Cantidad máxima de muestras que tiene la señal apuntada.
	uint32_t    Largo;             // la cantidad de muestras
    uint8_t     Multiplicador;     // Cantidad de ciclos que carga en las Largo muestras

	senial_tipo Tipo;
	uint32_t    Maximo;            // en cuentas
    uint32_t    Minimo;            // en cuentas
	float       Fase;              // en grados, entre 0º y 360º
	float       Simetria;          // Número entre 0 y 1. No aplica en senoidal.
	                               // En cuadrada equivale a ciclo de trabajo.
	// uint32_t    Muestra[U_MAX_N_MUESTRAS];  // las muestras de la senial
	//float       Amplitud;          // en voltios
	//float       Continua;          // en voltios
	//double      FrecuenciaDeseada; // en Hertz
	//double      FrecuenciaConfigurada;    // en Hertz
	operacion_e UltimaAccion;
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
