/*
 * Lab8main.c
 *
 *  Created on: Mar 24, 2026
 *      Author: mmistry
 */

#include "open_interface.h"
#include "lcd.h"
#include "stdio.h"
#include "math.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "mission2.h"
#include "timer.h"
#include "uart-interrupt.h"
#include "adc.h"

int main(void){

    lcd_init();
    adc_init();

        while(1) {
            uint16_t rawValues = adc_read();

            lcd_clear();
            lcd_printf("ADC: %d", rawValues);

                timer_waitMillis(200);  // slow update so display is readable
                }
}



