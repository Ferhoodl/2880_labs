/*
 * movement.h
 *
 *  Created on: Feb 3, 2026
 *      Author: mmistry
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

typedef struct movementTunes movementTunes;


void move_forward(oi_t *sensor_data, movementTunes *t, double distance_mm);

void turn_right(oi_t *sensor_data, movementTunes *t, double degrees);

void turn_left(oi_t *sensor_data, movementTunes *t, double degrees);

void task1(oi_t *sensor_data, movementTunes *t);

void task2(oi_t *sensor_data, movementTunes *t);

void task3(oi_t *sensor_data, movementTunes *t);

void benchAngle(oi_t *sensor_data, movementTunes *t, int rotations);

void escapeLeft(oi_t *sensor_data, movementTunes *t);

void escapeRight(oi_t *sensor_data, movementTunes *t);

void move_backward(oi_t *sensor_data, movementTunes *t, double distance_mm);



#endif /* MOVEMENT_H_ */
