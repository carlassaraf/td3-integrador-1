/**************************************************************************//**
 * @file     sd.h
 * @brief    Header file for sd.c
 * @version  1.0
 * @date     18. Nov. 2010
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP), ChaN. All rights reserved.
 *
 * General SD driver (SD_xxxx()): NXP
 * SD card initilization flow and some code snippets are inspired from ChaN.
 *
 ******************************************************************************/

#ifndef __SD_H
#define __SD_H
             /* LPC17xx Definitions */
#include <spi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ff.h"
#include "ffconf.h"
#include "diskio.h"

/* type defintion */
typedef unsigned char    SD_BOOL;
#define SD_TRUE     1
#define SD_FALSE    0

#ifndef NULL
 #ifdef __cplusplus              // EC++
  #define NULL          0
 #else
  #define NULL          ((void *) 0)
 #endif
#endif

/* Memory card type definitions */
#define CARDTYPE_UNKNOWN        0
#define CARDTYPE_MMC            1   /* MMC */
#define CARDTYPE_SDV1           2   /* V1.x Standard Capacity SD card */
#define CARDTYPE_SDV2_SC        3   /* V2.0 or later Standard Capacity SD card */
#define CARDTYPE_SDV2_HC        4   /* V2.0 or later High/eXtended Capacity SD card */

/* SD/MMC card configuration */
typedef struct tagCARDCONFIG
{
    uint32_t sectorsize;    /* size (in byte) of each sector, fixed to 512bytes */
    uint32_t sectorcnt;     /* total sector number */
    uint32_t blocksize;     /* erase block size in unit of sector */
	uint8_t  ocr[4];		/* OCR */
	uint8_t  cid[16];		/* CID */
	uint8_t  csd[16];		/* CSD */
	uint8_t  status[64];    /* Status */
} CARDCONFIG;

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

/* Public variables */
//extern uint8_t CardType;
//extern CARDCONFIG CardConfig;

/* Public functions */
SD_BOOL     SD_Init (uint8_t *Tipo);
SD_BOOL     SD_ReadSector (uint32_t sect, uint8_t *buf, uint32_t cnt);
SD_BOOL     SD_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt);
SD_BOOL     SD_ReadConfiguration (CARDCONFIG *CarConfig);
void        disk_timerproc (void);
uint8_t     SD_SendCommand (uint8_t cmd, uint32_t arg, uint8_t *buf, uint32_t len);
uint8_t     SD_SendACommand (uint8_t cmd, uint32_t arg, uint8_t *buf, uint32_t len);
SD_BOOL     SD_RecvDataBlock (uint8_t *buf, uint32_t len);
SD_BOOL     SD_SendDataBlock (const uint8_t *buf, uint8_t tkn, uint32_t len) ;
SD_BOOL     SD_WaitForReady (void);

void imprimir(char *cadena, float *valor);
#endif // __SD_H

/* --------------------------------- End Of File ------------------------------ */
