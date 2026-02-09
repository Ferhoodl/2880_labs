/*
 * movement.c
 *
 *  Created on: Feb 3, 2026
 *      Author: mmistry
 */

#include "open_interface.h"
#include "lcd.h"
#include "stdio.h"

#include "movement.h"

struct movementTunes{
    double turnAngleMultiplier;       // 1 is nothing. <1 is less angle, >1 is more angle.
    double driveDistanceMultiplier;   // 1 is nothing. <1 is less distance, >1 is more distance.
    double driveDriftMultiplier;      // 0 is nothing. <0 is correct to the left; >0 is correct to the right
};

void main()
{
    oi_t *sensor_data = oi_alloc();

    struct movementTunes tunes;
    tunes.driveDistanceMultiplier = 0.998;
    tunes.turnAngleMultiplier = 0.925;
    tunes.driveDriftMultiplier = 0.0;

    oi_init(sensor_data);
    lcd_init();

    task3(sensor_data, &tunes);
    //benchAngle(sensor_data, &tunes, 4);

    oi_free(sensor_data);
}

void task1(oi_t *sensor_data, movementTunes *t){
    move_forward(sensor_data, t, 1000);
}

void task2(oi_t *sensor_data, movementTunes *t){
    move_forward(sensor_data, t, 500);
    turn_right(sensor_data, t, 90);

    move_forward(sensor_data, t, 500);
    turn_right(sensor_data, t, 90);

    move_forward(sensor_data, t, 500);
    turn_right(sensor_data, t, 90);

    move_forward(sensor_data, t, 500);
    turn_right(sensor_data, t, 90);
}

void task3(oi_t *sensor_data, movementTunes *t){
    double accumDistance = 0;
    double distBeforeEscape = 0;
    double distAfterEscape = 0;
    oi_update(sensor_data);

    while(accumDistance < 2000){
        oi_update(sensor_data);

        if (sensor_data -> bumpLeft == 0 & sensor_data -> bumpRight == 0){
            oi_setWheels(50,50);
        }
        else if (sensor_data -> bumpLeft == 1){
            distBeforeEscape = accumDistance;
            escapeRight(sensor_data, t);
            distAfterEscape = accumDistance;
        }

        else if (sensor_data -> bumpRight == 1){
            escapeLeft(sensor_data, t);
        }
        accumDistance += sensor_data -> distance;
        lcd_printf("A. dist: %f", accumDistance);
    }
}

void benchAngle(oi_t *sensor_data, movementTunes *t, int rotations){
    turn_right(sensor_data, t, rotations * 360);
}

void escapeLeft(oi_t *sensor_data, movementTunes *t){
    move_backward(sensor_data, t, 150);
    turn_left(sensor_data, t, 90);
    move_forward(sensor_data, t, 250);
    turn_right(sensor_data, t, 90);
    move_forward(sensor_data, t, 150);
}

void escapeRight(oi_t *sensor_data, movementTunes *t){
    move_backward(sensor_data, t, 150);
    turn_right(sensor_data, t, 90);
    move_forward(sensor_data, t, 250);
    turn_left(sensor_data, t, 90);
    move_forward(sensor_data, t, 150);
}

void move_forward(oi_t *sensor_data, movementTunes *t, double distance_mm){
    double sum = 0;
    double wheelspeed = 50; // mm/sec
    double driveAdjustVal = ((t->driveDriftMultiplier * wheelspeed) / 2);
    oi_setWheels(wheelspeed + driveAdjustVal, wheelspeed - driveAdjustVal); // applying driveAdjustVal, the right is driveDriftMultiplier% more than the left
        while (sum < distance_mm){
            oi_update(sensor_data);
            sum += sensor_data -> distance * t->driveDistanceMultiplier;

            lcd_printf("dist: %f", sum);
        }
    oi_setWheels(0,0);
}

void move_backward(oi_t *sensor_data, movementTunes *t, double distance_mm){
    double sum = 0;
    oi_setWheels(-50,-50);
        while (sum < distance_mm){
            oi_update(sensor_data);
            sum += abs(sensor_data -> distance * t->driveDistanceMultiplier);

            lcd_printf("dist: %f", -sum);
        }
    oi_setWheels(0,0);
}



void turn_right(oi_t *sensor_data, movementTunes *t, double degrees){
    oi_update(sensor_data);
    double angleDiff = 0.0;
    while(abs(angleDiff) < degrees * t->turnAngleMultiplier){
        oi_setWheels(-50,50);
        angleDiff += sensor_data->angle;
        lcd_printf("angle: %f", angleDiff);
        oi_update(sensor_data);
    }
    oi_setWheels(0,0);

}

void turn_left(oi_t *sensor_data, movementTunes *t, double degrees){

    oi_update(sensor_data);
    double angleDiff = 0;
    while(abs(angleDiff) < degrees * t->turnAngleMultiplier){
        oi_setWheels(50,-50);
        angleDiff += sensor_data->angle;
        lcd_printf("angle: %f", angleDiff);
        oi_update(sensor_data);
    }
    oi_setWheels(0,0);
}

