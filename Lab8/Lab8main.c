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
            uint16_t t = rawValues;
            double distance = 7e6 * pow(t, -1.697);

            lcd_clear();
            lcd_printf("ADC: %d  D: %.2f", rawValues, distance);

            timer_waitMillis(1000);  // slow update so display is readable
        }
}



