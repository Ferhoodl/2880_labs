/*
 * servo.c
 *
 *  Created on: Apr 7, 2026
 *      Author: mmistry
 */

#include "Timer.h"
#include "lcd.h"
#include "servo.h"
#include "uart-interrupt.h"
#include "button.h"

//#include <inc/tm4c123gh6pm.h>



void servo_init(void){

    SYSCTL_RCGCTIMER_R |= 0x02;   // Timer 1
       SYSCTL_RCGCGPIO_R  |= 0x02;   // Port B
       while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}  // wait for Port B ready
       GPIO_PORTB_DIR_R    |= 0x20;   // PB5 output
       GPIO_PORTB_DEN_R    |= 0x20;   // digital enable
       GPIO_PORTB_AFSEL_R  |= 0x20;   // alternate function
       GPIO_PORTB_PCTL_R   = (GPIO_PORTB_PCTL_R & ~0x00F00000) | 0x00700000;

       TIMER1_CTL_R &= ~0x0100;       // Disable Timer 1B before configuration
       TIMER1_CFG_R = 0x04;    // Set 16-bit mode (GPTMCFG = 4 split mode)

       TIMER1_TBMR_R = (TIMER1_TBMR_R & ~0xFF) | 0x0A;
       TIMER1_TBPMR_R |= 0xB;
       TIMER1_CTL_R &= ~0x4000;
       TIMER1_TBILR_R = 0xE200;
       TIMER1_TBPR_R = 0x4;
       TIMER1_TBMATCHR_R = 0x8440; // set this to 90 degrees
       TIMER1_TBPMR_R = 0x4; // set this to 90 degrees (you need the formulas and set total clock cycles)
       TIMER1_CTL_R |= 0x0100;
}

uint32_t servo_low_value = 0;
uint32_t servo_high_value = 0;

void servo_move(int32_t degrees){

    uint32_t highClockCycles;
    double highMillis = -1;


    if(servo_low_value && servo_high_value){
        highClockCycles = servo_low_value + ((servo_high_value - servo_low_value) * degrees) / 180.0;
    }else{
        highMillis = (double)degrees/180.0 + 1;              // number of milliseconds we want the high to be to achieve the given degrees
        highClockCycles = ((double)highMillis/1000.0)/(6.25 * 0.00000001); // number of clock cycles for above millis
    }

    uint32_t lowClockCycles =  0x4E200 - highClockCycles;// 20 (0x4E200) ms  - highClockMillis (we set the register with lowClockCycles. That causes the high to be highClockCycles).

    uint16_t first16 = 0xFFFF & lowClockCycles;
    uint16_t last4 = lowClockCycles >> 16;

    TIMER1_TBMATCHR_R = first16; // gets the first 16 bits of the value (we have to split between these two registers)
    TIMER1_TBPMR_R = last4; // gets the last 4 bits of the value

    lcd_printf("Hdeg: %d\nHms: %f\nHcycs: %d", degrees, highMillis, highClockCycles);


}

void servo_calibrate(){
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    servo_init();
    uart_interrupt_init();
    button_init();

    servo_move(90);

    int currentButton = button_getButton();
    int lastButton = 999;

    int desiredAngle = 180;
    while(1)
    {
        if(currentButton == lastButton){
            lastButton = currentButton;
            currentButton = button_getButton();
            continue;
        } else if(currentButton == 4){
            desiredAngle -= 5;
        }else if(currentButton == 3){
            desiredAngle -= 1;
        }else if(currentButton == 2){
            desiredAngle += 1;
        }
        else if(currentButton == 1){
            desiredAngle += 5;
        }

        servo_move(desiredAngle-90);

        lastButton = currentButton;
        currentButton = button_getButton();

    }


}
