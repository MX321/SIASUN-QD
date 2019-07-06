/*
 * Odom_Calib.h
 *
 *  Created on: Jun 27, 2019
 *      Author: siasunhebo
 */

#ifndef ODOM_CALIB_H_
#define ODOM_CALIB_H_

#include <common.h>
#include <stdio.h>

float Angle_Trans(float angle, const float base);
//float angle = 0.0;
Coordinate_Class_t Odom_Calib();



#endif /* ODOM_CALIB_H_ */
