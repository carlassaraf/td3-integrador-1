/*
 *	Desarrollar un programa ejemplo que permita comprender el concepto y uso de “Punto muerto”.
 *	Como mínimo se deben utilizar dos tareas y una cola gestionando un recurso del microcontrolador.
 */

/*
 * 	En este ejemplo, la tarea del pulsador toma de la queue para luego operar con ese dato y hacer un give al mutex.
 * 	Mientras, la tarea del LED debe hace un take del mutex para luego escribir en la cola.
 * 	Este orden de acciones hace que se llegue a un punto muerto
 */

#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Definiciones de puertos y pines */
#define LEDB_PORT 3
#define LEDG_PORT 3
#define LEDR_PORT 0

#define LEDB_PIN 26
#define LEDG_PIN 25
#define LEDR_PIN 22

#define SW_PORT 0
#define SW_PIN 0

/* Declaracion de cola */
xQueueHandle queue;
/* Declaracion de semaforos */
xSemaphoreHandle mutex;

/*
 * 	@brief	Inicializacion del microcontrolador
 */
void init(void){
	/* Inicializacion del GPIO */
	Chip_GPIO_Init(LPC_GPIO);
	/* Inicializo el pulsador */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SW_PORT, SW_PIN);
	/* Inicializo los LEDs */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LEDB_PORT, LEDB_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LEDG_PORT, LEDG_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LEDR_PORT, LEDR_PIN);
	/* Los inicializo apagados */
	Chip_GPIO_SetPinState(LPC_GPIO, LEDB_PORT, LEDB_PIN, TRUE);
	Chip_GPIO_SetPinState(LPC_GPIO, LEDG_PORT, LEDG_PIN, TRUE);
	Chip_GPIO_SetPinState(LPC_GPIO, LEDR_PORT, LEDR_PIN, TRUE);
}

/*
 * 	@brief	Tarea del LED rojo
 */
static void LEDRtask(void *pvParameters){
	/* Variable para mandar a la cola */
	bool state;
	/* Mando el dato por la cola */
	while(1) {
		/* Tomo del semaforo */
		xSemaphoreTake(mutex, portMAX_DELAY);
		/* Obtengo el valor del estado del LED */
		state = Chip_GPIO_GetPinState(LPC_GPIO, LEDR_PORT, LEDR_PIN);
		/* Mando el dato por la cola */
		xQueueSendToBack(queue, &state, portMAX_DELAY);
	}
}

/*
 * 	@brief	Tarea del pulsador
 */
static void SWtask(void *pvParameters){
	/* Variable para recibir de la cola */
	bool state;

	while(1) {
		/* Reviso el estado del pulsador */
		if(!Chip_GPIO_GetPinState(LPC_GPIO, SW_PORT, SW_PIN)) {
			/* Espero a que tener un valor en la cola */
			xQueueReceive(queue, &state, portMAX_DELAY);
			/* Cambio el estado del LED */
			Chip_GPIO_SetPinState(LPC_GPIO, LEDR_PORT, LEDR_PIN, !state);
			/* Escribo el semaforo */
			xSemaphoreGive(mutex);
		}
	}
}

/*
 * 	@brief	Programa principal
 */
int main(void){

	SystemCoreClockUpdate();
	/* Inicializo MCU */
	init();
	/* Inicializo cola */
	queue = xQueueCreate(1, sizeof(bool));
	/* Inicializo el mutex */
	mutex = xSemaphoreCreateMutex();
	/* Creacion de tareas */
	xTaskCreate(LEDRtask, (const signed char *) "", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
	xTaskCreate(SWtask, (const signed char *) "", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
    /* Inicia el scheduler */
	vTaskStartScheduler();

	while(1);

    return 0;
}
