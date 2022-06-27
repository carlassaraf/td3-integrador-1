/*
 * 7segments.c
 *
 *  Created on: Jun 16, 2022
 *      Author: fabri
 */

#include "7segments.h"

/* Para catodo ocmun */
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
	~0x00	/* Clear */
};

void gpio_7segments_init(void) {
	/* Inicializo los pines como salidas */
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 0, DIGITS);
	Chip_GPIO_SetPortDIROutput(LPC_GPIO2, 2, SEGMENTS);
	Chip_IOCON_Init(LPC_IOCON);
}

void gpio_7segments_clear(void) {
	/* Apago todos los segmentos */
	gpio_7segments_write(10);
}

void gpio_7segments_write(uint8_t number) {
	/* Obtengo el valor que hay que escribir */
	uint8_t val = numbers[number];
	/* Recorro los siete pines */
	for(uint8_t pin = 0; pin < 7; pin++) {
		/* Si el bit de esa posicion esta en uno, true, sino, false */
		bool state = (val & (1 << pin))? true : false;
		/* Escribo el valor del pin */
		Chip_GPIO_SetPinState(LPC_GPIO2, 2, pin, state);
	}
}

void gpio_7segments_set_digit(uint8_t number) {
	/* Apago los tres 7 segmentos */
	Chip_GPIO_SetPinState(LPC_GPIO, 0, DIGIT_1, true);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, DIGIT_2, true);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, DIGIT_3, true);
	/* Prendo el digito solicitado */
	Chip_GPIO_SetPinState(LPC_GPIO, 0, number, false);
}

void gpio_7segments_set_dp(bool on) {

	Chip_GPIO_SetPinState(LPC_GPIO2, 2, DP, on);
}
