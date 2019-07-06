/*
 * can.h
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */

#ifndef CAN_H_
#define CAN_H_

#include <linux/can.h>      //sockaddr_can
#include <sys/socket.h>     //PF_CAN SOCK_RAW
#include <net/if.h>         //struct ifreq
#include <sys/ioctl.h>      //SI
#include <pthread.h>
//#include <Timer.h>

typedef struct canBusElem{
    int canFd;                      //can设备句柄
    struct can_frame sendBuffer;    //can设备发送buffer
    struct can_frame recvBuffer;    //can设备接收buffer
    pthread_mutex_t  rdwrLock;      //can设备发送读取互斥锁
}canBus_t;



typedef struct canbuscmd{
    int show;
    int record;
    int loopback;
    char *interface;
}canbuscmd_t;


typedef struct taskArg{
    canBus_t * can;
    unsigned int boId;
}taskArg_t;


typedef struct TaskAdd{
    void(*task)(taskArg_t *);
	//void(*task)(taskArg_t *);
    int canID;
    taskArg_t arg;
}TaskAdd_t;

int CAN_Send_Random(struct can_frame * frameptr,int can_fd);

int SocketCAN_init(char *interface);

canBus_t * CanBusInit(char *interface);

extern int CanBusTestMode();

void PthreadCanbusTest();

void CanbusRecvRecord(canbuscmd_t* cmd);

void CanRecvThread(canBus_t *can);
//void CanSendThread(taskArg_t *arg);
void print_frame(struct can_frame *fr);

#endif /* CAN_H_ */
