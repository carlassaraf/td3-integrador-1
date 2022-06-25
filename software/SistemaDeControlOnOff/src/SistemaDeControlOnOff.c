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
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "SPI.h"
#include "sd.h"

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
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


/* Handler de la cola */
xQueueHandle queueADC;

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

    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
	/* Sets up DEBUG UART */
	DEBUGINIT();

    /* Inicializo SPI */
    SPI_Inicializar();

	/* Creacion de la cola */
	queueADC = xQueueCreate(1,sizeof(uint16_t));

    /* Creacion de tareas */
	xTaskCreate(SD_CreaCarpeta, (char *) "Temporizacion",
    			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
    			(xTaskHandle *) NULL);
    xTaskCreate(SD_LeeCarpeta, (char *) "Lectura",
        		configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
        		(xTaskHandle *) NULL);

    /* Arranca el scheduler */
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    while(1) {

    }
    return 0 ;
}
