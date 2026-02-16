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
#include "timer.h"

typedef struct {
    int objectNum;
    int angle;
    double distance;
    int angularWidth;
} ScanData;

struct movementTunes{
    double turnAngleMultiplier;       // 1 is nothing. <1 is less angle, >1 is more angle.
    double driveDistanceMultiplier;   // 1 is nothing. <1 is less distance, >1 is more distance.
    double driveDriftMultiplier;      // 0 is nothing. <0 is correct to the left; >0 is correct to the right
};



void main(){
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    lcd_init();
    timer_init();
    cyBot_uart_init();
    cyBOT_init_Scan(0b0111);
    oi_setWheels(0,0);

    right_calibration_value = 280000;
    left_calibration_value = 1288000;

    struct movementTunes tunes14;
    tunes14.driveDistanceMultiplier = 1;
    tunes14.turnAngleMultiplier = 1;
    tunes14.driveDriftMultiplier = 0.0;


    char readings[91];
    float distances[91];

    char msg = cyBot_getByte();
    while(msg != 's'){
        msg = cyBot_getByte();
    }


    scanField(readings, distances);

    analyzeReadingsAndTurn(readings, distances, sensor_data, &tunes14);
}

void analyzeReadingsAndTurn(char readings[], float distances[], oi_t *sensor_data, movementTunes *t){
    ScanData scans[10];
    int currentObject = 0;
    int inObject = 0;
    int startIndex = 0;
    int widthSamples = 0;

    int i = 0;
    for (i = 0; i < 90; i++) {
        if (!inObject && readings[i] == '#') {
            // Start of a new object
            inObject = 1;
            startIndex = i;
            widthSamples = 1;

        } else if (inObject && readings[i] == '#') {
            // Continuing object
            widthSamples++;

        } else if (inObject && readings[i] == ' ') {
            // End of object
            int centerIndex = startIndex + widthSamples / 2;

            scans[currentObject].objectNum = currentObject;
            scans[currentObject].angularWidth = widthSamples * 2;
            scans[currentObject].angle = centerIndex * 2;
            scans[currentObject].distance = distances[centerIndex];

            inObject = 0;

            char temp[150];
            sprintf(temp, "Object Number: %d, Angle: %d, Distance: %f, angular width: %d\n\r", scans[currentObject].objectNum, scans[currentObject].angle, scans[currentObject].distance, scans[currentObject].angularWidth);
            sendMessage(temp);
            currentObject++;
        }
    }
    scans[currentObject].objectNum = 999; // designate the next object as nothing
    scans[currentObject].angle = 999;
    scans[currentObject].distance = 999;
    scans[currentObject].angularWidth = 999;
    int object;
    int numObjects = currentObject;
    int smallestObject = -1;
    int smallestAngle = 999;
    for(object = 0; object < numObjects; object += 1){
        if(scans[object].angularWidth < smallestAngle){
            smallestObject = object;
            smallestAngle = scans[object].angularWidth;
        }
    }
    int angleToTurn = 90 - scans[smallestObject].angle;
    if(angleToTurn > 0){
        turn_right(sensor_data, t, abs(angleToTurn));
        move_forward(sensor_data, t, scans[smallestObject].distance);
    }else if (angleToTurn < 0){
        turn_left(sensor_data, t, abs(angleToTurn));
        move_forward(sensor_data, t, scans[smallestObject].distance);
    }

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

void sendMessage(char *c) {
    while (*c != '\0') {
        cyBot_sendByte(*c);
        c += 1;
    }
}


void scanField(char readings[], float distances[]){

    cyBOT_Scan_t scanStruct;

    int currentAngle;
    double currentDist;

    //these give the servo time to get over to angle 0 so we don't get bad data.
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);
    // end of maintenance polls

    for(currentAngle = 0; currentAngle < 180; currentAngle += 2){
        cyBOT_Scan(currentAngle, &scanStruct);
        currentDist = scanStruct.sound_dist;
        distances[currentAngle/2] = currentDist;
        if(distances[currentAngle/2] > 100){
            readings[currentAngle/2] = ' ';

        }else{
            readings[currentAngle/2] = '#';
        }
        char temp[100];
        sprintf(temp, "Angle: %d, Distance: %f\n\r", currentAngle, currentDist);
        sendMessage(temp);
    }
}

