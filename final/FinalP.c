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
        .driveDriftMultiplier    = 0,
        .driveDistanceMultiplier = .98,
        .turnAngleMultiplier     = .95
    };

    manual_control(movement, &tunes);
    oi_free(movement);
}
