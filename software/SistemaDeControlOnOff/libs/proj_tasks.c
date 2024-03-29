/*
 * proj_tasks.c
 *
 *  Created on: Jun 24, 2022
 *      Author: fabri
 */

#include "proj_tasks.h"

/* Cola para el ADC */
xQueueHandle queueTEMP, queueSP;

/*
 * 	@brief	Tarea de inicializacion de perifericos
 */
void initTask(void *params) {
    /* Inicializo SPI */
    SPI_Inicializar();
    /* Inicializo ADC */
    adc_init();
    /* Inicializo los 7 Segmentos */
    gpio_7segments_init();
    /* Inicializo botones */
    gpio_btn_init();
    /* Inicialización de celda */
    gpio_celda_init();
    /* Elimino tarea */
    vTaskDelete(NULL);
}

/*
 * 	@brief	Tarea que lee el estado de los botones y actualiza el setpoint
 */
void btnTask(void *params) {
	/* Delay para captura de botones */
	const uint16_t DELAY_MS = 5;
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
			if(!gpio_get_btn_up()) {
				while(!gpio_get_btn_up());
				inc = 1;
			}
			/* Si el boton down se apreto, se decrementa */
			else if(!gpio_get_btn_down()) {
				while(!gpio_get_btn_down());
				inc = -1;
			}
			/* Si el boton enter se apreto, no cambia el setpoint, pero cambio de digito */
			else if (!gpio_get_btn_enter()) {
				while(!gpio_get_btn_enter());
				/* No hay incremento */
				inc = 0;
				/* Voy al digito de la derecha*/
				digits >>= 1;
				/* Vuelvo al primer digito */
				if(digits == 0) { digits = 0x04; }
			}
			else{
				inc = 0;
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
			/* Si el setpoint es negativo, lo vuelvo a cero */
			if(setpoint < 0) { setpoint = 0; }
			/* Si el setpoint se excedio de 100, vuelvo a 99.9 */
			if(setpoint >= 100.0) { setpoint = 99.9; }
		}
		/* Mando el setpoint a la cola */
		xQueueSendToBack(queueSP, &setpoint, portMAX_DELAY);
		/* Bloqueo la tarea por un rato */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

/*
 * 	@brief	Tarea que inicia las lecturas del LM35
 */
void lm35Task(void *params) {
	/* Delay entre conversiones */
	const uint16_t DELAY_MS = 5;

	while(1) {
		/* Inicio la conversion */
		adc_start();
		/* Bloqueo por un tiempo hasta empezar otra conversion*/
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

/*
 * 	@brief	Muestreo del valor de temperatura/setpoint en el 7 segmentos
 */
void displayTask(void *params) {
	/* Delay entre cambio de digitos */
	const uint8_t DELAY_MS = 5;
	/* Variable para guardar la temperatura/setpoint */
	float temp;
	/* Variable para almacenar cada digito separado */
	uint8_t digit1, digit2, digit3;

	while(1) {
		/* Si se eligio mostrar la temperatura, bloqueo la tarea hasta que la interrupcion la cargue */
		if(TEMPERATURE_SELECTED) { xQueuePeek(queueTEMP, &temp, portMAX_DELAY); }
		/* Si se eligio mostrar el setpoint, bloqueo la tarea hasta que este en la cola */
		else { xQueuePeek(queueSP, &temp, portMAX_DELAY); }
		/* Obtengo el primer digito */
		digit1 = (uint8_t)(temp / 10);
		/* Escribo el numero en el primer digito, sin el punto */
		gpio_7segments_set(DIGIT_1, digit1, false);
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
		/* Obtengo el segundo digito */
		digit2 = (uint8_t)temp % 10;
		/* Escribo el numero en el segundo digito, con el punto */
		gpio_7segments_set(DIGIT_2, digit2, true);
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
		/* Obtengo los decimales */
		digit3 = (uint8_t)((temp - 10 * digit1 - digit2) * 10);
		/* Escribo el numero en el tercer digito, sin el punto */
		gpio_7segments_set(DIGIT_3, digit3, false);
		/* Bloqueo la tarea por unos momentos */
		vTaskDelay(DELAY_MS / portTICK_RATE_MS);
	}
}

/*
 * 	@brief 	Tarea que escribe el valor de temperatura en la SD
 */
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
    carpeta.fr = f_open(&carpeta.fil, "td3.csv", FA_CREATE_ALWAYS); // Si no existe crea.
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
		xQueuePeek(queueTEMP, &temp, portMAX_DELAY);
		carpeta.fr = f_mount(&carpeta.fs, "0:", 0); // Registro un objeto del sistema de archivos para una unidad lógica "0:", es decir es el Driver.
		carpeta.fr = f_open(&carpeta.fil, "td3.csv", FA_READ | FA_WRITE); // Si no existe crea.
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
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

}

/*
 * 	@brief	Tarea que se encarga de manejar la celda peltier
 */
void celdaTask(void *params){
	/* Variables para guardar los valores de las colas */
	float temp, sp;
	/* Versiones enteras para la temperatura y setpoint */
	uint8_t iTemp, iSp;

	while(1) {
		/* Se bbloquea hasta recibir la temperatura */
		xQueueReceive(queueTEMP, &temp, portMAX_DELAY);
		/* Se bloquea hasta recibir el setpoint */
		xQueueReceive(queueSP, &sp, portMAX_DELAY);
		/* Obtengo la temperatura y setpoint como enteros */
		iTemp = temp;
		iSp = sp;
		/* Enciendo el fan */
		gpio_fan_on(true);
		/* Si la temperatura es menor al setpoint, calefacciono */
		if(iTemp < iSp) {
			/* Enciendo la calefaccion */
			gpio_heat_on();
		}
		/* Si la temperatura es mayor al setpoint, refrigero */
		else if(iTemp > iSp) {
			/* Enciendo la refrigeracion */
			gpio_cold_on();
		}
		/* Si son iguales descontando los decimales, apago la celda */
		else {
			/* Apago la celda */
			gpio_celda_off();
		}
	}
}

/*
 * 	@brief	Handler para las interrupciones del ADC
 */
void ADC_IRQHandler(void) {
	/* Control para cambiar de tarea al salir de la interrupcion */
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	/* Variable auxiliar para el valor  del ADC */
	uint16_t adc;
	/* Variable para la temperatura */
	float temp;
	/* Obtengo el valor del ADC */
	adc_read(adc);
	/* Convierto el valor del ADC en temperatura */
	temp = CONV_FACTOR * (float)adc * 50;
	/* Ingreso el valor a la cola */
	xQueueSendToBackFromISR(queueTEMP, &temp, &xHigherPriorityTaskWoken);
	/* Solicitar un cambio de contexto si fuese necesario */
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
