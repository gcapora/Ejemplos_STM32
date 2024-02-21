# Generador de señales doble
Autor: Guillermo F. Caporaletti <gcaporaletti@fi.uba.ar>
Fecha: Noviembre de 2023.
Curso: CESE, FIUBA, 18Co.

## Resumen
En este proyecto controlamos las salidas de los dos canales del DAC de un microcontrolador STM32F429 utilizando la placa Nucleo-144. Se basa en el trabajo final del curso del Programación de Microcontroladores (PdM) de CESE, FIUBA, 18Co. Se desarrolló un módulo uHALdac.h para manejo de hardware y uSeniales.h para generación de señal senoidal, triangular y cuadrada con diversos parámetros. Lo demás se trabajó sobre el código autogenerado de CubeMx. 

## Estructura en capas
En la carpeta **Control** se encuentran los siguientes módulos:

- uHALdac.h administra hardware del DAC, DMA y temporizadores necesarios.
- uSeniales.h genera señales senoidal, triangular y cuadrada en una cadena específica.
- uOSAL.h tiene algunas funciones elementales típicamente del sistema operativo. Está pensada para reducir el esfuerzo a la hora de hacer que estos módulos funciones bajo FreeRTOS u otro sistema operativo.

Además de esto hay otros módulos que habían sido desarrollados como parte del trabajo final de PdM bajo la carpeta **Drivers/API**. De estos módulos, utilizamso básicamente API_uart.h para enviar mensajes a través de UART. 

El código de prueba está implementado directamente en main.c, respetando los márgenes del código autogenerado. Existe duplicación de código autogenerado con el código elaborado en los módulos.

## Manejo de DAC
El módulo uHALdac.h define las siguientes funciones:

```c
void   uHALdacdmaInicializar ( dac_id_t );
double uHALdacdmaLeerFrecuenciaMuestreo ( dac_id_t );
double uHALdacdmaConfigurarFrecuenciaMuestreo ( dac_id_t, double );
void   uHALdacdmaComenzar ( dac_id_t, uint32_t *, uint32_t );
void   uHALdacdmaParar ( dac_id_t );
void   uHALdacdmaReanudar ( dac_id_t );
void   uHALdacdmaSincronizar ( void );
```

Primero se debe inicializar el DAC con uHALdacdmaInicializar(). Luego ya se puede utilizar uHALdacdmaComenzar(), utilizando una frecuencia de muestreo predeterminada. La frecuencia de muestreo se puede cambiar __al vuelo__ utilizando uHALdacdmaConfigurarFrecuenciaMuestreo() directamente.

![Diagrama de estados de las funciones DACs](https://github.com/gcapora/Ejemplos_STM32/blob/main/DACs/Docs/Estados%20DACs.drawio.png)

*Figura 1: Estados y transiciones posibles con las funciones de uHALdac.h*

En la __Figura 1__ se muestran los estados de cada DAC y las transiciones permitidas con las funciones de la librería.

## Problemas de implementación
Con este módulo logramos controlar los dos canales del DAC enviando señales independientes con frecuencias de muestreo independientes. Se ha utilizado 5 Msps para cada canal de modo de tener un margen grande hasta el máximo de 10,2 Msps que establece la hoja de datos para un único canal en funcionamiento. 

El problema está cuando se desea que ambas señales tengan la misma frecuencia de muestreo y tengan un defasaje conocido. Se utiliza la función uHALdacdmaSincronizar() para que ambas señales comiencen en el mismo momento, con un error de 50 ns. Este tiempo fue medido con osciloscopio y tiene que ver con el hecho de que utilizamos dos temporizadores diferentes para ambas señales. A 100 kHz esto representa un error de 1,8º.

## Referencias
Algunos enlaces que se estuvieron revisando:

1. Tutorial simple para la utilización de las funciones HAL que utilizan DAC con DMA [en este enlace](https://deepbluembedded.com/stm32-dac-sine-wave-generation-stm32-dac-dma-timer-example/). 
2. "How to use two DAC channels simultaneously" explicado [en este enlace](https://community.st.com/t5/stm32-mcus-products/how-to-use-two-dac-channels-simultaneously/td-p/210588). No da un código concreto de cómo hacerlo. Cita al manual de referencia RM0410 de STM32F76xxx.

