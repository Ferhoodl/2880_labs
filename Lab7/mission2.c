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

    right_calibration_value = 353500; //bot10
    left_calibration_value  = 1377250;

    movementTunes tunes14;
    tunes14.driveDistanceMultiplier = 1;
    tunes14.turnAngleMultiplier     = 1;
    tunes14.driveDriftMultiplier    = 0.0;

    rawScannerDatas rawDatas;

    scanField2(&rawDatas);                                    // Part 1: scan
    analyzeReadingsAndTurn2(&rawDatas, sensor_data, &tunes14); // Parts 2 & 4

}

// Part 3: sendMessage2 uses our own uart_sendChar
void sendMessage2(char *c) {
    while (*c != '\0') {
        uart_sendChar(*c);
        c++;
    }
}

// Part 1: scan field
//   - Average IR_SAMPLES raw IR readings at each angle for noise reduction
//   - Store raw PING and averaged raw IR values
//   - Use IR binary map to detect object edges
void scanField2(rawScannerDatas *rawDatas){
    cyBOT_Scan_t scanStruct;
    int currentAngle;
    int s;
    double irSum;

    // Settle the servo at 0 before scanning
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);
    cyBOT_Scan(0, &scanStruct);

    for (currentAngle = 0; currentAngle < 180; currentAngle += 2) {
        int idx = currentAngle / 2;

        // Average multiple IR readings at this angle (Part 1, step 3)
        irSum = 0;
        for (s = 0; s < IR_SAMPLES; s++) {
            cyBOT_Scan(currentAngle, &scanStruct);
            irSum += scanStruct.IR_raw_val;
        }
        rawDatas->rawIR[idx] = irSum / IR_SAMPLES;

        // PING distance (single reading is sufficient here;
        // the second scan per angle also re-settles the servo)
        cyBOT_Scan(currentAngle, &scanStruct);
        rawDatas->rawPing[idx] = scanStruct.sound_dist;

        // Binary maps
        rawDatas->binaryPing[idx] = (rawDatas->rawPing[idx] > 100) ? ' ' : '#';

        // IR threshold: objects produce a HIGHER raw ADC value (closer = higher voltage)
        // Adjust threshold as needed for your bot and test field
        rawDatas->binaryIR[idx]   = (rawDatas->rawIR[idx] < 900) ? ' ' : '#';

        char temp[100];
        sprintf(temp, "Angle: %d, ping: %.2f cm, IR avg: %.1f\n\r",
                currentAngle, rawDatas->rawPing[idx], rawDatas->rawIR[idx]);
        sendMessage2(temp);
    }
}

// Parts 2 & 4: find objects using IR edges, compute linear width,
//              drive to the smallest-width object while avoiding bumps.
void analyzeReadingsAndTurn2(rawScannerDatas *rawDatas, oi_t *sensor_data, movementTunes *t){
    ScanData scans[10];
    int currentObject = 0;
    int inObject      = 0;
    int startIndex    = 0;
    int widthSamples  = 0;
    int i;

    // Part 1 / Part 2: detect object edges from IR binary map,
    //                  then do a targeted PING scan at the midpoint.
    cyBOT_Scan_t scanStruct;

    // Settle servo first
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
            // End of object — compute midpoint angle and PING it (Part 1, step 5)
            int centerIdx   = startIndex + widthSamples / 2;
            int centerAngle = centerIdx * 2;

            cyBOT_Scan(centerAngle, &scanStruct);
            cyBOT_Scan(centerAngle, &scanStruct); // second read for stability
            double pingDist = scanStruct.sound_dist;

            int angularWidth = widthSamples * 2; // degrees

            if (angularWidth < 4) {
                inObject = 0;
                continue;
            }

            // Part 2: linear width from angular width and distance
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

    // Part 4: find the smallest LINEAR width object (> a noise floor)
    int    smallestObject = -1;
    double smallestWidth  = 9999.0;

    for (i = 0; i < numObjects; i++) {
        // Ignore objects that are clearly noise (< 2 cm linear width)
        if (scans[i].linearWidth < smallestWidth && scans[i].linearWidth > 2.0) {
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

    // Turn to face the target (servo is at 0 = right side; 90 deg = straight ahead)
    int angleToTurn = 90 - scans[smallestObject].centerAngle;

    if (angleToTurn > 0) {
        turn_right(sensor_data, t, (double)angleToTurn);
    } else if (angleToTurn < 0) {
        turn_left(sensor_data, t, (double)(-angleToTurn));
    }

    // Drive to within 10 cm of the object, stopping if bumped
    double driveDistMm = (scans[smallestObject].distance - 10.0) * 10.0; // cm -> mm
    if (driveDistMm < 0) driveDistMm = 0;

    // move_forward_avoid handles bump detection; falls back to escape maneuvers
    move_forward_avoid(sensor_data, t, driveDistMm);
}

// Part 4 helper: drive forward the requested distance; if a bump occurs,
// execute an escape and resume.  Uses existing escapeLeft/escapeRight from movement.c.
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
