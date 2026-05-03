/*
 * FinalP.c
 *
 *  Created on: Apr 14, 2026
 *      Author: mmistry
 */

#include "FinalFunctions.h"
#include "Timer.h"
#include "lcd.h"
#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "open_interface.h"
#include "driverlib/interrupt.h"
#include "imu.h"

volatile char currentChar = '\0';

void main(void) {
    timer_init();
    lcd_init();
    ping_init();
    uart_interrupt_init();
    imu_init();
    imu_writeReg(IMU_OPR_MODE, COMPASS);
    imu_setDefaultUnits();


    servo_low_value = 8000; //bot 11
    servo_high_value  = 36500;

    oi_t *movement = oi_alloc();
    oi_init(movement);

    movementTunes tunes = {
        .driveDriftMultiplier    = 0,     // 0 is nothing. <0 is correct to the right; >0 is correct to the left
        .driveDistanceMultiplier = 1.0,   // 1 is nothing. <1 is less distance, >1 is more distance.
        .turnAngleMultiplier     = 0.965    // 1 is nothing. <1 is less angle, >1 is more angle.
    };





    manual_control(movement, &tunes);
    oi_free(movement);
}
