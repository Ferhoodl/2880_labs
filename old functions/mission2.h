/*
 * mission2.h
 *
 *  Created on: Feb 10, 2026
 *      Author: mmistry
 */

#ifndef MISSION2_H_
#define MISSION2_H_

#include "open_interface.h"
#include "lcd.h"
#include "stdio.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"


void getMessage();

typedef struct {
    double rawPing[90];
    double rawIR[90];
    char   binaryPing[90];
    char   binaryIR[90];
} rawScannerDatas;

typedef struct {
    int    objectNum;
    int    startAngle;
    int    endAngle;
    int    centerAngle;
    double distance;
    int    angularWidth;
    double linearWidth;
} ScanData;

void scanField2(rawScannerDatas *rawDatas);
void sendMessage(char *c);
void analyzeReadingsAndTurn2(rawScannerDatas *rawDatas, oi_t *sensor_data, movementTunes *t);
void move_forward_avoid(oi_t *sensor_data, movementTunes *t, double distance_mm);


#endif /* MISSION2_H_ */
