# Módulos para captura de señal

Incluye el desarrollo de algunos módulos para el proyecto ISPEL:

- uHALadc.h: administra el hardware con las funciones HAL de STM.
- uCapturadora.h: recibe los modos de captura típicos de un osciloscopio y los ejecuta utilizando el módulo uHALadc.h.
- uOSAL.h: un desarrollo más, incluyendo funciones de escritura y lectura en UART.

El archivo main.c y su encabezado tienen un ejemplo de prueba de los módulos. En desarrollo.c hay funciones complementarias.

## uHALadc.h

Para inicializar y configurar un ADC debe utiliarse:

```c
bool uHALadcInicializar            ( adc_id_e );                     
bool uHALadcConfigurar             ( adc_id_e, adc_config_s * );  
bool uHALadcObtener                ( adc_id_e, adc_config_s * );  
```

Para la lectura en sí, se implementan las siguientes funciones:

```c
bool uHALadcComenzarLectura        ( adc_id_e, uint32_t *, uint32_t );
bool uHALadcPararLectura 		   ( adc_id_e );
void uHALadcLecturaCompletada      ( adc_id_e );           
void uHALadcMediaLecturaCompletada ( adc_id_e );                  
```

Este módulo implementa básicamente el modo utilizados en el proyecto ISPEL: la conversión ADC dual por DMA, sincronizada con un temporizador. Cuando se completa un vector lectura (con la cantidad de muestras preestablecida), se para la conversión y se espera a un nuevo inicio de lectura utilizando la función uHALadcComenzarLectura().

## uCapturadora.h

Incluye diversos modos de captura similares a un osciloscopio. Tiene una configuración general (base de tiempo, origen de disparo, modo de disparo, etc.) y otra específica por canal (escala vertical, nivel de disparo y flanco). Las estructuras de configuración son:

```c
typedef struct {
	double    				EscalaHorizontal;	// [s]  Intervalo a capturar y mostrar en pantalla.
	entrada_id_e			OrigenDisparo;		//      Incluye Origen Modo Alternado y Modo Asincronico
	uint8_t					ModoCaptura;		//      Ver Definiciones públicas (macros)
} capturadora_config_s;

typedef struct {
	float	  	  			EscalaVertical;		// [V]  Máxima tensión positiva, que se adecuará a escalas disponibles
	float	   				NivelDisparo;
	flanco_e 				FlancoDisparo;
	bool					Encendida;
} entrada_config_s;
```

Para leer las señales capturadas debe utilizarse la función `senial_s* uCapturadoraSenialObtener ( entrada_id_e )`, que devuelve un puntero a la señal. 

## uOSAL.h

Esta capa en desarrollo, incluye funciones de uso general. Manejo de tiempos. Manejo elemental de leds. Escritura y lectura por UART. Manejo de errores. Citamos encabezado:

```c
bool     uOSALinicializar   ( void );
uint32_t uMilisegundos      ( void );
uint32_t uMicrosegundos   	( void );
void     uLedEncender       ( uint16_t );
void     uLedApagar         ( uint16_t );
void     uLedInvertir       ( uint16_t );
void     uEscribirTxt       ( char * );
void     uEscribirUint      ( uint32_t );
void     uEscribirTxtUint	( char *, uint32_t );
void     uEscribirTxtUintTxt( char *, uint32_t, char * );
bool	 uLeerChar 			( char *, uint32_t );
uint32_t uLeerTxt 			( char *, uint32_t, uint32_t );
void     uHuboError         ( void );
void     uHuboErrorTxt      ( char * );
```

En el encabezado `uOSALconfig.h` se defines parámetros específicos para la placa y la aplicación. 


