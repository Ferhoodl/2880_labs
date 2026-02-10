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
    getMessage();
}

void getMessage(){
    char currentChar;
    char message[50];
    int charIndex = 0;
    currentChar = cyBot_getByte();

    cyBot_uart_init();
    lcd_init();

    while (charIndex < 50 && currentChar != '\0' ){
        message[charIndex] = currentChar;
        lcd_printf("%s", message); // displays input
    }

}
