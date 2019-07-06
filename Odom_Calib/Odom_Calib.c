/*
 * Odom_Calib.c
 *
 *  Created on: Jun 27, 2019
 *      Author: siasunhebo
 */


#include <Odom_Calib.h>
#include <common.h>
#include <math.h>
#include <PGV150.h>
#include <Trigonometric.h>

static float velocity_x = 0;
static float angular_velocity_angle = 0.0;
struct Coordinate_Class Coor_delta;
static float delta_distance = 0.0;
static float angle = 0.0;
struct Coordinate_Class Odom_Coor;

//************************************
// FullName:    Odom_Calib
// Returns:   NULL
// Parameter: NULL
// Parameter: NULL
// Description: 轮式里程计计算
//************************************
Coordinate_Class_t Odom_Calib()
{

	delta_distance = velocity_x * time_s;

	Coor_delta.angle_coor = angular_velocity_angle * time_s;

	angle += Coor_delta.angle_coor;

	angle = Angle_Trans(angle, 0);

	Odom_Coor.angle_coor = angle;
	//Coor_delta.x_coor += delta_distance * Cos_Lookup(Odom_Coor.angle_coor);
	Coor_delta.x_coor += delta_distance * cos(Odom_Coor.angle_coor * M_PI / 180.0);
	//Coor_delta.y_coor += delta_distance * Sin_Lookup(Odom_Coor.angle_coor);
	Coor_delta.y_coor += delta_distance * sin(Odom_Coor.angle_coor * M_PI / 180.0);

	Odom_Coor.x_coor = PGV150_coor.x_coor + Coor_delta.x_coor;
	Odom_Coor.y_coor = PGV150_coor.y_coor + Coor_delta.y_coor;

	return Odom_Coor;
}


//************************************
// Method:    Angle_Trans
// Returns:   float
// Parameter: float angle
// Parameter: const float base
// Description: 将angle缩放至base同一周期内(+180°~-180°)
//************************************
float Angle_Trans(float angle, const float base)
{
	int k = (int)((angle - base) / 360.0f);
	angle -= k * 360;
	if (angle - base >= 180.0f)
	{
		angle -= 360.0f;
	}
	else if (angle - base < -180.0f)
	{
		angle += 360.0f;
	}
	return angle;
}
