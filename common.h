/*
 * common.h
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */

#ifndef COMMON_H_
#define COMMON_H_



#define _BV(n) (1 << n)
#define time_s  0.01
#define M_PI 3.1415926
#define wheel_distance 581.8

typedef struct Coordinate_Class
{
	float x_coor;	//x坐标(mm)
	float y_coor;   //y坐标(mm)
	float angle_coor; //角度坐标(°)
	float angle_rad;	//角度坐标(弧度)

}Coordinate_Class_t;

typedef struct Velocity_Class
{
	float velocity_x;   //x方向速度 mm/s
	float velocity_y;   //y方向速度 mm/s
	float angular_velocity_rad;   //选择角速度rad/s
	float angular_velocity_angle;  //旋转角速度°/s
	float angular_velocity_mm;     //旋转角速度mm/s

}Velocity_Class_t;




#endif /* COMMON_H_ */
