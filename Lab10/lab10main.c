/*
 * lab10main.c
 *
 *  Created on: Apr 7, 2026
 *      Author: mmistry
 */


#include "Timer.h"
#include "lcd.h"
#include "servo.h"
#include "button.h"
#include "uart-interrupt.h"
//#include <inc/tm4c123gh6pm.h>

// Uncomment or add any include directives that are needed

int main(void) {
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    servo_init();
    uart_interrupt_init();
    button_init();

    servo_move(90);
    timer_waitMillis(1000);

    servo_move(30);
    timer_waitMillis(1000);

    servo_move(150);
    timer_waitMillis(1000);

    servo_move(90);
    timer_waitMillis(1000);

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
            desiredAngle -= 1;
        } else if(currentButton == 1){
            desiredAngle += 1;
        }

        servo_move(desiredAngle-90);

        lastButton = currentButton;
        currentButton = button_getButton();

    }

}


