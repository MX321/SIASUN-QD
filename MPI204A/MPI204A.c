/*
 * MPI204A.c
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */


#include <MPI204A.h>
#include <stdio.h>

canBus_t *CAN0;
//float MPI204A_Angle = 0.0;        //angle output
//float MPI204A_AngleRate = 0.0;    //angle's rate output


void MPI204A_init()
{
	CAN0 = CanBusInit("can0");       //init can

	//pthread_t canRecvCan0;              //can0接收线程句柄

	//pthread_create(&canRecvCan0, NULL, (void *)CanRecvThread, CAN0);
}

void MPI204A_Analyze_Data(float angle, float angleRate)
{
	angle     = (float)((short)(CAN0->recvBuffer.data[0] << 8) | CAN0->recvBuffer.data[1]) / 10.0;
	angleRate = (float)((short)(CAN0->recvBuffer.data[2] << 8) | CAN0->recvBuffer.data[3]) / 10.0;
	//printf("MPI204A_Angle=%f, MPI204A_AngleRate=%f\n", MPI204A_Angle, MPI204A_AngleRate);
	printf("MPI204A_Angle=%f\n", angle);
	printf("MPI204A_AngleRate=%f\n", angleRate);
}
