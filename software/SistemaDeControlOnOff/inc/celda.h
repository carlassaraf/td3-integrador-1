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
#define	GND_CP_PIN		8
#define VCC_CP_PIN		9
/* Mascaras para los pines de la celda */
#define FAN_PIN_MASK	(1 << FAN_PIN)
#define GND_CP_PIN_MASK	(1 << GND_CP_PIN)
#define VCC_CP_PIN_MASK	(1 << VCC_CP_PIN)
/* Mascara con todos los pines de la celda */
#define CELDA_PINS_MASK	FAN_PIN_MASK | GND_CP_PIN_MASK | VCC_CP_PIN_MASK

/* Protipos de funciones */
void gpio_celda_init(void);
void gpio_fan_on(bool on);
void gpio_heat_on(void);
void gpio_cold_on(void);
void gpio_celda_off(void);

#endif /* CELDA_H_ */
