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
    cyBOT_init_Scan(0b0111);

    right_calibration_value = 269500;
    left_calibration_value = 1204000;

    char readings[91];

    scanField(readings);
    //getMessage();
    int test = 0;
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

void scanField(char readings[]){
    float distances[91];

    cyBOT_Scan_t scanStruct;

    int currentAngle;
    for(currentAngle = 0; currentAngle < 180; currentAngle += 2){
        cyBOT_Scan(currentAngle, &scanStruct);
        distances[currentAngle/2] = scanStruct.sound_dist;
        if(distances[currentAngle/2] > 100){
            readings[currentAngle/2] = ' ';

        }else{
            readings[currentAngle/2] = '#';
        }
    }
    int test = 0;
}

