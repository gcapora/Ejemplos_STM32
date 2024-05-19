/**************************************************************************************************
 * Archivo: uSeniales.h
 * Breve:	Generador y manejador de seniales. Proyecto ISPEL.
 * Fecha:	Creado en noviembre 2023
 * Autor:
 *
 * Descripción:
 *    Define al objeto señal, con su estructura y funciones asociadas.
 *    Para ser utilizada tanto para generación como captura y análisis de señales.
 *    Las muestras de la señal son de 12 bits.
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

/****** Definiciones públicas (macros) ***********************************************************/

#define U_MAX_N_MUESTRAS	0xFFFF  // Límite acorde a 16 bits de profundidad
#define MINIMO_12B			0
#define MAXIMO_12B			4095
#define MEDIO_12B				2048

/****** Definiciones públicas de tipos de datos (public typedef) *********************************/

// Tipos de seniales ------------------------------------------------------------------------------
typedef enum {
	CUADRADA,
	TRIANGULAR,
	SENOIDAL
} senial_tipo;

// Tipos de alineamiento --------------------------------------------------------------------------
/* Se presupone que los datos son de 12 bits (almacenados en 16).
   En ningún caso deben modificarse los 16 bits que no corresponden con el alineamiento.
   Este tipo enumeracón aún no está impemetado en la librería <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
typedef enum {
	DERECHA_32,		// Datos almacenados en los 16 bits menos significativos de uint32_t
	IZQUIERDA_32,   // Datos almacenados en los 16 bits más significativos de uint32_t
	DERECHA_16      // Simplemente un número de 16 bits en uint16_t
} alineacion_e;

// Procesamiento último realizado a la estructura -------------------------------------------------
typedef enum {
	E_NO_INICIALIZADA,
	E_INICIALIZADA,
	E_CARGADA,
	E_EVALUADA
} operacion_e;

// Estructura para configurar y cargar una señal deseada ------------------------------------------
typedef struct {
	uint32_t *		Muestras_p;    // Puntero a las muestras de la señal.
	// Debe modificarse por void * de modo de que
	// pueda ser utilizado como vector uint16_t o uint32_t <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Otra forma podría ser agregar otro puntero uint16_t
	alineacion_e	Alineacion;
	uint32_t    	LargoMaximo;   // Cantidad máxima de muestras que tiene el vector apuntado.
	uint32_t    	Largo;         // La cantidad de muestras utilizadas de ese vector. 1 + última muestra válida.
	uint32_t    	Inicio;        // Primera muestra válida.
	uint32_t			ReferenciaT0;	// Muestra equivalente a tiempo=0 (puede haber señal antes de ese tiempo).
	double			FrecuenciaMuestreo;

	senial_tipo 	Tipo;
   uint32_t    	Maximo;        // en cuentas
   uint32_t    	Minimo;        // en cuentas
   uint8_t     	Multiplicador; // Cantidad de ciclos que carga en las Largo muestras
	float       	Fase;          // en grados, entre 0º y 360º
	float       	Simetria;      // Número entre 0 y 1. No aplica en senoidal.

	operacion_e 	UltimaAccion;	// Sirve para identificar origen de la señal.
} senial_s;

/****** Declaraciones de datos externos **********************************************************/


/****** Declaración de funciones públicas ********************************************************/

void uGenerarSenial     ( senial_s * Senial );
void uGenerarTriangular ( senial_s * Senial );
void uGenerarSenoidal   ( senial_s * Senial );
void uGenerarCuadrada   ( senial_s * Senial );
void uModificarNiveles  ( senial_s * Senial, double Ganancia, uint32_t Continua);
void uDefasar           ( senial_s * Senial, double Defasaje);
void uEvaluar           ( senial_s * Senial);  // Función a implementar <<<<<<<<<<<<<<<<<<<<<<<<<<<

/*************************************************************************************************/
#endif /* ISPEL_UOSAL_H_ */
/****************************************************************** FIN DE ARCHIVO ***************/
