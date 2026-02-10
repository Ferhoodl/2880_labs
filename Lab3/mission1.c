/*
 * mission1.c
 *
 *  Created on: Feb 10, 2026
 *      Author: mmistry
 *      Author: iwsteele
 */

#include "open_interface.h"
#include "lcd.h"
#include "stdio.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"
#include "mission1.h"

void main(){
    lcd_init();
    cyBot_uart_init();
    cyBOT_init_Scan(0111);

    cyBOT_SERVO_cal();
    //findObject();
    //getMessage();
}

void getMessage(){
    int messageLength = 50;
    char message[messageLength + 1];
    int currentChar;
    int charIndex = 0;

    currentChar = cyBot_getByte();
    while (charIndex < 50 && currentChar != '\0' ){
        message[charIndex] = currentChar;
        lcd_printf("Message: %s", message); // displays input
        cyBot_sendByte(currentChar);
        currentChar = cyBot_getByte();
        charIndex += 1;
    }

}

void findObject(){
    float distances[91];

    cyBOT_Scan_t *scanStruct;
    cyBOT_Scan(90, &scanStruct);

    /*
    currentAngle = 0;
    for()
    */

}

