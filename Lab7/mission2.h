/*
 * mission1.h
 *
 *  Created on: Feb 10, 2026
 *      Author: mmistry
 */

#ifndef MISSION1_H_
#define MISSION1_H_

#include "open_interface.h"
#include "lcd.h"
#include "stdio.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"

typedef struct {
    int objectNum;
    int angle;
    double distance;
    int angularWidth;
} ScanData;

typedef struct{
    char binaryPing[91];
    char binaryIR[91];

    float rawPing[91];
    float rawIR[91];
}rawScannerDatas;

void getMessage();

void scanField(rawScannerDatas rawDatas);

void sendMessage(char *c);

void analyzeReadingsAndTurn(rawScannerDatas rawDatas, oi_t *sensor_data, movementTunes *t);


#endif /* MISSION1_H_ */
