/*
 * 7segments.c
 *
 *  Created on: Jun 16, 2022
 *      Author: fabri
 */

#include "7segments.h"

/* Para anodo ocmun */
uint8_t numbers[] = {
	~0x3f,  	/* 0 */
	~0x06,  	/* 1 */
	~0x5b,  	/* 2 */
	~0x4f,  	/* 3 */
	~0x66,  	/* 4 */
	~0x6d,  	/* 5 */
	~0x7d,  	/* 6 */
	~0x07,  	/* 7 */
	~0x7f,  	/* 8 */
	~0x67,  	/* 9 */
	~0x00		/* Clear */
};

/*
 * 	@brief	Inicializa los pines conectados al 7 segmentos
 */
void gpio_7segments_init(void) {
	/* Inicializo los pines como salidas */
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);
	Chip_GPIO_SetPortDIROutput(DIGIT_PORT, DIGITS);
	Chip_GPIO_SetPortDIROutput(SEG_PORT, SEGMENTS);
}

/*
 * 	@brief	Limpia el 7 segmentos
 */
void gpio_7segments_clear(void) {
	/* Apago todos los segmentos */
	gpio_7segments_write(10);
}

/*
 * 	@brief	Escribe el numero en el segmento
 *
 * 	@param	number: numero del 0 al 9
 */
void gpio_7segments_write(uint8_t number) {
	/* Obtengo el valor que hay que escribir */
	uint8_t val = numbers[number];
	/* Recorro los siete pines */
	for(uint8_t pin = 0; pin < 7; pin++) {
		/* Si el bit de esa posicion esta en uno, true, sino, false */
		bool state = (val & (1 << pin))? true : false;
		/* Escribo el valor del pin */
		Chip_GPIO_SetPinState(SEG_PORT, pin, state);
	}
}

/*
 * 	@brief	Prende el digito elegido
 *
 * 	@param	digit: DIGITO_1, DIGITO_2 o DIGITO_3
 */
void gpio_7segments_set_digit(uint8_t digit) {
	/* Apago los tres 7 segmentos */
	Chip_GPIO_SetPinState(DIGIT_PORT, DIGIT_1, true);
	Chip_GPIO_SetPinState(DIGIT_PORT, DIGIT_2, true);
	Chip_GPIO_SetPinState(DIGIT_PORT, DIGIT_3, true);
	/* Prendo el digito solicitado */
	Chip_GPIO_SetPinState(DIGIT_PORT, digit, false);
}

/*
 * 	@brief	Prende/Apaga el punto del 7 segmentos
 *
 * 	@param	on: true para prenderlo, false para apagarlo
 */
void gpio_7segments_set_dp(bool on) { Chip_GPIO_SetPinState(SEG_PORT, SEG_DP, !on); }

/*
 * 	@brief	Prende el numero en el digito solicitado
 *
 * 	@param	digit: DIGIT_1, DIGIT_2 o DIGIT_3
 * 	@param	number: un numero del 0 al 9
 * 	@param	dp: true para prender el DP, false para apagarlo
 */
void gpio_7segments_set(uint8_t digit, uint8_t number, bool dp) {
	/* Prendo el digito */
	gpio_7segments_set_digit(digit);
	/* Escribo el numero */
	gpio_7segments_write(number);
	/* Prendo/apago el DP */
	gpio_7segments_set_dp(dp);
}
