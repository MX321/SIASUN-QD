/*
 * Movement.c
 *
 *  Created on: Jun 28, 2019
 *      Author: siasunhebo
 */


#include <Movement.h>
#include <math.h>


extern float target_velocity;



Velocity_Class_t Velocity_Absolute_To_Relative(Velocity_Class_t Absolute_Velocity, Velocity_Class_t Relative_Velocity, Coordinate_Class_t Base_Coor)
{
	float Cos_Angle = cos(Base_Coor.angle_coor * M_PI / 180.0);
	float Sin_Angle = sin(Base_Coor.angle_coor * M_PI / 180.0);

	Relative_Velocity.velocity_x = Cos_Angle * Absolute_Velocity.velocity_x + Sin_Angle * Absolute_Velocity.velocity_y;
	Relative_Velocity.velocity_y = -Sin_Angle * Absolute_Velocity.velocity_x + Cos_Angle * Absolute_Velocity.velocity_y;
	Relative_Velocity.angular_velocity_angle = Absolute_Velocity.angular_velocity_angle;
	Relative_Velocity.angular_velocity_mm = Absolute_Velocity.angular_velocity_mm;
	Relative_Velocity.angular_velocity_rad = Absolute_Velocity.angular_velocity_rad;

	return Relative_Velocity;
}

//************************************
// FullName:  Movement_Init
// Returns:   int
// Parameter: struct Coordinate_Class Origin_Coor 起点坐标
// Parameter: struct Interpolation_Parameter_t Input_Para  插补参数
// Description: 计算加减速时间及距离
//************************************
int Movement_Init(struct Coordinate_Class Origin_Coor, struct Interpolation_Parameter_t Input_Para)
{
	int temp_result;
	Origin_Coor_InWorld = Origin_Coor;  //保存起点坐标
	Destination_Coor_InOrigin.x_coor = Destination_Coor_InWorld.x_coor - Origin_Coor_InWorld.x_coor;    //计算终点在起点坐标系下的相对坐标
	Destination_Coor_InOrigin.y_coor = Destination_Coor_InWorld.y_coor - Origin_Coor_InWorld.y_coor;
	Destination_Coor_InOrigin.angle_coor = Destination_Coor_InWorld.angle_coor - Origin_Coor_InWorld.angle_coor;
	float input_distance_abs = Cal_Destination_Displacement(Destination_Coor_InOrigin);   //计算待插补的距离
	Update_Interpolation_Parameter(Input_Para);    //更新插补参数
	temp_result = Interpolation_Init(input_distance_abs);
	return temp_result;
}

//************************************
// FullName:  Cal_Projection_Coor
// Returns:   Coordinate_Class_t
// Parameter: struct Coordinate_Class Destination_Coor_InOrigin
// Description: 当前位置距起点位置
//************************************
Coordinate_Class_t Cal_Projection_Coor(Coordinate_Class_t Current_Coor_InOrigin)
{
	Coordinate_Class_t Projection_Coor;
	float X_H_mul_y = x_temp_InOrigin * Current_Coor_InOrigin.x_coor + y_temp_InOrigin * Current_Coor_InOrigin.y_coor + angle_equivalent_temp_InOrigin * Current_Coor_InOrigin.angle_coor * M_PI / 180.0 * (wheel_distance / 2.0);

	float k = X_H_mul_y / X_H_mul_X;

	Projection_Coor.x_coor = Destination_Coor_InOrigin.x_coor * k;
	Projection_Coor.y_coor = Destination_Coor_InOrigin.y_coor * k;
	Projection_Coor.angle_coor = Destination_Coor_InOrigin.angle_coor * k;
	Projection_Coor.angle_rad = Destination_Coor_InOrigin.angle_rad * k;

	return Projection_Coor;
}

//************************************
// FullName:  Cal_Current_Coor_InOrigin
// Returns:   float
// Parameter: struct Coordinate_Class Current_Coor_InOrigin
// Description: 当前位置距起点位置
//************************************
float Cal_Current_Coor_InOrigin(Coordinate_Class_t Current_Coor_InOrigin)
{
	float angle_equivalent_temp, distance_temp;
	angle_equivalent_temp = Current_Coor_InOrigin.angle_coor * M_PI / 180.0 * (wheel_distance / 2.0);
	distance_temp = Current_Coor_InOrigin.x_coor * Current_Coor_InOrigin.x_coor + Current_Coor_InOrigin.y_coor * Current_Coor_InOrigin.y_coor + angle_equivalent_temp * angle_equivalent_temp;
	distance_temp = sqrtf(distance_temp);
	return distance_temp;
}

//************************************
// FullName:  Assign_Velocity
// Returns:   Velocity_Class_t
// Parameter: Coordinate_Class_t Current_Coor_InWorld
// Parameter: float velocity
// Description: 根据当前坐标在起点坐标系下的坐标，将速度分配给各个轴
//************************************
Velocity_Class_t Assign_Velocity(Coordinate_Class_t Current_Coor_InWorld, float velocity)
{
	float k = 0.0;

	k = x_temp_InOrigin / distance_InOrigin_ABS;
	Target_Velocity_InAGV.velocity_x = k * velocity;

	k = y_temp_InOrigin / distance_InOrigin_ABS;
	Target_Velocity_InAGV.velocity_y = k * velocity;

	k = angle_equivalent_temp_InOrigin / distance_InOrigin_ABS;
	Target_Velocity_InAGV.angular_velocity_mm = k * velocity;

	return Target_Velocity_InAGV;
}



//************************************
// FullName:  Cal_Destination_Displacement
// Returns:   float
// Parameter: struct Coordinate_Class Destination_Coor_InOrigin
// Description: 计算待插补的距离
//************************************
float Cal_Destination_Displacement(Coordinate_Class_t Destination_Coor_InOrigin)
{
	//储存各个轴在起点坐标的距离
	x_temp_InOrigin = Destination_Coor_InOrigin.x_coor;
	y_temp_InOrigin = Destination_Coor_InOrigin.y_coor;
	angle_equivalent_temp_InOrigin = Destination_Coor_InOrigin.angle_coor * M_PI / 180.0 * (wheel_distance / 2.0);

	X_H_mul_X = x_temp_InOrigin * x_temp_InOrigin + y_temp_InOrigin * y_temp_InOrigin + angle_equivalent_temp_InOrigin * angle_equivalent_temp_InOrigin;

	//二范数
	distance_InOrigin_ABS = x_temp_InOrigin * x_temp_InOrigin + y_temp_InOrigin * y_temp_InOrigin + angle_equivalent_temp_InOrigin * angle_equivalent_temp_InOrigin;

	distance_InOrigin_ABS = sqrtf(distance_InOrigin_ABS);

	return distance_InOrigin_ABS;
}

//************************************
// FullName:  Movement_Cal_Velocity
// Returns:   int
// Parameter: Coordinate_Class_t Current_Coor_InWorld
// Description: 插补速度
//************************************
int Movement_Cal_Velocity(Coordinate_Class_t Current_Coor_InWorld)
{
	Coordinate_Class_t Current_Coor_InOrigin;
	Current_Coor_InOrigin.x_coor = Current_Coor_InWorld.x_coor - Origin_Coor_InWorld.x_coor;    //获取当前坐标在起点坐标系下的坐标
	Current_Coor_InOrigin.y_coor = Current_Coor_InWorld.y_coor - Origin_Coor_InWorld.y_coor;
	Current_Coor_InOrigin.angle_coor = Current_Coor_InWorld.angle_coor - Origin_Coor_InWorld.angle_coor;
	Current_Coor_InOrigin.angle_rad = Current_Coor_InWorld.angle_rad - Origin_Coor_InWorld.angle_rad;
	Coordinate_Class_t Target_Coor_InOrigin = Cal_Projection_Coor(Current_Coor_InOrigin);     //计算当前坐标向量在目标坐标向量上的投影

	float current_distance = Cal_Current_Coor_InOrigin(Target_Coor_InOrigin);     //获取目标坐标在起点坐标下的距离
	int Inter_result = Interpolation_Cal_Velocity(current_distance);     //计算插补结果

	Velocity_Class_t Target_Velocity_InOrigin;    //期望坐标坐标系下的速度

	Target_Velocity_InOrigin = Assign_Velocity(Target_Coor_InOrigin, target_velocity);    //将速度分配给各个轴

	//将起点坐标系中的速度旋转至AGV坐标系
	Target_Velocity_InAGV = Velocity_Absolute_To_Relative(Target_Velocity_InOrigin, Target_Velocity_InAGV, Target_Coor_InOrigin);
	//计算期望坐标在世界坐标系下的坐标
	Target_Coor_InWorld.x_coor = Origin_Coor_InWorld.x_coor + Target_Coor_InOrigin.x_coor;
	Target_Coor_InWorld.y_coor = Origin_Coor_InWorld.y_coor + Target_Coor_InOrigin.y_coor;
	Target_Coor_InWorld.angle_coor = Origin_Coor_InWorld.angle_coor + Target_Coor_InOrigin.angle_coor;
	Target_Coor_InWorld.angle_rad = Origin_Coor_InWorld.angle_rad + Target_Coor_InOrigin.angle_rad;

	return Inter_result;
}





