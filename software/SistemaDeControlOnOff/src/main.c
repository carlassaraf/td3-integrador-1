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
#include "proj_tasks.h"

extern xQueueHandle queueADC;

int main(void) {

    SystemCoreClockUpdate();
    Board_Init();
    /* Inicializo ADC */
    adc_init();
    /* Inicializo los 7 Segmentos */
    gpio_7segments_init();

    /* Inicializo SPI */
    SPI_Inicializar();

    /* Creo la cola para los datos del ADC */
    queueADC = xQueueCreate(1, sizeof(uint16_t));

	xTaskCreate(
		lm35Task,								/* Callback para la tarea */
		(const signed char *) "LM35 Task",		/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
		NULL,									/* Sin parametros */
		tskLM35_PRIORITY,						/* Prioridad */
		NULL									/* Sin handler */
	);

	xTaskCreate(
		displayTask,							/* Callback para la tarea */
		(const signed char *) "7 Segment Task",	/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
		NULL,									/* Sin parametros */
		tsk7SEG_PRIORITY,						/* Prioridad */
		NULL									/* Sin handler */
	);

    /* Creacion de tareas */
	xTaskCreate(
		sdWriteTask, 							/* Callback para la tarea */
		(const signed char *) "Temporizacion",	/* Nombre de la tarea para debugging */
    	configMINIMAL_STACK_SIZE, 				/* Minimo stack para la tarea */
		NULL, 									/* Sin parametros */
		tskSDWRITE_PRIOTITY,					/* Prioridad */
		NULL									/* Sin handler */
	);

	/* Inicio el scheduler */
	vTaskStartScheduler();

    while(1);

    return 0 ;
}

/* Handler para las interrupciones del ADC */
void ADC_IRQHandler(void) {
	/* Control para cambiar de tarea al salir de la interrupcion */
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	/* Variable auxiliar */
	uint16_t temp;
	/* Obtengo el valor del ADC */
	adc_read(temp);
	/* Ingreso el valor a la cola */
	xQueueSendToBackFromISR(queueADC, &temp, &xHigherPriorityTaskWoken);
	/* Solicitar un cambio de contexto si fuese necesario */
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

