/*
 * proj_tasks.c
 *
 *  Created on: Jun 24, 2022
 *      Author: fabri
 */

#include "proj_tasks.h"

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

void sdWriteTask(void *params){

	sd_variables_t carpeta;

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

void sdReadTask(void *params){

	sd_variables_t carpeta;

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
