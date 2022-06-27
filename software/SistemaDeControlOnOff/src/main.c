/*
===============================================================================
 Name        : SistemaDeControlOnOff.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "proj_tasks.h"

/*
 * 	@brief	Programa principal
 */
int main(void) {
	/* Actualizo el clock del sistema */
    SystemCoreClockUpdate();
    /* Creo la cola para la temperatura medida */
    queueTEMP = xQueueCreate(1, sizeof(float));
    /* Creo la cola para los datos del setpoint */
    queueSP = xQueueCreate(1, sizeof(float));
    /* Registro las colas para debuggear */
    vQueueAddToRegistry( queueTEMP, (signed char *) "Cola de temperatura");
    vQueueAddToRegistry( queueSP, (signed char *) "Cola de SP");

    /* Creacion de tareas */
    xTaskCreate(
		initTask,								/* Callback para la tarea */
		(const signed char *) "Init Task",		/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
		NULL,									/* Sin parametros */
		tskINIT_PRIORITY,						/* Prioridad */
		NULL									/* Sin handler */
    );

	xTaskCreate(
		btnTask, 								/* Callback para la tarea */
		(const signed char *) "Botones",		/* Nombre de la tarea para debugging */
    	configMINIMAL_STACK_SIZE, 				/* Minimo stack para la tarea */
		NULL, 									/* Sin parametros */
		tskBTN_PRIORITY,						/* Prioridad */
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

	xTaskCreate(
		lm35Task,								/* Callback para la tarea */
		(const signed char *) "LM35 Task",		/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
		NULL,									/* Sin parametros */
		tskLM35_PRIORITY,						/* Prioridad */
		NULL									/* Sin handler */
	);

	//xTaskCreate(
	//	sdWriteTask, 							/* Callback para la tarea */
	//	(const signed char *) "Escritura SD",	/* Nombre de la tarea para debugging */
	//	configSD_TASK_SIZE, 				    /* Minimo stack para la tarea */
	//	NULL, 									/* Sin parametros */
	//	tskSDWRITE_PRIOTITY,					/* Prioridad */
	//	NULL									/* Sin handler */
	//);

	xTaskCreate(
		celdaTask, 							/* Callback para la tarea */
		(const signed char *) "Celda Peltier",	/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE, 				    /* Minimo stack para la tarea */
		NULL, 									/* Sin parametros */
		tskCELDA_PRIOTITY,					/* Prioridad */
		NULL									/* Sin handler */
	);

	/* Inicio el scheduler */
	vTaskStartScheduler();

    while(1);

    return 0 ;
}
