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

volatile char currentChar = '\0';

void main(void) {
    timer_init();
    lcd_init();
    ping_init();
    uart_interrupt_init();

    oi_t *movement = oi_alloc();
    oi_init(movement);

    movementTunes tunes = {
        .driveDriftMultiplier    = 0,     // 0 is nothing. <0 is correct to the left; >0 is correct to the right
        .driveDistanceMultiplier = 1.05,   // 1 is nothing. <1 is less distance, >1 is more distance.
        .turnAngleMultiplier     = .95    // 1 is nothing. <1 is less angle, >1 is more angle.
    };

    manual_control(movement, &tunes);
    oi_free(movement);
}
