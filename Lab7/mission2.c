/*
 * mission2.c
 *
 *  Created on: Feb 10, 2026
 *      Author: mmistry
 *      Author: iwsteele
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


// Number of IR samples to average at each angle (Part 1)
#define IR_SAMPLES 3

void main(){
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    uart_interrupt_init();   // Part 3: use our own UART, not cyBot_uart_init()
    cyBOT_init_Scan(0b0111);
    oi_setWheels(0, 0);

    right_calibration_value = 269500; //bot04
    left_calibration_value  = 1204000;

    movementTunes tunes14;
    tunes14.driveDistanceMultiplier = 1;
    tunes14.turnAngleMultiplier     = 1;
    tunes14.driveDriftMultiplier    = 0.0;

    rawScannerDatas rawDatas;

    scanField2(&rawDatas);                                    // Part 1: scan
    analyzeReadingsAndTurn2(&rawDatas, sensor_data, &tunes14); // Parts 2 & 4

}


void sendMessage2(char *c) {
    while (*c != '\0') {
        uart_sendChar(*c);
        c++;
    }
}

//   - Average IR_SAMPLES by avergaing 3 scans
//   - Store raw PING and averaged raw IR values
void scanField2(rawScannerDatas *rawDatas){
    cyBOT_Scan_t scanStruct;
    int currentAngle;
    int s;
    double irSum;

    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);

    for (currentAngle = 0; currentAngle < 180; currentAngle += 2) {
        int ave = currentAngle / 2;

        irSum = 0;                                  // Average multiple IR readings at this angle
        for (s = 0; s < IR_SAMPLES; s++) {
            cyBOT_Scan(currentAngle, &scanStruct);
            irSum += scanStruct.IR_raw_val;
        }
        rawDatas->rawIR[ave] = irSum / IR_SAMPLES;

                cyBOT_Scan(currentAngle, &scanStruct);         // PING distance
        rawDatas->rawPing[ave] = scanStruct.sound_dist;

        rawDatas->binaryPing[ave] = (rawDatas->rawPing[ave] > 100) ? ' ' : '#';

        rawDatas->binaryIR[ave]  = (rawDatas->rawIR[ave] < 900) ? ' ' : '#';

        char temp[100];
        sprintf(temp, "Angle: %d, ping: %.2f cm, IR avg: %.1f\n\r",
                currentAngle, rawDatas->rawPing[ave], rawDatas->rawIR[ave]);
        sendMessage2(temp);
    }
}


void analyzeReadingsAndTurn2(rawScannerDatas *rawDatas, oi_t *sensor_data, movementTunes *t){
    ScanData scans[10];
    int currentObject = 0;
    int inObject      = 0;
    int startIndex    = 0;
    int widthSamples  = 0;
    int i;

    cyBOT_Scan_t scanStruct;

    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);

    for (i = 0; i < 90; i++) {
        if (!inObject && rawDatas->binaryIR[i] == '#') {
            // Start of object
            inObject     = 1;
            startIndex   = i;
            widthSamples = 1;

        } else if (inObject && rawDatas->binaryIR[i] == '#') {
            widthSamples++;

        } else if (inObject && rawDatas->binaryIR[i] == ' ' && rawDatas->binaryIR[i+1] == ' ' ) {

            int centerIdx   = startIndex + widthSamples / 2; //compute midpoint angle and PING it
            int centerAngle = centerIdx * 2;

            cyBOT_Scan(centerAngle, &scanStruct);
            cyBOT_Scan(centerAngle, &scanStruct); //read twice to make sure it works
            double pingDist = scanStruct.sound_dist;

            int angularWidth = widthSamples * 2; // degrees

            if (angularWidth < 4) {
                inObject = 0;
                continue;
            }


            // w = 2 * d * tan(angularWidth/2)   (d in cm, result in cm)
            double halfAngleRad = (angularWidth / 2.0) * (3.14159265 / 180.0);
            double linearWidth  = 2.0 * pingDist * tan(halfAngleRad);

            scans[currentObject].objectNum    = currentObject;
            scans[currentObject].startAngle   = startIndex * 2;
            scans[currentObject].endAngle     = (startIndex + widthSamples - 1) * 2;
            scans[currentObject].centerAngle  = centerAngle;
            scans[currentObject].distance     = pingDist;
            scans[currentObject].angularWidth = angularWidth;
            scans[currentObject].linearWidth  = linearWidth;

            inObject = 0;

            char temp[200];
            sprintf(temp,
                "Object %d: center=%d deg, dist=%.2f cm, "
                "angular width=%d deg, linear width=%.2f cm\n\r",
                scans[currentObject].objectNum,
                scans[currentObject].centerAngle,
                scans[currentObject].distance,
                scans[currentObject].angularWidth,
                scans[currentObject].linearWidth);
            sendMessage2(temp);

            currentObject++;
            if (currentObject >= 10) break;
        }
    }

    int numObjects = currentObject;
    if (numObjects == 0) {
        sendMessage2("No objects detected.\n\r");
        return;
    }

    int    smallestObject = -1; //find smallest object
    double smallestWidth  = 9999.0;

    for (i = 0; i < numObjects; i++) {
        if (scans[i].linearWidth < smallestWidth && scans[i].linearWidth > 2.0) { //help ignore noise
            smallestObject = i;
            smallestWidth  = scans[i].linearWidth;
        }
    }

    if (smallestObject == -1) {
        sendMessage2("No valid target found.\n\r");
        return;
    }

    char msg[100];
    sprintf(msg, "Driving to object %d (linear width=%.2f cm)\n\r",
            smallestObject, scans[smallestObject].linearWidth);
    sendMessage2(msg);

    int angleToTurn = 90 - scans[smallestObject].centerAngle;   // Turn to face the target
    double driveDistMm = (scans[smallestObject].distance - 10.0) * 10.0;      // Drive to within 10 cm of the object
    if (driveDistMm < 0) driveDistMm = 0;

    double d = scans[smallestObject].distance * 10.0; // convert cm -> mm
    double s = 10; // offset in mm of ir sensor to center of robot.
    double a = angleToTurn;
    angleToTurn = (180.0/3.14) * atan2(d * sin(a * (3.14/180.0)), d * cos(a * (3.14/180.0)) + s);

    char temp[100];
    sprintf(temp, "Angle to turn: %f", angleToTurn);
    sendMessage2(temp);


    if (angleToTurn > 0) {
        turn_right(sensor_data, t, (double)angleToTurn);
    } else if (angleToTurn < 0) {
        turn_left(sensor_data, t, (double)(-angleToTurn));
    }


    move_forward_avoid(sensor_data, t, driveDistMm);
}

void move_forward_avoid(oi_t *sensor_data, movementTunes *t, double distance_mm){
    double sum = 0;
    double wheelspeed = 50;
    double driveAdjustVal = ((t->driveDriftMultiplier * wheelspeed) / 2);

    oi_setWheels(wheelspeed + driveAdjustVal, wheelspeed - driveAdjustVal);

    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data->distance * t->driveDistanceMultiplier;

        lcd_printf("dist: %.0f mm", sum);

        // Bump left
        if (sensor_data->bumpLeft) {
            oi_setWheels(0, 0);
            sendMessage2("Bump left — escaping right\n\r");
            escapeRight(sensor_data, t);
            // Resume driving
            oi_setWheels(wheelspeed + driveAdjustVal, wheelspeed - driveAdjustVal);
        }

        // Bump right
        if (sensor_data->bumpRight) {
            oi_setWheels(0, 0);
            sendMessage2("Bump right — escaping left\n\r");
            escapeLeft(sensor_data, t);
            oi_setWheels(wheelspeed + driveAdjustVal, wheelspeed - driveAdjustVal);
        }
    }

    oi_setWheels(0, 0);
}
