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

#define tskLM35_PRIORITY	tskIDLE_PRIORITY + 1UL

void lm35Task(void) {


	while(1) {

	}
}

int main(void) {

    SystemCoreClockUpdate();
    Board_Init();

	xTaskCreate(
		lm35Task,					/* Callback para la tarea */
		"LM35 Task",				/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,	/* Minimo stack para la tarea */
		NULL,						/* Sin parametros */
		tskLM35_PRIORITY,			/* Prioridad */
		NULL						/* Sin handler */
	);

	/* Inicio el scheduler */
	vTaskStartScheduler();

    while(1) {

    }
    return 0 ;
}
