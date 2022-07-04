/*
 *	Desarrollar un programa ejemplo que permita comprender el concepto y uso de “Inversión de Prioridad”.
 *	Como mínimo se deben utilizar dos tareas y una cola gestionando un recurso del microcontrolador.
 */

/*
 * 	En este ejemplo, la tarea de mediana prioridad (el pulsador) nunca se bloquea, por lo que la del LED nunca se usa.
 * 	Cuando el pulsador se aprieta, la tarea del pulsador hace que la prioridad de la tarea del LED suba a una mayor que si misma.
 * 	Esto hace que el Scheduler vaya a atender la tarea del LED que tiene mas prioridad, quien luego hacer lo que debia, se baja la priodidad
 * 	a si misma, volviendo a repetir el proceso anterior.
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

/* Prioridades para cambiar */
#define HIGH_PRIORITY	(tskIDLE_PRIORITY + 3UL)
#define MID_PRIORITY	(tskIDLE_PRIORITY + 2UL)
#define LOW_PRIORITY	(tskIDLE_PRIORITY + 1UL)

/* LED Task Handlers */
xTaskHandle LEDRhandle = NULL;

/* Declaracion de cola */
xQueueHandle queue;

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
		/* Obtengo el valor del estado del LED */
		state = Chip_GPIO_GetPinState(LPC_GPIO, LEDR_PORT, LEDR_PIN);
		/* Mando el dato por la cola */
		xQueueSendToBack(queue, &state, portMAX_DELAY);
		/* Bajo la prioridad de la tarea */
		vTaskPrioritySet(NULL, LOW_PRIORITY);
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
			/* Subo la prioridad de la tarea del LED */
			vTaskPrioritySet(LEDRhandle, HIGH_PRIORITY);
			/* Espero a que tener un valor en la cola */
			xQueueReceive(queue, &state, portMAX_DELAY);
			/* Cambio el estado del LED */
			Chip_GPIO_SetPinState(LPC_GPIO, LEDR_PORT, LEDR_PIN, !state);
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
	/* Creacion de tareas */
	xTaskCreate(LEDRtask, (const signed char *) "", configMINIMAL_STACK_SIZE, NULL, LOW_PRIORITY, LEDRhandle);
	xTaskCreate(SWtask, (const signed char *) "", configMINIMAL_STACK_SIZE, NULL, MID_PRIORITY, (xTaskHandle *) NULL);
    /* Inicia el scheduler */
	vTaskStartScheduler();

	while(1);

    return 0;
}


