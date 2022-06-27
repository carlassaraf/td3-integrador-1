/*
 * 7segments.h
 *
 *  Created on: Jun 16, 2022
 *      Author: fabri
 */

#ifndef _7SEGMENTS_H_
#define _7SEGMENTS_H_

#include "chip.h"

/* Port de los segmentos y comunes */
#define SEG_PORT	LPC_GPIO2, 2
#define	DIGIT_PORT	LPC_GPIO, 0

/* Pines de los segmentos */
#define SEG_A	0
#define	SEG_B	1
#define SEG_C	2
#define SEG_D	3
#define	SEG_E	4
#define SEG_F	5
#define SEG_G	6
#define SEG_DP		7
/* Mascaras para los pines de los segmentos */
#define SEG_A_MASK	(1 << SEG_A)
#define SEG_B_MASK	(1 << SEG_B)
#define SEG_C_MASK	(1 << SEG_C)
#define SEG_D_MASK	(1 << SEG_D)
#define SEG_E_MASK	(1 << SEG_E)
#define SEG_F_MASK	(1 << SEG_F)
#define SEG_G_MASK	(1 << SEG_G)
#define SEG_DP_MASK	(1 << SEG_DP)
/* Pines de que controlan el comun de los segmentos */
#define DIGIT_1	10
#define DIGIT_2	5
#define DIGIT_3	4
/* Mascaras para los pines del comun */
#define DIGIT_1_MASK	(1 << DIGIT_1)
#define DIGIT_2_MASK	(1 << DIGIT_2)
#define DIGIT_3_MASK	(1 << DIGIT_3)
/* Mascara completa de los pines que controlan el comun */
#define DIGITS 		DIGIT_1_MASK | DIGIT_2_MASK | DIGIT_3_MASK
/* Mascara completa de los pines que van a los segmentos */
#define SEGMENTS	SEG_A_MASK | SEG_B_MASK | SEG_C_MASK | SEG_D_MASK | SEG_E_MASK | SEG_F_MASK | SEG_G_MASK | SEG_DP_MASK

/* Prototipos de funciones */
void gpio_7segments_init(void);
void gpio_7segments_clear(void);
void gpio_7segments_write(uint8_t number);
void gpio_7segments_set_digit(uint8_t number);
void gpio_7segments_set_dp(bool on);

#endif /* _7SEGMENTS_H_ */
