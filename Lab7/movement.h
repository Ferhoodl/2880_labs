/*
 * movement.h
 *
 *  Created on: Feb 3, 2026
 *      Author: mmistry
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

typedef struct {
    double turnAngleMultiplier;       // 1 is nothing. <1 is less angle, >1 is more angle.
    double driveDistanceMultiplier;   // 1 is nothing. <1 is less distance, >1 is more distance.
    double driveDriftMultiplier;      // 0 is nothing. <0 is correct to the left; >0 is correct to the right
}movementTunes;

void move_forward(oi_t *sensor_data, movementTunes *t, double distance_mm);

void turn_right(oi_t *sensor_data, movementTunes *t, double degrees);

void turn_left(oi_t *sensor_data, movementTunes *t, double degrees);

void benchAngle(oi_t *sensor_data, movementTunes *t, int rotations);

void escapeLeft(oi_t *sensor_data, movementTunes *t);

void escapeRight(oi_t *sensor_data, movementTunes *t);

void cheeseEscape(oi_t *sensor_data, movementTunes *t);

void move_backward(oi_t *sensor_data, movementTunes *t, double distance_mm);

#endif /* MOVEMENT_H_ */
