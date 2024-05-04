#include <stdint.h>

// Direcciones base para puertos GPIO y periféricos
#define PORTA 0x50000000U // dirección base del puerto A
#define PORTB 0X50000400U // dirección base del puerto B
#define PORTC 0X50000800U // dirección base del puerto C
#define PERIPHERAL_BASE_ADDRESS 0X40000000U  // Dirección base para periféricos
#define AHB_BASE_ADDRESS (PERIPHERAL_BASE_ADDRESS + 0X00020000U) // dirección base AHB
#define APB1_BASE_ADDRESS (PERIPHERAL_BASE_ADDRESS + 0X00000000U) // dirección base APB1

// Dirección base RCC
#define RCC_C 0X40021000U

// dirección base USART2
#define USART2_BASE_ADDRESS ( APB1_BASE_ADDRESS + 0x00004400U)

// Recibir bit de registro de datos no vacío (RXNE) en el registro ISR USART2
#define USART_ISR_RXNE (1 << 5)

// Transmite el bit vacío del registro de datos (TXE) en el registro ISR USART2
#define USART_ISR_TXE (1 << 7)

// Estructura para registros GPIO
typedef struct {
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFR[2];
	uint32_t BRR;
} GPIO_RegDef_t;

// Estructura para registros RCC
typedef struct {
	uint32_t CR;
	uint32_t ICSCR;
	uint32_t CRRCR;
	uint32_t CFGR;
	uint32_t CIER;
	uint32_t CIFR;
	uint32_t CICR;
	uint32_t IOPRSTR;
	uint32_t AHBRSTR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t IOPENR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t IOPSMENR;
	uint32_t AHBSMENR;
	uint32_t APB2SMENR;
	uint32_t APB1SMENR;
	uint32_t CCIPR;
	uint32_t CSR;
} RCC_RegDef_t;

// Estructura para registros USART
typedef struct {
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t BRR;
	uint32_t GTPR;
	uint32_t RTOR;
	uint32_t RQR;
	uint32_t ISR;
	uint32_t ICR;
	uint32_t RDR;
	uint32_t TDR;
} USART_RegDef_t;

// Estructura para realizar un seguimiento de las variables de tiempo
struct Time_t {

	int8_t hora_decimal;
	int8_t hora_unidad;

	int8_t minuto_decimal;
	int8_t minuto_unidad;

	int8_t segundo_decimal;
	int8_t segundo_unidad;
};

// Estructura para variables de temporizador
struct alarm {

	uint8_t hora_decimal;
	uint8_t hora_unidad;

	uint8_t minuto_decimal;
	uint8_t minuto_unidad;

	uint8_t segundo_decimal;
	uint8_t segundo_unidad;
};

struct temporizador {
	uint8_t hora_decimal;
	uint8_t hora_unidad;

	uint8_t minuto_decimal;
	uint8_t minuto_unidad;

	uint8_t segundo_decimal;
	uint8_t segundo_unidad;
};

// Macros para puertos GPIO y RCC
#define GPIOA 	((GPIO_RegDef_t*)PORTA)
#define GPIOB 	((GPIO_RegDef_t*)PORTB)
#define GPIOC 	((GPIO_RegDef_t*)PORTC)
#define RCC 	((RCC_RegDef_t*)RCC_C)
#define USART2 	((USART_RegDef_t*)USART2_BASE_ADDRESS)

// Variables globales
uint8_t myfsm = 0x00; //Secuencia de encendido
uint8_t myfsm1 = 0x00; // maquina de estados del keypad
uint16_t mascara = 0x00;

int numero;
int alm_1 = { 0, 0 };  //Almacenamiento 1
int alm_2[2] = { 0, 0 };  //Almacenamiento 2
int contador = 0;
int numero_entero; // Entero decena
int Ent_unidad; //Entero Unidad
char operador;
int caso;  //Operación actual
int resultado;
int estado = 0;
int boton = 0;

int StK = 0; //Cambio entre el Serial y el Keypad

uint32_t inc_segundos;

// declaraciones de funciones
void delay_ms(uint16_t n);

uint8_t decoder(uint8_t value_to_decode);

//Numeros del display
#define digit0 0x3F
#define digit1 0X06
#define digit2 0X5B
#define digit3 0X4F
#define digit4 0X66
#define digit5 0X6D
#define digit6 0X7D
#define digit7 0X07
#define digit8 0X7F
#define digit9 0X6F
#define A 0X77
#define B 0X7C
#define C 0X39
#define D 0X5E

int main(void) {

	watch.segundo_unidad = 0x00;
	watch.segundo_decimal = 0x00;
	watch.minuto_unidad = 0x00;
	watch.minuto_decimal = 0x00;
	watch.hora_unidad = 0x00;
	watch.hora_decimal = 0x00;

	char option;
	int num1, num2;
	struct Time_t watch;

	//Puerto A

	RCC->IOPENR |= 1 << 0;

	GPIOA->MODER &= ~(1 << 0); //PIN A0
	GPIOA->MODER &= ~(1 << 1);
	GPIOA->PUPDR &= ~(1 << 0); //RESISTENCIA DE PULLDOWN
	GPIOA->PUPDR |= 1 << 1;

	// CONFIGURACION USART 2
	GPIOA->MODER &= ~(1 << 4);
	GPIOA->MODER &= ~(1 << 6);

	// AF4
	GPIOA->AFR[0] |= 1 << 10;  // Mapeos
	GPIOA->AFR[0] |= 1 << 14;

	// Periferico del USART2 y RCC
	RCC->APB1ENR |= 1 << 17;

	// Tasa de 9600 bits - 16 MHz
	USART2->BRR = 218;

	// Trasmisor y receptor
	USART2->CR1 |= (1 << 2) | (1 << 3);
	USART2->CR1 |= 1 << 0;

	//Puerto B
	RCC->IOPENR |= 1 << 1;

	//Display de 7 segmentos multiplexado

	GPIOB->MODER |= 1 << 0;  //PIN B0
	GPIOB->MODER &= ~(1 << 1);

	GPIOB->MODER |= 1 << 2;	//PIN B1
	GPIOB->MODER &= ~(1 << 3);

	GPIOB->MODER |= 1 << 4;	//PIN B2
	GPIOB->MODER &= ~(1 << 5);

	GPIOB->MODER |= 1 << 6;	//PIN B3
	GPIOB->MODER &= ~(1 << 7);

	GPIOB->MODER |= 1 << 8;	//PIN B4
	GPIOB->MODER &= ~(1 << 9);

	GPIOB->MODER |= 1 << 10;	//PIN B5
	GPIOB->MODER &= ~(1 << 11);

	GPIOB->MODER |= 1 << 12;	//PIN B6
	GPIOB->MODER &= ~(1 << 13);

	GPIOB->MODER |= 1 << 14;	//PIN B7
	GPIOB->MODER &= ~(1 << 15);

	// Entradas del Keypad
	GPIOB->MODER &= ~(1 << 16); // PIN PB8
	GPIOB->MODER &= ~(1 << 17);

	GPIOB->PUPDR |= 1 << 16; //Resistencia de pull-up PB8
	GPIOB->PUPDR &= ~(1 << 17);

	GPIOB->MODER &= ~(1 << 18); //PIN PB9
	GPIOB->MODER &= ~(1 << 19);

	GPIOB->PUPDR |= 1 << 18; //Resistencia de pull-up PB9
	GPIOB->PUPDR &= ~(1 << 19);

	GPIOB->MODER &= ~(1 << 20); //PIN PB10
	GPIOB->MODER &= ~(1 << 21);

	GPIOB->PUPDR |= 1 << 20; //Resistencia de pull-up PB10
	GPIOB->PUPDR &= ~(1 << 21);

	GPIOB->MODER &= ~(1 << 22); // PB11
	GPIOB->MODER &= ~(1 << 23);

	GPIOB->PUPDR |= 1 << 22; //Resistencia de pull-up PB11
	GPIOB->PUPDR &= ~(1 << 23);

	// Salidas del Keypad

	GPIOB->MODER |= 1 << 24;	//PIN B12
	GPIOB->MODER &= ~(1 << 25);

	GPIOB->MODER |= 1 << 26;	//PIN B13
	GPIOB->MODER &= ~(1 << 27);

	GPIOB->MODER |= 1 << 28;	//PIN B14
	GPIOB->MODER &= ~(1 << 29);

	GPIOB->MODER |= 1 << 30;	//PIN B15
	GPIOB->MODER &= ~(1 << 31);

	//Control de display multiplexado
	RCC->IOPENR |= 1 << 2;

	GPIOC->MODER |= 1 << 10;  //PIN C5
	GPIOC->MODER &= ~(1 << 11);

	GPIOC->MODER |= 1 << 12;  //PIN C6
	GPIOC->MODER &= ~(1 << 13);

	GPIOC->MODER |= 1 << 16;  //PIN C8
	GPIOC->MODER &= ~(1 << 17);

	GPIOC->MODER |= 1 << 18;  //PIN C9
	GPIOC->MODER &= ~(1 << 19);

	GPIOC->MODER |= 1 << 20;  //PIN C10
	GPIOC->MODER &= ~(1 << 21);

	GPIOC->MODER |= 1 << 22;  //PIN C11
	GPIOC->MODER &= ~(1 << 23);

	while (1) {

		//Display keypad
		if (StK == 0) {

			switch (myfsm1) {
			case 0: {
				//Columna 0
				myfsm1++;
				delay_ms(1);
				GPIOB->ODR |= 1 << 15; // APAGANDO PB15
				GPIOB->ODR &= ~(1 << 12); //HABILITANDO PB12
				mascara = GPIOB->IDR & 0XF00;
				if (mascara == 0xE00) {
					operador = '+';  // A
					numero = 100;
					caso = 1;
					boton = 0;

				} else if (mascara == 0xD00) {
					operador = '-'; //B
					numero = 100;
					caso = 2;
					boton = 0;
				} else if (mascara == 0xB00) {
					operador = '*'; //C
					numero = 100;
					caso = 3;
					boton = 0;
				} else if (mascara == 0X700) { //0111
					operador = '='; //D
					boton = 0;

				}

				break;

			}
			case 1: {
				//PARA COLUMNA 1
				myfsm1++;
				GPIOB->ODR |= 1 << 12; // APAGANDO PB12
				GPIOB->ODR &= ~(1 << 13); //HABILITANDO PB13
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00) { //1110
					numero = 3; //NUMERO 3
					operador = '/';
					boton = 1;
				} else if (mascara == 0xD00) { //1101
					//NUMERO 6
					numero = 6;
					operador = '/';
					boton = 1;
				} else if (mascara == 0xB00) { //1011
					// NUMERO 9
					numero = 9;
					operador = '/';
					boton = 1;
				} else if (mascara == 0X700) { //0111
					//NUM
					StK = 1;
				}

				break;
			}
			case 2: {
				//PARA COLUMNA 2
				myfsm1++;
				GPIOB->ODR |= 1 << 13; // APAGANDO PB13
				GPIOB->ODR &= ~(1 << 14); //HABILITANDO PB14
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00) { //1110
					//NUMERO 2
					numero = 2;
					operador = '/';
					boton = 1;

				} else if (mascara == 0xD00) { //1101
					//NUMERO 5
					numero = 5;
					operador = '/';
					boton = 1;

				} else if (mascara == 0xB00) { //1011
					//NUMERO 8
					numero = 8;
					operador = '/';
					boton = 1;

				} else if (mascara == 0X700) { //0111
					//NUMERO 0
					numero = 0;
					operador = '/';
					boton = 1;

				}

				break;
			}

			case 3: {
				//PARA COLUMNA 3
				myfsm1 = 0;

				GPIOB->ODR |= 1 << 14; // APAGANDO PB14
				GPIOB->ODR &= ~(1 << 15); //HABILITANDO PB15
				mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
				if (mascara == 0xE00) { //1110
					//NUMERO 1
					numero = 1;
					boton = 1;
					operador = '/';

				} else if (mascara == 0xD00) { //1101
					//NUMERO 4
					numero = 4;
					boton = 1;
					operador = '/';

				} else if (mascara == 0xB00) { //1011
					//NUMERO 7
					numero = 7;
					operador = '/';
					boton = 1;
				} else if (mascara == 0X700) { //0111
					//NUMERO *
					contador = 0;
					caso = 0;
					estado = 0;
					numero = 100;
					alm_1[0] = 0;
					alm_1[1] = 0;
					alm_2[0] = 0;
					alm_2[1] = 0;
					operador = '/';
					watch.segundo_decimal = 0;
					watch.segundo_unidad = 0;

				}

				break;
			}
			default: {
				myfsm1 = 0;

				break;
			}

			}

			//Calculadora

			if (mascara != 0XF00 && boton == 1) { //Numero ingresado
				delay_ms(500);
				if (estado == 0) {
					if (numero >= 0 && numero <= 9) {
						alm_1[contador] = numero;

						watch.segundo_decimal = alm_1[0];
						watch.segundo_unidad = alm_1[1];
						contador++; //AUMENTA PARA ALMACENAR SIGUIENTE NUMERO PRESIONADO
					}
				}
				if (estado == 1) {
					if (numero >= 0 && numero <= 9) {
						alm_2[contador] = numero;

						watch.segundo_decimal = alm_2[0];
						watch.segundo_unidad = alm_2[1];
						contador++; //AUMENTA PARA ALMACENAR SIGUIENTE NUMERO PRESIONADO
					}
				}

				if (contador > 1) {
					contador = 0;
				}
			}

			switch (operador) {
			case '+':

				Ent_decena = alm_1[0] * 10 + alm_1[1]; // CONVERTIR ARREGLO EN NUMERO ENTERO
				contador = 0;
				caso = 1;
				estado = 1;
				watch.segundo_decimal = 0;
				watch.segundo_unidad = 0;
				operador = '/';

				break;
			case '-':

				Ent_decena = alm_1[0] * 10 + alm_1[1]; // CONVERTIR ARREGLO EN NUMERO ENTERO
				contador = 0;
				caso = 2;
				estado = 1;
				watch.segundo_decimal = 0;
				watch.segundo_unidad = 0;
				operador = '/';
				break;
			case '*':
				Ent_decena = alm_1[0] * 10 + alm_1[1]; // CONVERTIR ARREGLO EN NUMERO ENTERO
				contador = 0;
				caso = 3;
				estado = 1;
				watch.segundo_decimal = 0;
				watch.segundo_unidad = 0;
				operador = '/';

				break;
			case '=':
				if (caso == 1) {

					Ent_unidad = alm_2[0] * 10 + alm_2[1];
					resultado = Ent_decena + Ent_unidad;
					if (resultado >= 0 && resultado <= 99) {
						int parteEntera = resultado / 10;
						int parteDecimal = resultado % 10;

						watch.segundo_unidad = parteDecimal;
						watch.segundo_decimal = parteEntera;
						contador = 0;
						caso = 0;
						estado = 0;
						numero = 100;
						alm_1[0] = 0;
						alm_1[1] = 0;
						alm_2[0] = 0;
						alm_2[1] = 0;
						operador = '/';
					} else {
						watch.segundo_unidad = 0;
						watch.segundo_decimal = 0;
						contador = 0;
						caso = 0;
						estado = 0;
						numero = 100;
						alm_1[0] = 0;
						alm_1[1] = 0;
						alm_2[0] = 0;
						alm_2[1] = 0;
						operador = '/';
					}
				}
				if (caso == 2) {
					Ent_unidad = alm_2[0] * 10 + alm_2[1];
					resultado = Ent_decena - Ent_unidad;
					if (resultado >= 0 && resultado <= 99) {
						int parteEntera = resultado / 10;
						int parteDecimal = resultado % 10;

						watch.segundo_unidad = parteDecimal;
						watch.segundo_decimal = parteEntera;
						caso = 0;
						estado = 0;
						numero = 100;
						alm_1[0] = 0;
						alm_1[1] = 0;
						alm_2[0] = 0;
						alm_2[1] = 0;
						operador = '/';
					} else {
						watch.segundo_unidad = 0;
						watch.segundo_decimal = 0;
						caso = 0;
						estado = 0;
						numero = 100;
						alm_1[0] = 0;
						alm_1[1] = 0;
						alm_2[0] = 0;
						alm_2[1] = 0;
						operador = '/';

					}
				}
				if (caso == 3) {
					Ent_unidad = alm_2[0] * 10 + alm_2[1];
					resultado = Ent_decena * Ent_unidad;
					if (resultado >= 0 && resultado <= 99) {
						int parteEntera = resultado / 10;
						int parteDecimal = resultado % 10;

						watch.segundo_unidad = parteDecimal;
						watch.segundo_decimal = parteEntera;
						caso = 0;
						estado = 0;
						numero = 100;
						alm_1[0] = 0;
						alm_1[1] = 0;
						alm_2[0] = 0;
						alm_2[1] = 0;
						operador = '/';
					} else {
						watch.segundo_unidad = 0;
						watch.segundo_decimal = 0;
						caso = 0;
						estado = 0;
						numero = 100;
						alm_1[0] = 0;
						alm_1[1] = 0;
						alm_2[0] = 0;
						alm_2[1] = 0;
						operador = '/';

					}

				}

			}
		}
		// StK = 0

		//Serial

		if (StK == 1) {
			watch.segundo_decimal = 0;
			watch.segundo_unidad = 0;
			void UART_SendChar(char c) {
				while (!(USART2->ISR & USART_ISR_TXE))
					; // Transmisión esté vacío
				USART2->TDR = c; // Enviar el carácter
			}

			// Función para recibir un carácter por la UART
			char UART_ReceiveChar() {
				while (!(USART2->ISR & USART_ISR_RXNE))
					; // Llegue un byte
				return USART2->RDR; // Devolver el byte
			}

			// Función para enviar una cadena de caracteres por la UART
			void UART_SendString(const char *str) {
				while (*str != '\0') {
					UART_SendChar(*str++);
				}
			}

			// Menú de operaciones
			void printMenu() {
				UART_SendString("---- Menu de Operaciones ----\r\n");
				UART_SendString("1. Suma\r\n");
				UART_SendString("2. Resta\r\n");
				UART_SendString("3. Multiplicación\r\n");
				UART_SendString("Seleccione una opción: ");
			}
			printMenu();

			option = UART_ReceiveChar();
			UART_SendChar(option); // Opción seleccionada
			int readTwoDigitNumber() {
				int firstDigit = UART_ReceiveChar() - '0'; // Convertir el primer carácter a número
				UART_SendChar(firstDigit + '0'); // Mostrar el primer dígito ingresado
				int secondDigit = UART_ReceiveChar() - '0'; // Convertir el segundo carácter a número
				UART_SendChar(secondDigit + '0'); // Mostrar el segundo dígito ingresado

				return firstDigit * 10 + secondDigit; // Combinar los dos dígitos en un número de dos dígitos
			}
			UART_SendString("\r\nIngrese el primer numero: ");
			num1 = readTwoDigitNumber(); // Leer el primer número de dos dígitos

			UART_SendString("\r\nIngrese el segundo numero: ");
			num2 = readTwoDigitNumber(); // Leer el segundo número de dos dígitos

			int produc;
			switch (option) {
			case '1': // Suma
				produc = num1 + num2;
				break;
			case '2': // Resta
				produc = num1 - num2;
				break;
			case '3': // Multi
				produc = num1 * num2;
				break;
			default:
				produc = 0; // Invalido
				break;
			}
			watch.segundo_unidad = produc;
			UART_SendString("\r\n El resultado es igual a:  ");

			char val = produc + '0';
			UART_SendChar(val);
			UART_SendString("\r\n ");

			UART_SendString("\r\n Continuar en el keypad presione: 0");
			UART_SendString("\r\n Continuar en la consola serial presione: 1");
			UART_SendString("\r\n ");
			StK = UART_ReceiveChar() - '0';
			if (StK == 0) {
				watch.segundo_unidad = produc;
				watch.segundo_decimal = produc;
			}

		}

		if (StK == 1) {
			myfsm = 0;
		}

		switch (myfsm) {

		case 0: {
			if (StK == 0) {
				myfsm++;
			} else if (StK == 1) {
				myfsm = 0;
			}
			GPIOB->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
			GPIOC->BSRR |= 0X01 << 27; // LIMPIAR D5/C11
			GPIOC->BSRR |= 0X01 << 5; // HABILITA D0/C5
			GPIOB->BSRR |= decoder(watch.segundo_unidad) << 0; // MANDA A PINTAR DISPLAY 1

			break;
		}

		case 1: {
			if (StK == 0) {
				myfsm++;
				GPIOB->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
				GPIOC->BSRR |= 0X01 << 21; // LIMPIAR D0/C5
				GPIOC->BSRR |= 0X01 << 6; // HABILITA D1/C6
				GPIOB->BSRR |= decoder(watch.segundo_decimal) << 0; //MANDA A PINTAR DISPLAY 2
			} else if (StK == 1) {
				myfsm = 0;
			}

			break;
		}

		case 2: {
			if (StK == 0) {
				myfsm++;
				GPIOB->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
				GPIOC->BSRR |= 0X01 << 22; // LIMPIAR D1/C6
				GPIOC->BSRR |= 0X01 << 8; // HABILITA D2/C8
				GPIOB->BSRR |= decoder(watch.minuto_unidad) << 0; //MANDA A PINTAR DISPLAY 3
			}

			else if (StK == 1) {
				myfsm = 0;
			}
			break;
		}

		case 3: {
			if (StK == 0) {
				myfsm++;
				GPIOB->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
				GPIOC->BSRR |= 0X01 << 24; // LIMPIAR D2/C8
				GPIOC->BSRR |= 0X01 << 9; // HABILITA D3/C9
				GPIOB->BSRR |= decoder(watch.minuto_decimal) << 0; //MANDA A PINTAR DISPLAY 4
			} else if (StK == 1) {
				myfsm = 0;
			}

			break;

		}
		case 4: {
			if (StK == 0) {
				myfsm++;
				GPIOB->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
				GPIOC->BSRR |= 0X01 << 25; // LIMPIAR D3/C9
				GPIOC->BSRR |= 0X01 << 10; // HABILITA D4/C10
				GPIOB->BSRR |= decoder(watch.hora_unidad) << 0; //MANDA A PINTAR DISPLAY 4
			} else if (StK == 1) {
				myfsm = 0;
			}

			break;

		}

		case 5: {
			if (StK == 0) {
				myfsm = 0;
				GPIOB->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
				GPIOC->BSRR |= 0X01 << 26; // LIMPIAR D4/C10
				GPIOC->BSRR |= 0X01 << 11; // HABILITA D5/C11
				GPIOB->BSRR |= decoder(watch.hora_decimal) << 0; //MANDA A PINTAR DISPLAY 4
			} else if (StK == 1) {
				myfsm = 0;
			}

			break;
		}

		default: {
			//Seguro para el myfsm
			myfsm = 0;
			break;
		}

		}

	}

}

void delay_ms(uint16_t n) {
// para 2.097 140
	// para 16MHZ 3195
	uint32_t i;
	for (; n > 0; n--)
		for (i = 0; i < 140; i++)
			;
}
uint8_t decoder(uint8_t value_to_decode) {
	switch (value_to_decode) {
	case 0: {
		return (digit0);

	}
	case 1: {

		return (digit1);
	}
	case 2: {

		return (digit2);
	}
	case 3: {

		return (digit3);
	}
	case 4: {

		return (digit4);
	}
	case 5: {

		return (digit5);
	}

	case 6: {

		return (digit6);
	}

	case 7: {

		return (digit7);
	}

	case 8: {

		return (digit8);
	}
	case 9: {

		return (digit9);
	}
	case 10: {
		return (A);
	}
	case 11: {
		return (B);
	}
	case 12: {
		return (C);
	}
	case 13: {
		return (D);
	}

	default: {

	}
		return 0x00;
	}
}
