/*
 * proj_tasks.h
 *
 *  Created on: Jun 24, 2022
 *      Author: fabri
 */

#ifndef PROJ_TASKS_H_
#define PROJ_TASKS_H_

#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "adc.h"
#include "7segments.h"
#include "SPI.h"
#include "sd.h"
#include "buttons.h"

#include "ff.h"
#include "ffconf.h"
#include "diskio.h"

#define tskINIT_PRIORITY		tskIDLE_PRIORITY + 4UL
#define	tskBTN_PRIORITY			tskIDLE_PRIORITY + 3UL
#define tskLM35_PRIORITY		tskIDLE_PRIORITY + 2UL
#define tsk7SEG_PRIORITY		tskIDLE_PRIORITY + 3UL
#define tskSDWRITE_PRIOTITY		tskIDLE_PRIORITY + 2UL
#define tskCELDA_PRIOTITY		tskIDLE_PRIORITY + 1UL

#define configSD_TASK_SIZE		1000
extern xQueueHandle queueTEMP, queueSP;
/* Factor de conversion para el valor del ADC */
#define conv_factor  			3.3 / (1 << 12)

void initTask(void *params);
void btnTask(void *params);
void lm35Task(void *params);
void displayTask(void *params);
void sdWriteTask(void *params);
void celdaTask(void *params);
void imprimir(char *cadena, float *valor);

#endif /* PROJ_TASKS_H_ */
