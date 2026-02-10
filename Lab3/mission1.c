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
    lcd_printf("test 0!");
    getMessage();
}

void getMessage(){
    int messageLength = 50;
    char message[messageLength + 1] = ""; // create message and set it to all zeros?
    int currentChar;
    int charIndex = 0;

    currentChar = cyBot_getByte();

    while (charIndex < 50 && currentChar != '\0' ){
        message[charIndex] = currentChar;
        lcd_printf("Message: %s", message); // displays input
        currentChar = cyBot_getByte();
        charIndex += 1;
    }

}
