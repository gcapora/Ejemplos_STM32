# Ejemplos para la placa Nucleo-144 de STM32

## FreeRTOS 1 y 2
Corresponden a ejemplos vistos en CESE-FIUBA.

## Generador de señal
Proyecto con una salida DAC utilizando DMA.

## DACS
Código que saca por los dos canales señales de frecuencia variable. Funcionan con temporizadores independientes.
Se pueden sincronizar señales de misma frecuencia (para verificar información de fase) con error de 50 ns entre señales. Este error podría reducirse si lográramos utilizar el DAC en modo dual o si logramos que ambos canales del DAC se lancen simultáneamente con un pulso de hardware -en lugar de lanzamiento por software-.
