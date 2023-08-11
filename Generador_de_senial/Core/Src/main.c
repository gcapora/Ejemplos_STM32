/*******************************************************************************
  * @file		main.c
  * @brief      Generador de señales con entrada por UART
  * @author		Guillermo F. Caporaletti <gfcaporaletti@undav.edu.ar>
  * @detail		Recibe una entrada por UART y la saca por DAC.
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/
#define LARGO_PAQUETE		5		// Paquete de datos recibidos por UART
#define LARGO_MAX_PAQUETE	16		// Lo admisible ante error de transmisión

/* Private typedef -----------------------------------------------------------*/

/* Variables privadas ------- ------------------------------------------------*/
uint32_t Senial[N_MUESTRAS] = {
		0,6,19,39,66,100,142,190,244,306,373,
		447,526,611,700,795,894,997,1104,1215,1328,1444,1562,1682,1803,1925,
		2048,2170,2292,2413,2533,2651,2767,2880,2991,3098,3201,3300,3395,3484,3569,
		3648,3722,3789,3851,3905,3953,3995,4029,4056,4076,4089,4095,4093,4084,4067,
		4044,4013,3975,3930,3879,3821,3756,3686,3609,3527,3440,3348,3251,3150,3045,
		2936,2824,2709,2592,2473,2353,2231,2109,1986,1864,1742,1622,1503,1386,1271,
		1159,1050,945,844,747,655,568,486,409,339,274,216,165,120,82,
		51,28,11,2
};								// Senial inicial y donde se almacenarán nuevas señales
uint16_t MuestraNro = 0;		// Posición en que se va a almacenar próxima muestra

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void Leer_UART(void);	// <-- Esta rutina lee de a un caracter
static void Cargar_Paquete(uint8_t PaqueteRecibido[]);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Inicializacion de periféricos y APIs -------------------------------------*/
  Inicializar_DAC_DMA();					// DAC con acceso DMA utilizando Timer 2
  if (uartInit() != true) Error_Handler();	// Conexión con terminal
  debounceFSM_init();						// MEF antirrebote del pulsador de usuario
  Gen_Init();								// Inicialización del generador de señal

  /* Inicio... ----------------------------------------------------------------*/
  uartSendString((uint8_t *) "\nGENERADOR DE SENIAL V1.0\nTiempo entre muestras: 10,5 Msps\nTension: 0V - 3,3V");
  uartSendString((uint8_t *) "\n\n");
  Gen_Espera();	// Estado inicial del generador

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Reviso botón de usuario:
	  debounceFSM_update();

	  // Reviso parpadeo de leds
	  Gen_Actualiza_Leds();

	  // Si estoy en estado = Recibiendo o Cargado, espero recibir una señal
	  if (Gen_Estado() == Recibiendo || Gen_Estado() == Cargado) {
          Leer_UART();
	  }

	  // Si presionamos el botón de usuario cambiamos de estadoMEF
	  if (readKeyPush()) {
		  switch (Gen_Estado()) {

		  case Espera:
			  Gen_Recibir();
			  break;

		  case Recibiendo:
			  // Para salir de este estado, se deben cargar los datos por UART
			  break;

		  case Cargado:
			  Gen_Encender();
			  break;

		  case Generando:
			  Gen_Pausar();
			  break;

		  case Pausa:
			  Gen_Encender();
			  break;

		  default:
			  // nada...
			  break;
		  }
	  }

	  // Si hay flanco descendente (señal lógica de 1 a 0), evalúo si fue un pulso largo:
	  if (readKeyRelease()) {
		  if ((readPresionadoLargo() == true) &&  Gen_Estado() >= Generando)
			  Gen_Espera();
	  }
   }
}

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

/*******************************************************************************
  * @brief  Carga una muestra en la Senial (que luego se pasará al generador)
  * @param  Estructura de datos del generador
  * @retval None
  */
static void Cargar_Paquete(uint8_t PaqueteRecibido[]) {
	// PaqueteRecibido es un string?
	uint32_t Largo = strlen((char *) PaqueteRecibido);
	if ( Largo > LARGO_MAX_PAQUETE) Error_Handler();

	// Transformamos string recibido en número y lo asignamos a Senial[]
	uint32_t Numero = 0;
	Numero = atoi((char *) PaqueteRecibido );
	Senial[MuestraNro] = Numero;

	// Informamos en UART
	char muestra_str[16];
	sprintf(muestra_str, "%u", MuestraNro);
	uartSendString((uint8_t *) "Muestra #");
	uartSendString((uint8_t *) muestra_str);
	uartSendString((uint8_t *) ": ");
	uartSendString((uint8_t *) PaqueteRecibido);
	uartSendString((uint8_t *) "\n");

	// Incrementamos para próxima carga y verificamos si ya completamos
	MuestraNro++;
	if (MuestraNro >= N_MUESTRAS) {
		MuestraNro = 0;
		Gen_Cargar(Senial);
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}
