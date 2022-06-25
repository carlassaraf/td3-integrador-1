/*
 * proj_tasks.h
 *
 *  Created on: Jun 24, 2022
 *      Author: fabri
 */

#ifndef PROJ_TASKS_H_
#define PROJ_TASKS_H_

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "adc.h"
#include "7segments.h"
#include "SPI.h"
#include "sd.h"
#include "buttons.h"

#define tskINIT_PRIORITY		tskIDLE_PRIORITY + 3UL
#define	tskBTN_PRIORITY			tskIDLE_PRIORITY + 2UL
#define tskLM35_PRIORITY		tskIDLE_PRIORITY + 1UL
#define tsk7SEG_PRIORITY		tskIDLE_PRIORITY + 2UL
#define tskSDWRITE_PRIOTITY		tskIDLE_PRIORITY + 1UL

extern xQueueHandle queueADC;

typedef struct{
	CARDCONFIG CardConfig;
	SD_BOOL tipo; 	// tipo se usa para ver el la version de la SD pero en este caso no es necesario, si para poder utilziar la funcion INIT
    FRESULT fr; 	// variable para leer los retornos de las funciones FATS
    FATFS fs; 		// Variable tipo FATS
    FIL fil; 		// Archivos
    uint32_t tamanioArchivo;
    UINT BytesWritten;
    UINT ByteRead;
    char bufferWrite[20]; // Buffer
    char *bufferRead;
} sd_variables_t;

void initTask(void *params);
void btnTask(void *params);
void lm35Task(void *params);
void displayTask(void *params);
void sdWriteTask(void *params);

#endif /* PROJ_TASKS_H_ */
