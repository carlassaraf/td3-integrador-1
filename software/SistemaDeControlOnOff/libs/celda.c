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
 *
 * 	@param	on: true para prendelo, false para apagarlo
 */
void gpio_heat_on(bool on) { Chip_GPIO_SetPinState(CELDA_PORT, HEAT_PIN, on); }

/*
 * 	@brief	Prende el sistema de refrigeracion
 *
 * 	@param	on: true para prendelo, false para apagarlo
 */
void gpio_cold_on(bool on) { Chip_GPIO_SetPinState(CELDA_PORT, COLD_PIN, on); }
