/*
 * SPI.c
 *
 *  Created on: 12 ene. 2022
 *      Author: Gustavo
 */
#include "SPI.h"
#include "sd.h"

void SPI_Pin(uint8_t nroPin, uint8_t estado)
{
	if(estado) // Pone 3.3V a la salida
		LPC_GPIO->PIN |= (1 << nroPin);
	else       // Pone 0V a la salida
		LPC_GPIO->PIN &= ~(1 << nroPin);
}

void SPI_ConfigClockRate (uint32_t spi_ClockRate)
{
    /* CPSR must be an even value between 2 and 254 */
    LPC_SSP1->CPSR = (spi_ClockRate & 0xFE);
}

uint8_t SPI_EscribirComando(uint8_t comando)
{
	// Activar esclavo
	SPI_Pin(SSEL1, Bajo);
	SPI_Pin(SSEL0, Alto);
	SPI_Pin(DC_RS, Comando);

    /* Put the data on the FIFO */
    LPC_SSP1->DR = comando;

    /* Wait for sending to complete */
    while (LPC_SSP1->SR & SSP_SR_BSY);

    //SPI_Pin(DC_RS, Dato);
    SPI_Pin(SSEL1, Alto);
    /* Return the received value */
    return (LPC_SSP1->DR);
}

uint8_t SPI_EscribirByte (uint8_t data)
{
	SPI_Pin(SSEL1, Bajo);
	SPI_Pin(SSEL0, Alto);
	SPI_Pin(DC_RS, Dato);
    /* Put the data on the FIFO */
    LPC_SSP1->DR = data;

    /* Wait for sending to complete */
    while (LPC_SSP1->SR & SSP_SR_BSY);

    SPI_Pin(SSEL1, Alto);
    /* Return the received value */
    return (LPC_SSP1->DR);
}

uint8_t SD_EscribirByte (uint8_t data)
{
	SPI_Pin(SSEL0, Bajo);
	SPI_Pin(F_CS, Bajo);
	SPI_Pin(SSEL1, Alto);
	SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);
	//SPI_Pin(DC_RS, Dato);
    /* Put the data on the FIFO */
    LPC_SSP1->DR = data;

    /* Wait for sending to complete */
    while (LPC_SSP1->SR & SSP_SR_BSY);

    SPI_Pin(SSEL0, Alto);
    SPI_ConfigClockRate (SPI_CLOCKRATE_HIGH);
    /* Return the received value */
    return (LPC_SSP1->DR);
}
uint8_t SPI_RecibirByte (void)
{
    /* Send 0xFF to provide clock for MISO to receive one byte */
    return SPI_EscribirByte (0xFF);
}

uint8_t SD_RecibirByte (void)
{
    /* Send 0xFF to provide clock for MISO to receive one byte */
    return SD_EscribirByte (0xFF);
}
uint8_t SD_RecibirByteVar (uint8_t dato)
{
    /* Send 0xFF to provide clock for MISO to receive one byte */
    return SD_EscribirByte (dato);
}

void SPI_Inicializar(void)
{
    LPC_SYSCTL->PCONP |= (0x1 << 10);

    LPC_SYSCTL->PCLKSEL[0] |= (0x01 << 20);  // SSP1_PCLK = CCLK/1 (100 MHz)
    LPC_IOCON->PINSEL[0] &= ~(0x3 << 00);	// F_CS  - P0.0
    LPC_IOCON->PINSEL[0] &= ~(0x3 << 02);	// RESET -	P0.1
    LPC_IOCON->PINSEL[0] &= ~(0x3 << 12);	// SSEL1 -	P0.6
    LPC_IOCON->PINSEL[1] &= ~(0x3 << 00);	// SSEL0 -	P0.16
    LPC_IOCON->PINSEL[1] &= ~(0x3 << 02);	// LED   -	P0.17
    LPC_IOCON->PINSEL[1] &= ~(0x3 << 05);	// DC/RS -	P0.18

    LPC_GPIO->DIR |= (1 << 0);			 	// SET P0.0  COMO SALIDA (F_CS)
    LPC_GPIO->DIR |= (1 << 1);			 	// SET P0.1  COMO SALIDA (RESET)
    LPC_GPIO->DIR |= (1 << 6);			 	// SET P0.6  COMO SALIDA (SSEL1)
    LPC_GPIO->DIR |= (1 << 16);			 	// SET P0.17 COMO SALIDA (SSEL0)
    LPC_GPIO->DIR |= (1 << 17);			 	// SET P0.17 COMO SALIDA (LED)
    LPC_GPIO->DIR |= (1 << 18);			 	// SET P0.18 COMO SALIDA (DATO/COMANDO)

    LPC_IOCON->PINSEL[0] |= (0x2 << 14)  	// SCK1  -	P0.7
						 |  (0x2 << 16)  	// MISO1 -	P0.8
						 |  (0x2 << 18); 	// MOSI1 -	P0.9

    LPC_SSP1->CR0 = (0x7) 					// Datos: 8bits
    			  | (0x0 << 4)				// formato de trama: SPI
				  | (0x0 << 6)				// CPOL: 0
				  | (0x0 << 7)				// CPHA: 0
				  | (0x0 << 8);				// SCR = 0

    LPC_SSP1->CR1 = (0x00 << 0)  // Modo normal
				  |	(0x01 << 1)  // Habilitar SSP1
				  | (0x00 << 2)  // Maestro
				  | (0x00 << 3); // Salida esclavo desabilitada

    /* Configure SSP0 clock rate to 10MHz (100MHz/10) */
    SPI_ConfigClockRate (SPI_CLOCKRATE_HIGH);

    /* Set SSEL to high */
    SPI_Pin(SSEL1, Alto);
    SPI_Pin(SSEL0, Alto);
}


