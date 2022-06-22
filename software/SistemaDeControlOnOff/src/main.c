/*
===============================================================================
 Name        : SistemaDeControlOnOff.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "adc.h"

#define tskLM35_PRIORITY	tskIDLE_PRIORITY + 1UL

/* Cola para el ADC */
xQueueHandle queueADC;
/* Handle para el semaforo */
xSemaphoreHandle semaphoreIRQ;

void lm35Task(void *params) {
	/* Delay entre conversiones */
	const uint16_t DELAY_MS = 500;

	while(1) {
		/* Inicio la conversion */
		adc_start();
		/* Bloqueo la tarea hasta que termine la interrupcion */
		xSemaphoreTake(semaphoreIRQ, portMAX_DELAY);
		/* Bloqueo por un tiempo hasta empezar otra conversion*/
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

int main(void) {

    SystemCoreClockUpdate();
    Board_Init();
    /* Inicializo ADC */
    adc_init();

    /* Creo la cola para los datos del ADC */
    queueADC = xQueueCreate(1, sizeof(uint16_t));
    /* Creo el semaforo */
    vSemaphoreCreateBinary(semaphoreIRQ);

	xTaskCreate(
		lm35Task,								/* Callback para la tarea */
		(const signed char *) "LM35 Task",		/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
		NULL,									/* Sin parametros */
		tskLM35_PRIORITY,						/* Prioridad */
		NULL									/* Sin handler */
	);

	/* Inicio el scheduler */
	vTaskStartScheduler();

    while(1) {

    }
    return 0 ;
}

/* Handler para las interrupciones del ADC */
void ADC0_IRQHandler(void) {
	/* Control para cambiar de tarea al salir de la interrupcion */
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	/* Variable auxiliar */
	uint16_t temp;
	/* Obtengo el valor del ADC */
	adc_read(temp);
	/* Ingreso el valor a la cola */
	xQueueSendToBack(queueADC, &temp, 0);
	/* Aviso que la interrupcion termino */
	xSemaphoreGiveFromISR(semaphoreIRQ, &xHigherPriorityTaskWoken);
	/* Solicitar un cambio de contexto si fuese necesario */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

