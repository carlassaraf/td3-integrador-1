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

#include "adc.h"
#include "7segments.h"

#define tskLM35_PRIORITY	tskIDLE_PRIORITY + 1UL
#define tsk7SEG_PRIORITY	tskIDLE_PRIORITY + 2UL

/* Cola para el ADC */
xQueueHandle queueADC;

/* Tarea que inicia las lecturas del LM35 */
void lm35Task(void *params) {
	/* Delay entre conversiones */
	const uint16_t DELAY_MS = 50;

	while(1) {
		/* Inicio la conversion */
		adc_start();
		/* Bloqueo por un tiempo hasta empezar otra conversion*/
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

/* Muestreo del valor de temperatura en el 7 segmentos */
void displayTask(void *params) {
	/* Delay entre cambio de digitos */
	const uint8_t DELAY_MS = 5;
	/* Factor de conversion para el valor del ADC */
	const float conv_factor = 3.3 / (1 << 12);
	/* Variable para guardar el valor del ADC */
	uint16_t adc;
	/* Variable para guardar la temperatura */
	float temp;

	while(1) {
		/* Bloqueo la tarea hasta que termine la interrupcion */
		xQueueReceive(queueADC, &adc, portMAX_DELAY);
		/* Calculo la temperatura */
		temp = conv_factor * adc * 100;
		/* Obtengo el primer digito */
		uint8_t digit1 = (uint8_t)(temp / 10);
		/* Prendo el primer digito */
		gpio_7segments_set_digit(DIGIT_1);
		/* Muestro el numero */
		gpio_7segments_write(digit1);
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
		/* Obtengo el segundo digito */
		uint8_t digit2 = (uint8_t)temp % 10;
		/* Prendo el segundo digito */
		gpio_7segments_set_digit(DIGIT_2);
		/* Muestro el numero */
		gpio_7segments_write(digit2);
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
		/* Obtengo los decimales */
		uint8_t digit3 = (uint8_t)((temp - 10 * digit1 - digit2) * 10);
		/* Prendo el tercer digito */
		gpio_7segments_set_digit(DIGIT_3);
		/* Muestro el numero */
		gpio_7segments_write(digit3);
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

int main(void) {

    SystemCoreClockUpdate();
    Board_Init();
    /* Inicializo ADC */
    adc_init();
    /* Inicializo los 7 Segmentos */
    gpio_7segments_init();

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
	xQueueSendToBackFromISR(queueADC, &temp, 0);
	/* Solicitar un cambio de contexto si fuese necesario */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

