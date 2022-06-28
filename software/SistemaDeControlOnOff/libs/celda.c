/*
 * celda.c
 *
 *  Created on: Jun 27, 2022
 *      Author: fabri
 */

#include "celda.h"

void gpio_celda_init(void) {
	/* Inicializo los pines como salida */
	Chip_GPIO_SetPortDIROutput(CELDA_PORT, CELDA_PINS_MASK);
	Chip_IOCON_Init(LPC_IOCON);
}

/*
 * 	@brief	Prende/apaga el fan de la celda
 *
 * 	@param	on: true para prenderlo, false para apagarlo
 */
void gpio_fan_on(bool on) { Chip_GPIO_SetPinState(CELDA_PORT, FAN_PIN, on); }

/*
 * 	@brief	Prende el sistema de calefaccion
 */
void gpio_heat_on(void) {
	gpio_fan_on(true);
	Chip_GPIO_SetPinState(CELDA_PORT, VCC_CP_PIN, false);
	Chip_GPIO_SetPinState(CELDA_PORT, GND_CP_PIN, false);
	gpio_fan_on(false);
}

/*
 * 	@brief	Prende el sistema de refrigeracion
 */
void gpio_cold_on(void) {
	gpio_fan_on(true);
	Chip_GPIO_SetPinState(CELDA_PORT, VCC_CP_PIN, true);
	Chip_GPIO_SetPinState(CELDA_PORT, GND_CP_PIN, true);
	gpio_fan_on(false);
}

/*
 * 	@brief	Apaga la celda
 */
void gpio_celda_off(void) { gpio_fan_on(true); }
