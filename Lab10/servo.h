/*
 * servo.h
 *
 *  Created on: Apr 7, 2026
 *      Author: mmistry
 */

#ifndef SERVO_H_
#define SERVO_H_

volatile uint32_t servo_low_value;
volatile uint32_t servo_high_value;

void servo_init(void);

void servo_move(int32_t degrees);

#endif /* SERVO_H_ */
