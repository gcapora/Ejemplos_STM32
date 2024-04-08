/*******************************************************************************
  * @brief  Lee UART porque espera recibir la señal: números separados por coma
  * @param  Estructura de datos del generador
  * @retval None
  */
static void Leer_UART(void){
	static uint8_t Buffer[LARGO_MAX_PAQUETE];	//Aquí guardo los caracteres antes de mandar el paquete
	static uint8_t Buffer_pos=0;
	uint8_t Leo;

	while (uartReceiveStringSize( &Leo, 1) == true) {
		// Mientras esté recibiendo por UART me quedo en este loop

		if ( Leo>=48 && Leo<=57 ) {
			// Es un dígito numérico: Debo almacenarlo en el Buffer
			Buffer[Buffer_pos] = Leo;
			Buffer_pos++;
			Buffer_pos = (Buffer_pos >= LARGO_MAX_PAQUETE) ? (LARGO_MAX_PAQUETE - 1) : Buffer_pos;
			// Esto último evita corromper la memoria si desde la terminal mandan formatos incorrectos

		} else if (Leo == ',') {
			// Es el fin de un paquete: Debo cargarlo en la Senial
			// 1) Le damos formato de string (debe terminar en \0)
			for (uint16_t i=Buffer_pos; i<LARGO_MAX_PAQUETE; i++) Buffer[i] = '\0';
			// 2) Enviamos a cargarlo...
			Cargar_Paquete(Buffer);
			Buffer_pos=0;

		}   // Si no es número ni ',', no lo considero.
	}
}