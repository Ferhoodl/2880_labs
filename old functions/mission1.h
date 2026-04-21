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


void getMessage();

void scanField(char readings[], float distances[]);

void sendMessage(char *c);

void analyzeReadingsAndTurn(char readings[], float distances[], oi_t *sensor_data, movementTunes *t);


#endif /* MISSION1_H_ */
