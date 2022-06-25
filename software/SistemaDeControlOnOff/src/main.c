/*
===============================================================================
 Name        : SistemaDeControlOnOff.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <stdlib.h>
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "adc.h"
#include "7segments.h"
#include "SPI.h"
#include "sd.h"

#define tskLM35_PRIORITY	tskIDLE_PRIORITY + 1UL
#define tsk7SEG_PRIORITY	tskIDLE_PRIORITY + 2UL

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
}SD_Variables;

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

uint8_t contador = 0;

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
		//xQueueReceive(queueADC, &adc, portMAX_DELAY);
		/* Calculo la temperatura */
		//temp = conv_factor * adc * 100;
		/* Obtengo el primer digito */
		//uint8_t digit1 = (uint8_t)(temp / 10);
		/* Prendo el primer digito */
		//gpio_7segments_set_digit(DIGIT_1);
		/* Muestro el numero */
		gpio_7segments_write(contador);
		contador++;
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
		/* Obtengo el segundo digito */
		//uint8_t digit2 = (uint8_t)temp % 10;
		/* Prendo el segundo digito */
		//gpio_7segments_set_digit(DIGIT_2);
		/* Muestro el numero */
		//gpio_7segments_write(digit2);
		/* Bloqueo la tarea por unos momentos */
		//vTaskDelay(DELAY_MS / portTICK_RATE_MS);
		/* Obtengo los decimales */
		//uint8_t digit3 = (uint8_t)((temp - 10 * digit1 - digit2) * 10);
		/* Prendo el tercer digito */
		//gpio_7segments_set_digit(DIGIT_3);
		/* Muestro el numero */
		//gpio_7segments_write(digit3);
		/* Bloqueo la tarea por unos momentos */
		//vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}
static void SD_CreaCarpeta(void *pvParameters){

    SD_Variables carpeta;

    /* Inicializamos y verificamos que se inicialice correctamente el SD*/
    if (SD_Init (&carpeta.tipo) == SD_FALSE)
    {
    	// No se pudo iniciar la tarjeta. por favor, revisa la tarjeta
        while (1);
    }

    /* Verificamos que se pueda leer */
    if (SD_ReadConfiguration (&carpeta.CardConfig) == SD_FALSE)
    {
    	// No se pudo leer
		while (1);
    }
    // Se inicializó con exito.

	/* Abro una carpeta que exista o creo una nueva */
    carpeta.fr = f_mount(&carpeta.fs, "0:", 0); // Registro un objeto del sistema de archivos para una unidad lógica "0:", es decir es el Driver.
    carpeta.fr = f_open(&carpeta.fil, "TD3.txt", FA_WRITE | FA_CREATE_ALWAYS); // Si no existe crea.
    if (carpeta.fr == FR_OK) {
    	carpeta.fr = f_write (&carpeta.fil, carpeta.bufferWrite, sizeof(carpeta.bufferWrite), &carpeta.BytesWritten); // Escribe
    	// Buscar hasta el final del archivo para agregar datos
    	carpeta.fr = f_lseek(&carpeta.fil, f_size(&carpeta.fil));

        // En caso de que no poder escribirse la carpeta se debe cerrarla.
        if (carpeta.fr != FR_OK)
            f_close(&carpeta.fil);
    }

    // Verifico que se haya creado correctamente.
	if (carpeta.fr != FR_OK)
	{
		// Si no crea nada, viene acá.
		while (1);
	}
	// Se creó la carpeta TecnicasDigitalesIII, debemos cerrar la carpeta.
	f_close(&carpeta.fil);

	while(1)
	{

	}
}

static void SD_LeeCarpeta(void *pvParameters){

    SD_Variables carpeta;

	while(1){

    	/* Abro una carpeta e imprimo los datos */
		carpeta.fr = f_mount(&carpeta.fs, "0:", 0); // Driver para la carpeta

    	// Busco la carpeta, en caso de no existir fr sera distinto de FR_OK
    	carpeta.fr = f_open(&carpeta.fil, "TD3.txt", FA_READ);
    	if (carpeta.fr == FR_OK)
    	{
    		carpeta.tamanioArchivo = f_size(&carpeta.fil);
    		carpeta.bufferRead = malloc(carpeta.tamanioArchivo);
    		while(carpeta.tamanioArchivo > 0)
    		{
    			// Leemos el contenido de la carpeta, en FR tiene que devolver FR_OK si se leyó correctamente
    			carpeta.fr = f_read (&carpeta.fil, carpeta.bufferRead, (UINT)sizeof(carpeta.bufferWrite), &carpeta.ByteRead);
    			carpeta.tamanioArchivo -= sizeof(carpeta.bufferWrite); //
    		}
    		// Buscar hasta el final del archivo para agregar datos
    		carpeta.fr = f_lseek(&carpeta.fil, f_size(&carpeta.fil));

    		// Si no se pudo leer nada cierro la carpeta
    		if (carpeta.fr != FR_OK)
    			f_close(&carpeta.fil);
    	}

    	// Una vez leido se cierra la carpeta
    	f_close(&carpeta.fil);
	}

}

int main(void) {

    SystemCoreClockUpdate();
    Board_Init();
    /* Inicializo ADC */
//    adc_init();
    /* Inicializo los 7 Segmentos */
    gpio_7segments_init();

    /* Inicializo SPI */
//    SPI_Inicializar();


    /* Creo la cola para los datos del ADC */
    queueADC = xQueueCreate(1, sizeof(uint16_t));

//	xTaskCreate(
//		lm35Task,								/* Callback para la tarea */
//		(const signed char *) "LM35 Task",		/* Nombre de la tarea para debugging */
//		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
//		NULL,									/* Sin parametros */
//		tskLM35_PRIORITY,						/* Prioridad */
//		NULL									/* Sin handler */
//	);

	xTaskCreate(
		displayTask,							/* Callback para la tarea */
		(const signed char *) "7 Segment Task",	/* Nombre de la tarea para debugging */
		configMINIMAL_STACK_SIZE,				/* Minimo stack para la tarea */
		NULL,									/* Sin parametros */
		tsk7SEG_PRIORITY,						/* Prioridad */
		NULL									/* Sin handler */
	);

    /* Creacion de tareas */
//	xTaskCreate(SD_CreaCarpeta, (char *) "Temporizacion",
//    			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
//    			(xTaskHandle *) NULL);
//    xTaskCreate(SD_LeeCarpeta, (char *) "Lectura",
//        		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
//        		(xTaskHandle *) NULL);

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
	xQueueSendToBackFromISR(queueADC, &temp, 0);
	/* Solicitar un cambio de contexto si fuese necesario */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

