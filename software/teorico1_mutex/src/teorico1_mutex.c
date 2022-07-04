/*
 * Desarrollar un programa ejemplo que permita comprender el concepto y uso de “Semáforos Mutex"
 */

#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Definiciones de puertos y pines */
#define LEDB_PORT 3
#define LEDG_PORT 3
#define LEDR_PORT 0

#define LEDB_PIN 26
#define LEDG_PIN 25
#define LEDR_PIN 22

#define SW1_PORT 0
#define SW1_PIN 0

/* Declaracion del semaforo */
xSemaphoreHandle mutex;

/*
 * 	@brief	Inicializo el MCU
 */
void init(void){
	/* Inicializo el GPIO */
	Chip_GPIO_Init(LPC_GPIO);
	/* Inicializo el switch */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SW1_PORT, SW1_PIN);
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
 *	@brief	Tarea que controla el LED azul
 */
static void ledTask(void *pvParameters){

	while(1) {
		/* Espero a que haya dato en semaforo */
		xSemaphoreTake(mutex, portMAX_DELAY);
		/* Blinkeo tres veces el LED azul */
		for(int i=0; i <= 3; i++) {
			Chip_GPIO_SetPinState(LPC_GPIO, LEDB_PORT, LEDB_PIN, !Chip_GPIO_GetPinState(LPC_GPIO, LEDB_PORT, LEDB_PIN));
			/* Demora */
			vTaskDelay(500 / portTICK_RATE_MS);
		}			
	}
}

/*
 * 	@brief	Tarea para el switch que controla el LED azul
 */
static void switchTask(void *pvParameters) {

	while(1) {
		/* Reviso el estado del pulsador */
		if(!Chip_GPIO_GetPinState(LPC_GPIO, SW1_PORT, SW1_PIN)) {
			/* Pongo dato en el semaforo */
			xSemaphoreGive(mutex);
			/* Bloqueo por un tiempo */
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	}
}

/*
 * 	@brief	Programa principal
 */
int main(void){

	SystemCoreClockUpdate();
	/* Inicializo el MCU */
	init();
	/* Inicializo el mutex */
	mutex = xSemaphoreCreateMutex();
    /* Inicializo tareas */
	xTaskCreate(ledTask, (const signed char *) "", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
	xTaskCreate(switchTask, (const signed char *) "", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
    /* Inicia el scheduler */
	vTaskStartScheduler();

	while(1);

    return 0;
}


