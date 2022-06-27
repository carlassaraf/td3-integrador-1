/*
 * celda.h
 *
 *  Created on: Jun 27, 2022
 *      Author: fabri
 */

#ifndef CELDA_H_
#define CELDA_H_

#include "chip.h"

/* Puerto usado para controlar la celda */
#define CELDA_PORT	LPC_GPIO, 0
/* Pines de la celda */
#define FAN_PIN			7
#define	HEAT_PIN		8
#define COLD_PIN		9
/* Mascaras para los pines de la celda */
#define FAN_PIN_MASK	(1 << FAN_PIN)
#define HEAT_PIN_MASK	(1 << HEAT_PIN)
#define COLD_PIN_MASK	(1 << COLD_PIN)
/* Mascara con todos los pines de la celda */
#define CELDA_PINS_MASK	FAN_PIN_MASK | HEAT_PIN_MASK | COLD_PIN_MASK

/* Protipos de funciones */
void gpio_celda_init(void);
void gpio_fan_on(bool on);
void gpio_heat_on(bool on);
void gpio_cold_on(bool on);

#endif /* CELDA_H_ */
