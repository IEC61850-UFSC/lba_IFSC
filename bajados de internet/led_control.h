/*
 * led_control.h
 *
 *  Created on: Aug 26, 2010
 *      Author: Jan Axelson
 *
 *      www.Lvr.com/beagleboard.htm
 */

#include "led_control.c"

#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

void led_control(int led_number, int led_state);
void led_initialize();

#endif /* LED_CONTROL_H_ */
