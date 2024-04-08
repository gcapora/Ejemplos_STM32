# Comentarios para el desarrollo de módulo uHALuart

Abril 2024.

## ¿Qué necesitamos?
Un módulo que administre el envío y recepción de datos a través de un puerto UART de la NUCLEO144-429ZI. Todas las instrucciones HAL deben estar dentro de este módulo y sólo se accederá a ellas a través de este módulo. El módulo se probará en bare-metal.

**Las funciones a desarrollar:**
```
bool     uHALuartInicializar   (void);
bool     uHALuartEnviarBytes   (uint8_t * pbytes, uint16_t tamanio);
bool     uHALuartEnviarCadena  (uint8_t * pcadena); 
                               // ¿o (char *) pcadena?
uint16_t uHALuartRecibirBytes  (uint8_t * pstring, uint16_t tamanio);
void     uHALuartLimpiarBuffer (void);
```

**La estructura de archivos:**
- ``uHALuart.h``: Incluye funciones, tipos y macros públicos. Este encabezado debiera ser portable a otras plataformas de hardware.
- ``uHALconfig.h``: Incluye los macros vinculados a la configuración de la NUCLEO144-429ZI y de la aplicación en particular. 
- ``uHALuart.c``: Las definiciones de las funciones, básicamente. Es la implementación HAL en NUCLEO144-429ZI.

## ¿Qué tenemos como punto de partida? 
- En las carpetas incluí API_uart, desarrollado en la especialización para escribir y leer por UART. Las separé para modificar por separado y armar una edición más adecuada al proyecto ISPEL.
- También incluí un archivo API_complemento.c, que incluye una función utilizada en el proyecto Generador_de_senial. Esta función no anda sola pero sirve para mostrar cómo resolví un problema en la recepción de datos, que es que la función HAL de STM no te dice cuántos caracteres logró leer. La función implementada básicamente lee de a un caracter.

## Comentarios
- El módulo podrá enviar datos sólo por un de los puertos UART, especificado en ``uHALconfig.h``. Esto simplifica los parámetros de las funciones.
- Lo primero a implementar y probar es la parte de envío de datos. ``uHALuartEnviarBytes ()`` envía la cantidad ``tamanio`` de bytes; y ``uHALuartEnviarCadena ()`` envía una cadena terminada en /0. 
- La función ``uHALuartRecibirBytes ()`` no tendrá tiempo de espera. Devuelve la cantidad de datos leídos. 

