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
#include "mission2.h"
#include "timer.h"

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


    right_calibration_value = 285250;
    left_calibration_value = 1240750;

    struct movementTunes tunes14;
    tunes14.driveDistanceMultiplier = 1;
    tunes14.turnAngleMultiplier = 1;
    tunes14.driveDriftMultiplier = 0.0;


    rawScannerDatas rawDatas;

    scanField(*rawDatas);

    analyzeReadingsAndTurn(rawDatas, sensor_data, &tunes14);

}

void analyzeReadingsAndTurn(rawScannerDatas rawDatas, oi_t *sensor_data, movementTunes *t){
    ScanData scans[10];
    int currentObject = 0;
    int inObject = 0;
    int startIndex = 0;
    int widthSamples = 0;

    int i = 0;
    for (i = 0; i < 90; i++) {
        if (!inObject && rawDatas.binaryIR[i] == '#') {
            // Start of a new object
            inObject = 1;
            startIndex = i;
            widthSamples = 1;

        } else if (inObject && rawDatas.binaryIR[i] == '#') {
            // Continuing object
            widthSamples++;

        } else if (inObject && rawDatas.binaryIR[i] == ' ') {
            // End of object
            int centerIndex = startIndex + widthSamples / 2;

            scans[currentObject].objectNum = currentObject;
            scans[currentObject].angularWidth = widthSamples * 2;
            scans[currentObject].angle = centerIndex * 2;
            scans[currentObject].distance = rawDatas.rawPing[centerIndex];

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
        if(scans[object].angularWidth < smallestAngle && scans[object].angularWidth > 6){
            smallestObject = object;
            smallestAngle = scans[object].angularWidth;
        }
    }
    int angleToTurn = 90 - scans[smallestObject].angle;
    if(angleToTurn > 0){
        turn_right(sensor_data, t, abs(angleToTurn) * .8);
        move_forward(sensor_data, t, scans[smallestObject].distance*10 * .8);
    }else if (angleToTurn < 0){
        turn_left(sensor_data, t, abs(angleToTurn) * .8);
        move_forward(sensor_data, t, scans[smallestObject].distance*10 * .8);
    }

}

/*
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
*/

void sendMessage(char *c) {
    while (*c != '\0') {
        cyBot_sendByte(*c);
        c += 1;
    }
}


void scanField(rawScannerDatas *rawDatas){

    cyBOT_Scan_t scanStruct;

    int currentAngle;
    double pingDist;
    double irDist;


    //these give the servo time to get over to angle 0 so we don't get bad data.
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);
    // end of maintenance polls

    for(currentAngle = 0; currentAngle < 180; currentAngle += 2){
        cyBOT_Scan(currentAngle, &scanStruct); // do the scan

        pingDist = scanStruct.sound_dist; // set current ping distance
        irDist = scanStruct.IR_raw_value; // set current ir distance

        rawDatas.rawPing[currentAngle/2] = pingDist; // assign appropriate location in ping array
        rawDatas.rawIR[currentAngle/2] = irDist;    // assign appropriate location in ir array

        if(rawDatas.rawPing[currentAngle/2] > 100){ // assign appropriate symbol in binary ping array
            rawDatas.binaryPing[currentAngle/2] = ' ';
        }else{
            rawDatas.binaryPing[currentAngle/2] = '#';
        }

        if(rawIR[currentAngle/2] > 100){ // assign appropriate symbol in binary ir array
            rawDatas.binaryIR[currentAngle/2] = ' ';
        }else{
            rawDatas.binaryIR[currentAngle/2] = '#';
        }

        char temp[100];
        sprintf(temp, "Angle: %d, ping distance: %f, ir distance: %f\n\r", currentAngle, pingDist, irDist);
        sendMessage(temp);
    }
}

