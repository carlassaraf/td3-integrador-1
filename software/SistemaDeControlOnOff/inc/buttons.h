/*
 * buttons.h
 *
 *  Created on: Jun 25, 2022
 *      Author: fabri
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define	BTN_UP		21
#define BTN_DOWN	22
#define BTN_ENTER	27
#define	BTN_CHANGE	28
#define	BUTTONS	(1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_ENTER) | (1 << BTN_CHANGE)

#define gpio_btn_init()			Chip_GPIO_SetPortDIRInput(LPC_GPIO, 0, BUTTONS)
#define gpio_get_btn(btn)		Chip_GPIO_GetPinState(LPC_GPIO, 0, btn)
#define gpio_get_btn_up()		gpio_get_btn(BTN_UP)
#define gpio_get_btn_down()		gpio_get_btn(BTN_DOWN)
#define gpio_get_btn_enter()	gpio_get_btn(BTN_ENTER)
#define gpio_get_btn_change()	gpio_get_btn(BTN_CHANGE)

#define TEMPERATURE		0
#define SETPOINT		1

#define TEMPERATURE_SELECTED	gpio_get_btn_change() == TEMPERATURE
#define SETPOINT_SELECTED		gpio_get_btn_change() == SETPOINT

#endif /* BUTTONS_H_ */
