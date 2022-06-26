/*
 * proj_tasks.c
 *
 *  Created on: Jun 24, 2022
 *      Author: fabri
 */

#include "proj_tasks.h"

/* Cola para el ADC */
xQueueHandle queueADC, queueSD, queueSP;


void initTask(void *params) {
	/* Sets up DEBUG UART */
	DEBUGINIT();
    /* Inicializo SPI */
    SPI_Inicializar();
    /* Inicializo ADC */
    adc_init();
    /* Inicializo los 7 Segmentos */
    gpio_7segments_init();
    /* Inicializo botones */
    gpio_btn_init();
    /* Elimino tarea */
    vTaskDelete(NULL);
}

void btnTask(void *params) {
	/* Delay para captura de botones */
	const uint16_t DELAY_MS = 250;
	/* Setpoint inicial */
	float setpoint = 25.0;
	/* Variable para verificar que digito modificar */
	uint8_t digits = 0x04;
	/* Valor de incremento/decremento */
	int8_t inc = 0;

	while(1) {
		/* Solo valido si se elige mostrar el setpoint */
		if(SETPOINT_SELECTED) {
			/* Veo si se sube o baja el valor del setpoint */
			/* Si el boton up se apreto, se incrementa */
			if(gpio_get_btn_up()) { inc = 1; }
			/* Si el boton down se apreto, se decrementa */
			else if(gpio_get_btn_down()) { inc = -1; }
			/* Si el boton enter se apreto, no cambia el setpoint, pero cambio de digito */
			else if (gpio_get_btn_enter()) {
				/* No hay incremento */
				inc = 0;
				/* Voy al digito de la derecha*/
				digits >>= 1;
				/* Vuelvo al primer digito */
				if(digits == 0) { digits = 0x04; }
			}
			/* Reviso el digito */
			switch (digits) {
				/* Si es el primer digito, cambio la decena */
				case 0b100:
					setpoint += inc * 10;
					break;
				/* Si es el segundo digito, cambio la unidad */
				case 0b010:
					setpoint += inc;
					break;
				/* Si es el tercer digito, cambio el decimal */
				case 0b001:
					setpoint += inc / 10.0;
					break;
			}
			/* Mando el setpoint a la cola */
			xQueueSendToBack(queueSP, &setpoint, portMAX_DELAY);
		}
		/* Bloqueo la tarea por un rato */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

/* Tarea que inicia las lecturas del LM35 */
void lm35Task(void *params) {
	/* Delay entre conversiones */
	const uint16_t DELAY_MS = 1000;

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
	/* Variable para guardar la temperatura/setpoint */
	float temp;

	while(1) {
		/* Chequeo que tengo que mostrar */
		if(TEMPERATURE_SELECTED) {
			/* Bloqueo la tarea hasta que termine la interrupcion */
			xQueueReceive(queueADC, &adc, portMAX_DELAY);
			/* Calculo la temperatura */
			temp = conv_factor * adc * 100;
			xQueueSendToBack(queueSD, &temp, portMAX_DELAY);
		}
		else {
			/* Bloqueo la tarea hasta que llegue el dato de la cola */
			xQueueReceive(queueSP, &temp, portMAX_DELAY);
		}
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
	uint32_t i = 0;

    /* Inicializamos y verificamos que se inicialice correctamente el SD*/
    if (SD_Init (&carpeta.tipo) == SD_FALSE)
    {
    	// No se pudo iniciar la tarjeta. por favor, revisa la tarjeta
        while (1)
        	i++;
    }

    /* Verificamos que se pueda leer */
    if (SD_ReadConfiguration (&carpeta.CardConfig) == SD_FALSE)
    {
    	// No se pudo leer
		while (1)
			i++;
    }
    // Se inicializó con exito.

	/* Abro una carpeta que exista o creo una nueva */
    carpeta.fr = f_mount(&carpeta.fs, "0:", 0); // Registro un objeto del sistema de archivos para una unidad lógica "0:", es decir es el Driver.
    carpeta.fr = f_open(&carpeta.fil, "td3.c", FA_CREATE_ALWAYS); // Si no existe crea.
    if (carpeta.fr == FR_OK) {
    	//carpeta.fr = f_write (&carpeta.fil, carpeta.bufferWrite, sizeof(carpeta.bufferWrite), &carpeta.BytesWritten); // Escribe
    	// Buscar hasta el final del archivo para agregar datos
    	//carpeta.fr = f_lseek(&carpeta.fil, f_size(&carpeta.fil));

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

	float temp;
	char vector[40];
	/* vacio el vector*/
	for (uint8_t i = 0; i < 40; i++)
		vector[i] ='\0';
	while(1)
	{
		xQueueReceive(queueSD, &temp, portMAX_DELAY);
		carpeta.fr = f_mount(&carpeta.fs, "0:", 0); // Registro un objeto del sistema de archivos para una unidad lógica "0:", es decir es el Driver.
		carpeta.fr = f_open(&carpeta.fil, "td3.c", FA_READ | FA_WRITE); // Si no existe crea.
		if (carpeta.fr == FR_OK){
			imprimir(vector, &temp);
			carpeta.tamanioArchivo = f_size(&carpeta.fil);
			carpeta.bufferRead = malloc(carpeta.tamanioArchivo);
			while(carpeta.tamanioArchivo > 0)
			{
				carpeta.fr = f_read (&carpeta.fil, carpeta.bufferRead, (UINT)sizeof(carpeta.bufferWrite), &carpeta.ByteRead);
				carpeta.tamanioArchivo -= sizeof(carpeta.bufferWrite);
			}
			carpeta.fr = f_write (&carpeta.fil, vector, sizeof(carpeta.bufferWrite), &carpeta.BytesWritten); // Escribe
			// Buscar hasta el final del archivo para agregar datos
			carpeta.fr = f_lseek(&carpeta.fil, f_size(&carpeta.fil));
			// En caso de que no poder escribirse la carpeta se debe cerrarla.
			for (uint8_t i = 0; i < 40; i++)
				vector[i] ='\0';
			if (carpeta.fr != FR_OK)
				f_close(&carpeta.fil);
			// Se creó la carpeta TecnicasDigitalesIII, debemos cerrar la carpeta.
		}
		f_close(&carpeta.fil);
	}

}
void imprimir(char *cadena, float *valor){

	int aux2, aux3;
	char cad [2], cad1 [2];
	*valor = *valor * 100;    	    // nn,ddc -> nndd,c
	aux2 = (int) *valor ;     	    // nndd,c -> nndd
	aux3 = aux2 - (aux2/100) * 100; // nndd - nndd/100 -> dd
	aux2 = aux2 / 100;				// nn

	itoa (aux2, cad, 10 );
	itoa (aux3, cad1, 10 );
	strcpy(cadena, "\ntemperatura: ");
	strcat(cadena, cad);
	strcat(cadena, ".");
	strcat(cadena, cad1);
}

