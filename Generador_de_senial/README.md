# Generador de señal cargada vía UART
Autor: Guillermo F. Caporaletti <gfcaporaletti@undav.edu.ar>
Fecha: Octubre de 2022.
Curso: CESE, FIUBA, 18Co.

## Resumen
En este proyecto, la placa Núcleo-144 (STMicroelectronics) recibe una señal desde un puerto UART y la transmite como salida en un puerto DAC. Desarrollado como trabajo final del curso de Programación de Microcontroladores de CESE, FIUBA, 18Co. Utiliza el DAC2 del microcontrolador stm32f429. El DAC recibe los datos mediante el Acceso Directo a Memoria (DMA) y cada un intervalo definido en el Timer 2. Logra una velodicad de muestras de 10,5 Msps. Las señales tienen 105 muestras por período, por lo que tienen una frecuencia de 100 KHz.
En la carpeta Drivers/API se encuetran las librerías desarrolladas. Las dos específicas de este proyecto son "API_dac_dma.h" y "API_generador.h". Los demás módulos habían sido desarrollados a lo largo el curso, aunque se les hicieron algunas mejoras. En la carpeta Core, se encuentra "main.c" y "main.h", donde tiene implementada una Máquina de Estados Finitos (MEF, o _FSM_ en anglosajón) para la recepción y generación de la señal. En la carpeta "Seniales", hay algunos ejemplos de señales probadas en este proyecto.

## La Máquina de Estado Finitos
La MEF implementada no es compleja aunque tiene una particularidad: Las condiciones para pasar de un estado a otro a veces depende del pulsador de usuario, a veces depende de si el pulso es corto o largo, y otras veces depende de la entrada desde la UART. Los estados:
- **ESPERA** (estado inicial) | Led verde titilante. Con el pulsador pasa a **RECIBIENDO**. 
- **RECIBIENDO** | Led verde. Cuando termina de recibir señal desde UART pasa a **CARGADO**.
- **CARGADO** | Led azul titilante lento. Con el pulsador pasa a **ENCENDIDO**.
- **ENCENDIDO** | Led azul titilante rápido. Con el pulsador corto pasa a **PAUSA**. Con el pulsador largo pasa nuevamente a **ESPERA** para cargar una nueva señal.
- **PAUSA** | Led azul titilante lento. Con el pulsador corto pasa a **ENCENDIDO**. Con el pulsador largo pasa nuevamente a **ESPERA** para cargar una nueva señal.
Como mencionamos, la MEF está implementada en main.c y se nutre de las librerías implementadas para leer datos de UART, evluar el pulsador de ususario y enviar la señal al DAC. 

## Implementación
### Utilización del DAC con DMA
El módulo "API_dac_dma.h" define tres funciones públicas, con las cuales maneja los datos de salida:
```
/* Funciones públicas --------------------------------------------------------*/
void Inicializar_DAC_DMA(void);
void Comenzar_DAC_DMA(uint32_t * Datos, uint32_t Num_Datos);
void Parar_DAC_DMA(void);
```
La primera función inicializa la salida DAC2, su modo de operación mediante DMA y el Timer 2 que hará que los datos sean convertidos a analógico directamente sin consumir recursos extras del micro. `void Comenzar_DAC_DMA(uint32_t * Datos, uint32_t Num_Datos)` y `void Parar_DAC_DMA(void)` activan y desactivan la salida. Nótese que lo único que necesita este módulo como parámetros es un puntero a memoria, donde comienzan los datos de la señal, y el tamaño de la señal. Un tutorial sencillo para la utilización de las funciones HAL que son llamadas dentro del módulo puede verse en [el ejemplo de este enlace](https://deepbluembedded.com/stm32-dac-sine-wave-generation-stm32-dac-dma-timer-example/). 

### Generador de señales
Lo primero que define el módulo "API_generador.h" es el tipo de datos enumeración con los estados posibles del generador: 
```
// Los estados por los que puede pasar cada generador (DAC1 o DAC2)
typedef enum {
	Espera,
	Vacio = Espera,
	Recibiendo,
	Cargado,
	Generando,
	Pausa
} estadosMEF;
```
Estos estados son tomados por "main.c" para la implementación de la MEF. Implementamos la MEF en el módulo principal porque las formas para cambiar de un estado a otro pueden ser muy distintas y varias con cada aplicación. Mantuvimos dentro del módulo "API_generador.h" lo mínimo indispensable para definir al generador.
Otra estructura fundamental es la del propio generador:
```
// Cada generador tiene estado y senial almacenada
typedef struct {
	estadosMEF estado;
	bool encendido;			// Este bool es redundante
	bool cargado;			// Este bool es redundante
	uint32_t senial[N_MUESTRAS];
} generador_t;
```
Esta estructura est´definida como privada dentro del archivo "API_generador.c" porque queremos que, desde afuera, sólo pueda ser modificada por las funciones públicas definidas. Éstas son:
```
/* Funciones públicas --------------------------------------------------------*/
void Gen_Init(void);
void Gen_Espera(void);
void Gen_Recibir(void);
void Gen_Cargar(uint32_t Senial[]);
void Gen_Encender(void);
void Gen_Pausar(void);
estadosMEF Gen_Estado(void);
void Gen_Actualiza_Leds(void);
```
Las funciones, nuevamente, sólo necesitan como parámetro a la señal. Si por algún motivo quisiéramos utilizar ambos DAC's, deberíamos crear una estructura pública con la mínima información necesaria. Esta estructura también podría tener otros parámetros como la frecuencia de muestras, largo de la señal, etc. El criterio de cómo pasar de un estado a otro va por cuenta del módulo principal. El módulo "API_generador" se encarga de hacer titilar los leds de modo adecuado, motivo por el cual se ha implementado la función `void Gen_Actualiza_Leds(void)`.

### Otras mejoras en demás módulos
Algunas mejoras implementadas en los demás módulos utilizados:
- "API_debounce.h": Contabilizar el tiempo en que el pulsador está presionado, para distinguir un pulso corto de uno largo. 
- "API_delay.h": Implementación de la función `void delayReset( delay_t * delay);` para poder evaluar correctamente el retardo agregado en el punto anterior.
- "API_uart.h": Implementación de la función `void uartClearBuffer()` para eliminar datos indeseados dentro de la UART.

## Mejoras posibles
Analizando el resultado concreto, y pensando en posibles aplicaciones, enumero algunas posibles mejoras:
- Estudiar mejor el funcionamiento de las funciones HAL de la UART, para acelerar la transmisión. En particular, sería deseable una lectura de línea de entrada terminada en '/n' (caracter 13). Por ahora se hace byte a byte. Y dado que para que no haya errores de comunicación utilizamos unos 12ms entre caracteres, la transmisión se hace lenta para valores de señal más allá de las 105 muestras.
- Utilizar un evento para medir el largo del presionado del botón de usuario. También, en el mismo sentido, deberíamos utilizar interrupciones para evaluar si se presionó el botón.
- Introducir una estructur para identificar si utililzamos el DAC1 o el DAC2. Esto podría permitir identificar tambíén otro DAC externo conectado de alguna otra forma.
