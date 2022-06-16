/*
 * 7segments.h
 *
 *  Created on: Jun 16, 2022
 *      Author: fabri
 */

#ifndef _7SEGMENTS_H_
#define _7SEGMENTS_H_

#define SEG_A	(1 << 0)
#define SEG_B	(1 << 1)
#define SEG_C	(1 << 2)
#define SEG_D	(1 << 3)
#define SEG_E	(1 << 4)
#define SEG_F	(1 << 5)
#define SEG_G	(1 << 6)

#define DIGIT_1	(1 << 10)
#define DIGIT_2	(1 << 11)
#define DIGIT_3	(1 << 12)

#define SEGMENTS	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G

void gpio_7segments_init(void);
void gpio_7segments_clear(void);
void gpio_7segments_write(uint8_t number);
void gpio_7segments_set_digit(uint8_t number);

#endif /* _7SEGMENTS_H_ */
