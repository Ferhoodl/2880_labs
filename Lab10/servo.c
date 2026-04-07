/*
 * servo.c
 *
 *  Created on: Apr 7, 2026
 *      Author: mmistry
 */

#include "Timer.h"
#include "lcd.h"
#include "servo.h"
#include <inc/tm4c123gh6pm.h>


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
       TIMER1_TBPR_R = 0xFF;
       TIMER1_TBILR_R = 0xE200;
       TIMER1_TBPR_R = 0xE;
       TIMER1_TBMATCHR_R = // set this to 90 degrees
       TIMER1_TBPMR_R = // set this to 90 degrees (you need the formulas and set total clock cycles)
       TIMER1_CTL_R &= ~0x4000;





}

void servo_move(uint16_t degrees){
    uint16_t milis = degrees/180 + 1;
    uint16_t clockCycles = (milis/1000)/(6.25 * 0.00000001);

    lcd_printf("Degrees: %d\nMilis: %d\nCycles: %d", degrees, milis, clockCycles);

    // this code is currently finding for the number of cycles for the high. We need to set the value for the low. Probably just do initial - clockCycles;



}
