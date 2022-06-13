/*
 * SPI.h
 *
 *  Created on: 12 ene. 2022
 *      Author: Gustavo
 */

#ifndef SPI_H_
#define SPI_H_

#include "chip.h"


typedef enum pinesSPI{
	F_CS  	  = 0,
	TFT_RESET = 1,
	SSEL1 	  = 6,
	SSEL0 	  = 16,
	LED 	  = 17,
	DC_RS 	  = 18,
}PinSPI;

typedef enum estadoSPI{
	Bajo  	  = 0,
	Alto	  = 1,
}estadoSPI;

typedef enum DCTFT{
	Comando   = 0,
	Dato	  = 1,
}datComTFT;

/* SPI clock rate setting.
SSP0_CLK = SystemCoreClock / divider,
The divider must be a even value between 2 and 254!
In SPI mode, max clock speed is 20MHz for MMC and 25MHz for SD */
#define SPI_CLOCKRATE_LOW   (uint32_t) (8)   	/* 100MHz / 10 = 10MHz */
#define SPI_CLOCKRATE_HIGH  (uint32_t) (2)     	/* 100MHz / 3 = 33.3MHz */

/* Macro defines for SSP SR register */
#define SSP_SR_TFE      ((uint32_t)(1<<0)) /** SSP status TX FIFO Empty bit */
#define SSP_SR_TNF      ((uint32_t)(1<<1)) /** SSP status TX FIFO not full bit */
#define SSP_SR_RNE      ((uint32_t)(1<<2)) /** SSP status RX FIFO not empty bit */
#define SSP_SR_RFF      ((uint32_t)(1<<3)) /** SSP status RX FIFO full bit */
#define SSP_SR_BSY      ((uint32_t)(1<<4)) /** SSP status SSP Busy bit */
//#define SSP_SR_BITMASK	((uint32_t)(0x1F)) /** SSP SR bit mask */

// Funciones SPI General
void SPI_ConfigClockRate (uint32_t spi_ClockRate);
void SPI_Inicializar(void);
uint8_t SPI_EscribirByte (uint8_t data);
uint8_t SPI_RecibirByte (void);
void SPI_Pin(uint8_t nroPin, uint8_t estado);

// Funciones TFT
uint8_t SPI_EscribirComando(uint8_t comando);

// Funciones SD
uint8_t SD_EscribirByte (uint8_t data);
uint8_t SD_RecibirByte (void);
uint8_t SD_RecibirByteVar (uint8_t dato);

#endif /* SPI_H_ */
