/*
 * FinalFunctions.h
 *
 *  Created on: Apr 14, 2026
 *      Author: mmistry
 */

#ifndef FINALFUNCTIONS_H_
#define FINALFUNCTIONS_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "open_interface.h"
#include "driverlib/interrupt.h"

// ADC -----------------------------------------------------------------------------
void adc_init(void);
uint32_t adc_read(void);


// BUTTON-----------------------------------------------------------------------------
void button_init();
uint8_t button_getButton();


// MOVEMENT-----------------------------------------------------------------------------
typedef struct {
    double turnAngleMultiplier;       // 1 is nothing. <1 is less angle, >1 is more angle.
    double driveDistanceMultiplier;   // 1 is nothing. <1 is less distance, >1 is more distance.
    double driveDriftMultiplier;      // 0 is nothing. <0 is correct to the left; >0 is correct to the right
} movementTunes;

void move_forward(oi_t *sensor_data, movementTunes *t, double distance_mm);
void move_backward(oi_t *sensor_data, movementTunes *t, double distance_mm);
void turn_right(oi_t *sensor_data, movementTunes *t, double degrees);
void turn_left(oi_t *sensor_data, movementTunes *t, double degrees);
void benchAngle(oi_t *sensor_data, movementTunes *t, int rotations);
void escapeLeft(oi_t *sensor_data, movementTunes *t);
void escapeRight(oi_t *sensor_data, movementTunes *t);


// PING SENSOR-----------------------------------------------------------------------------
extern volatile uint32_t g_start_time;
extern volatile uint32_t g_end_time;

void ping_init(void);
void ping_trigger(void);
void TIMER3B_Handler(void);
float ping_getDistance(void);


// SERVO-----------------------------------------------------------------------------
void servo_init(void);
void servo_move(uint16_t degrees);


// UART (basic)-----------------------------------------------------------------------------
void uart_init(void);
char uart_receive(void);
char uart_receive_nonblocking(void);


// UART Interrupt-----------------------------------------------------------------------------
extern volatile char command_byte; // byte value for special character used as a command
extern volatile int command_flag;  // flag to tell the main program a special command was received

void uart_interrupt_init(void);
void uart_sendChar(char data);
void uart_sendStr(const char *data);
void UART1_Handler(void);


#endif /* FINALFUNCTIONS_H_ */
