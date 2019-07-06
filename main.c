/*
 * main.c
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */


#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <can.h>
#include <MPI204A.h>
#include <time.h>
#include <signal.h>
#include <Timer.h>
#include <PGV150.h>
#include <common.h>
#include <Movement.h>

extern canBus_t *CAN0;
struct Coordinate_Class AGV_Current_Coor_InWorld;
struct Coordinate_Class Virtual_AGV_Current_Coor_InWorld; //虚拟的AGV坐标用于路径跟随
Velocity_Class_t Virtual_AGV_Current_Velocity_InWorld;    //虚拟的AGV速度，用于运动控制

struct Velocity_Class AGV_Current_Velocity_By_Encoder;    //PLC上传由编码器得到速度
struct Velocity_Class AGV_Current_Velocity_InAGV;         //AGV当前速度
extern Interpolation_State_Enum_m Interpolation_State;


Coordinate_Class_t Destiantion_Coor_Inworld;     //目标位置


float MPI204A_Angle = 0.0;   //IMU角度值
float MPI204A_AngleRate = 0.0;    //IMU角速度
int virtual_agv_coor_init_flag = 0;   //1为地标更新
struct Interpolation_Parameter_t Interpolation_Parameter_m;

void Prase_Sensor_Data()
{
	PGV_Send();           //串口发送
	PGV_Rev();            //串口接受
	PGV_AnalyzeData();    //数据解算
	CanRecvThread(CAN0);  //初始化can0通道
	MPI204A_Analyze_Data(MPI204A_Angle, MPI204A_AngleRate);  //解析imu角度及角速度
	//PGV解算
	if (PGV_AnalyzeData() == 1)
	{
		PGV_Cal_Coor();
		if (!virtual_agv_coor_init_flag)
		{
			virtual_agv_coor_init_flag = 1;
			Virtual_AGV_Current_Coor_InWorld = PGV150_coor;
		}
	}
}

void Location_AGV()
{
	AGV_Current_Velocity_InAGV.angular_velocity_angle = 0.84 * MPI204A_AngleRate + 0.16 * AGV_Current_Velocity_By_Encoder.angular_velocity_angle;  //角速度加权融合
	AGV_Current_Velocity_InAGV.velocity_x = AGV_Current_Velocity_By_Encoder.velocity_x;
	AGV_Current_Velocity_InAGV.velocity_y = 0;


	if (data_ok == 1)     //相机读取到数据
	{
		data_ok = 0;
		AGV_Current_Coor_InWorld = PGV150_coor;   //更新全局坐标
        printf("PGV150_coor.x = %f, PGV150_coor.y = %f, PGV150_coor.angle = %f\n", PGV150_coor.x_coor, PGV150_coor.y_coor, PGV150_coor.angle_coor);

	}
	else
	{
		AGV_Current_Coor_InWorld = Odom_Calib();    //无二维码部分进行里程推算
	}
}

enum Interpolation_State_Enum Interpolation_State;

int Run_Movement_Class(enum Interpolation_State_Enum State, Coordinate_Class_t Current_Coor)
{
	struct Coordinate_Class Coor = virtual_agv_coor_init_flag ? Virtual_AGV_Current_Coor_InWorld : Current_Coor;
    switch (State)
    {
    case No_Interpolation:  //未插补
    	if (Movement_Init(Coor, Interpolation_Parameter_m) == 1)
    	{
    		State = IS_Interpolating;    //IS_Interpolating
    	}
    	else
    	{
    		State = IS_Interpolated;   //IS_Interpolated
    		return 1;    //插补失败，要移动距离小于阈值
    	}
    	break;
    case IS_Interpolating:   //插补中
    	if (!Movement_Cal_Velocity())    //插补完成
    	{
    		State = IS_Interpolated;
    		Virtual_AGV_Current_Coor_InWorld = Destiantion_Coor_Inworld;
    		Virtual_AGV_Current_Velocity_InWorld = Target_Velocity_InAGV;

    	}
    	else   //还在插补，获取虚拟的坐标和速度
    	{
    		Virtual_AGV_Current_Coor_InWorld = Target_Coor_InWorld;
    		Virtual_AGV_Current_Velocity_InWorld = Target_Velocity_InAGV;
    	}
    	break;
    default:
    	break;


    }
    return 0;

}













int main ()
{
	MPI204A_init();      //IMU初始化
	//pthread_t thread;
	PGV_init();          //相机初始化


	/*
	int a = 10;

	timer_t timerid = 0;
	int ret;
    struct sigevent evp;
    struct itimerspec it;
    ret = pthread_create(&thread, NULL, thread_func, &a);

    ret = init_timer(&timerid, &evp, &it);
    if (ret < 0)
    {
        printf("init_timer failed\n");
        return -1;
    }



    sleep(1);
    start_timer(&timerid, &it);
    sleep(2);
    pause();
    exit(0);
    */


	while(1)
	{

		Prase_Sensor_Data();    //传感器处理
		Location_AGV();         //AGV定位
		Run_Movement_Class(Interpolation_State, AGV_Current_Coor_InWorld);

	}

	return 0;
}
