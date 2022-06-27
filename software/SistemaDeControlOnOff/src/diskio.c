/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sd.h"
#include "SPI.h"
#include "stdio.h"
#include "chip.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/** Select the card */
#define SD_Select()  do {SPI_Pin(SSEL0, Bajo);} while (0)
/** DeSelect the card */
#define SD_DeSelect() do {SPI_Pin(SSEL0, Alto); SD_RecibirByte();} while (0)

DSTATUS stat;
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber to identify the drive */
)
{
	//DSTATUS stat;
	uint8_t tipo;
	CARDCONFIG *CConfig;
	if (drv) return STA_NOINIT;			/* Supports only single drive */
//	if (stat & STA_NODISK) return stat;	/* No card in the socket */

	//if (SD_Init(&tipo) && SD_ReadConfiguration(CConfig))
		//stat &= ~STA_NOINIT;

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (stat & STA_NOINIT) return RES_NOTRDY;

	if (SD_ReadSector (sector, buff, count) == SD_TRUE)
		return RES_OK;
	else
		return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (stat & STA_NOINIT) return RES_NOTRDY;
//	if (stat & STA_PROTECT) return RES_WRPRT;

	if ( SD_WriteSector(sector, buff, count) == SD_TRUE)
		return RES_OK;
	else
		return 	RES_ERROR;

}
#endif /* _READONLY == 0 */


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, *ptr = buff;
	uint8_t CardType;          /* card type */
	CARDCONFIG CardConfig;     /* Card configuration */

	if (drv) return RES_PARERR;
	if (stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl) {
	case CTRL_SYNC :		/* Make sure that no pending write process */
        SD_Select();
		if (SD_WaitForReady() == SD_TRUE)
			res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
		*(DWORD*)buff = CardConfig.sectorcnt;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
		*(WORD*)buff = CardConfig.sectorsize;	//512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		*(DWORD*)buff = CardConfig.blocksize;
		res = RES_OK;
		break;

	case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
		for (n=0;n<16;n++)
			*(ptr+n) = CardConfig.csd[n];
		res = RES_OK;
		break;

	case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
		for (n=0;n<16;n++)
			*(ptr+n) = CardConfig.cid[n];
		res = RES_OK;
		break;

	case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
		for (n=0;n<4;n++)
			*(ptr+n) = CardConfig.ocr[n];
		res = RES_OK;
		break;

	case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
		for (n=0;n<64;n++)
            *(ptr+n) = CardConfig.status[n];
        res = RES_OK;
		break;

	default:
		res = RES_PARERR;
	}

    SD_DeSelect();

	return res;
}

