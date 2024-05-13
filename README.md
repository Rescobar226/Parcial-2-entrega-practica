# Parcial-2-entrega-practica
Parcial 2 entrega practica

Enlace del video: [https://youtu.be/Jn2KLEhXJXo](https://youtu.be/Jn2KLEhXJXo)

Notas:

->El proyecto se centra en la creación de una calculadora que utiliza una pantalla de 7 segmentos y un teclado numérico para la interacción del usuario. Implementa operaciones básicas como suma, resta y multiplicación entre números de uno y dos dígitos. La lógica de la interfaz permite ingresar números y seleccionar operaciones usando el teclado. El código está diseñado para gestionar eficientemente la entrada del usuario, borrando las pantallas después de cada operación y esperando nuevas entradas.

->Se utiliza un enfoque bidimensional para almacenar las pulsaciones de teclas, lo que facilita la gestión de los datos introducidos. Además, la función atoi o una función similar se utiliza para convertir entradas de caracteres a enteros de 8 bits (uint8_t), asegurando la correcta interpretación de los datos.

->La funcionalidad de la calculadora se ha ampliado para permitir la interacción con el usuario a través de una consola serial, donde se pueden ingresar números y seleccionar operaciones a través de un menú. 

->Para garantizar un rendimiento óptimo del sistema, la frecuencia operativa de un microcontrolador se establece en 16 MHz, lo que proporciona suficiente potencia de procesamiento para manejar las operaciones requeridas y mantener una interfaz de usuario receptiva.

--Ahora bien, expliquemos la estructura del codigo y que es cada parte--

->Direcciones base:

El código define direcciones base para puertos y periféricos GPIO. ¿Qué significa eso? Cuando trabajamos con microcontroladores o dispositivos integrados, a menudo necesitamos comunicarnos con hardware externo como sensores, pantallas o incluso otros microcontroladores. Para ello necesitamos acceder a los registros de hardware específicos de estos dispositivos.

Aquí es donde entran en juego las direcciones base. Estas direcciones son como direcciones de memoria específicas que apuntan a ubicaciones físicas en el microcontrolador donde encontramos los registros que necesitamos para controlar los puertos GPIO y otros periféricos.

En el código, estas direcciones base se definen como constantes o macros. Por ejemplo, podríamos tener algo como esto: 
```c
#define GPIO_PORTA_BASE 0x40020000
#define GPIO_PORTB_BASE 0x40020400
```
Esto significa que la dirección base para el puerto GPIO A está en la dirección de memoria 0x40020000, mientras que la dirección base para el puerto GPIO B está en la dirección de memoria 0x40020400.

Ahora bien, ¿por qué son importantes estas instrucciones? Si queremos configurar un pin GPIO específico o interactuar con un dispositivo periférico como un módulo USART, necesitamos acceder a los registros de control correspondientes. Y para ello necesitamos saber dónde están estos registros. Las direcciones base nos proporcionan la información crucial.

->Estructuras de control de dispositivos:

En el código definimos estructuras para controlar dispositivos como pines GPIO, reloj RCC y comunicación USART. Estas estructuras sirven como plantillas que contienen todas las configuraciones y conjuntos de datos necesarios para cada componente. Por ejemplo, podemos tener una estructura para los pines GPIO que contenga registros de modo, tipo de salida y velocidad. Estas estructuras facilitan el acceso y la configuración de los dispositivos y simplifican así la programación.

Estructuras específicas del dispositivo:

Dentro de estas estructuras encontramos un ejemplo concreto, como es la estructura GPIO_Registers_t. Esta estructura contiene registros para modo pin, tipo de salida y velocidad, entre otros. Estos registros son cruciales para la configuración efectiva y eficiente de los pines GPIO.
```c
typedef struct {
    volatile uint32_t MODER;    // Registro de modo de pin
    volatile uint32_t OTYPER;   // Registro de tipo de salida
    volatile uint32_t OSPEEDR;  // Registro de velocidad de salida
    // Otros registros...
} GPIO_Registers_t;
```
->Gestión del tiempo:

También definimos una estructura llamada Time_t para rastrear horas, minutos y segundos. Esta estructura es útil para gestionar el tiempo en el sistema y configurar alarmas pero en este caso fue de apoyo para la multiplexación de los displays y como mostraria los numeros ingresados y su resultado.

->Alarmas y temporizadores:

Hemos creado estructuras especiales para almacenar información sobre alarmas y temporizadores. Estos son esenciales para configurar eventos y funciones cronometrados en el sistema, lo cual es útil para aplicaciones que requieren una gestión precisa del tiempo, como por ejemplo un reloj digital, que puede ser crucial.

->Macros y variables globales:

Para simplificar el acceso a los puertos GPIO y al reloj RCC, hemos definido macros. Además, declaramos variables globales para rastrear el estado del sistema, el estado de los botones y otras variables importantes. Esto nos permite rastrear el estado del sistema en cualquier momento y desde cualquier parte del código.

->Máquinas de estados:

El código se refiere a una "Secuencia de encendido" (myfsm) y una "Máquina de estado del teclado" (myfsm1). Estas máquinas de estados nos ayudan a controlar el flujo del programa y reaccionar ante ciertos eventos. Utilizamos este enfoque para simplificar la lógica de control y mejorar la modularidad del código.

->Operaciones matemáticas:

También definimos variables para almacenar números, operadores y resultados de operaciones matemáticas. Esto nos permite realizar cálculos numéricos en el sistema, lo que puede ser útil para aplicaciones que requieren procesamiento matemático.

->Interacción con botones:

El código hace referencia a una variable llamada botón, lo que sugiere que el sistema interactúa con dispositivos de entrada. Esta interacción es crucial para permitir a los usuarios controlar el sistema de forma intuitiva y sencilla, mejorando la experiencia del usuario y la usabilidad del sistema.
