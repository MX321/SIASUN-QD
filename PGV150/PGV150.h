/*
 * PGV150.h
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */

#ifndef PGV150_H_
#define PGV150_H_

#include <common.h>

void PGV_init();
void PGV_Send();
void PGV_Rev();
int PGV_AnalyzeData();
void PGV_Cal_Coor();

struct Coordinate_Class PGV150_coor;

int warn_flag, warn;
long int x_temp, y_temp, angle_temp;
int tag_control_num;

float x_deviation;
float y_deviation;
float angle_deviation;
int result;
int data_ok;

#endif /* PGV150_H_ */
