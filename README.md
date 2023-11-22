# Ejemplos para la placa Nucleo-144 de STM32

En DACs hay un código que saca por los dos canales señales senoidales de frecuencia variable. 
Por ahora funcionan con temporizadores independientes.
Se ha logrado sincronizarlos si ambos canañes DMA se activan siempre con el mismo temporizador.
Si utilizo temporizadores independientes, no logro resetear la cuenta del offset del DMA una vez comenzada. 
Entonces, aunque tengan la misma frecuencia, no logro controlar el defasaje entre señales.

