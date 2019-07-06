/*
 * can.c
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */


#include <can.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define errout(_s)	fprintf(stderr, "error class: %s\n", (_s))
#define errcode(_d) fprintf(stderr, "error code: %02x\n", (_d))

static void handle_err_frame(const struct can_frame *fr)
{
	if (fr->can_id & CAN_ERR_TX_TIMEOUT) {
		errout("CAN_ERR_TX_TIMEOUT");
	}
	if (fr->can_id & CAN_ERR_LOSTARB) {
		errout("CAN_ERR_LOSTARB");
		errcode(fr->data[0]);
	}
	if (fr->can_id & CAN_ERR_CRTL) {
		errout("CAN_ERR_CRTL");
		errcode(fr->data[1]);
	}
	if (fr->can_id & CAN_ERR_PROT) {
		errout("CAN_ERR_PROT");
		errcode(fr->data[2]);
		errcode(fr->data[3]);
	}
	if (fr->can_id & CAN_ERR_TRX) {
		errout("CAN_ERR_TRX");
		errcode(fr->data[4]);
	}
	if (fr->can_id & CAN_ERR_ACK) {
		errout("CAN_ERR_ACK");
	}
	if (fr->can_id & CAN_ERR_BUSOFF) {
		errout("CAN_ERR_BUSOFF");
	}
	if (fr->can_id & CAN_ERR_BUSERROR) {
		errout("CAN_ERR_BUSERROR");
	}
	if (fr->can_id & CAN_ERR_RESTARTED) {
		errout("CAN_ERR_RESTARTED");
	}
}
#define myerr(str)	fprintf(stderr, "%s, %s, %d: %s\n", __FILE__, __func__, __LINE__, str)


void print_frame(struct can_frame *fr)
{
	int i;
	printf("%08x\n", fr->can_id & CAN_EFF_MASK);
	//printf("%08x\n", fr->can_id);
	printf("dlc = %d\n", fr->can_dlc);
	printf("data = ");
	for (i = 0; i < fr->can_dlc; i++)
		printf("%02x ", fr->data[i]);
	printf("\n");
}

/*
canbus初始化函数
char *interface：can设备接口名
canBus_t * ：返回的canbus结构体
*/
canBus_t  *CanBusInit(char *interface)
{
    canBus_t *canbus_ptr = (canBus_t *)malloc(sizeof(canBus_t));
    memset(canbus_ptr, 0, sizeof(canBus_t));
    pthread_mutex_init(&canbus_ptr->rdwrLock, NULL);
    struct sockaddr_can addr;
    struct ifreq ifr;
    if((canbus_ptr->canFd = socket(PF_CAN,SOCK_RAW,CAN_RAW))<0)
    {
        printf("socket err!\n");
    }
    strcpy(ifr.ifr_name,interface);
    if(-1 == (ioctl(canbus_ptr->canFd,SIOCGIFINDEX,&ifr)))
    {
        printf("ioctl err!\n");
    }

	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1); //pass the interface to the ifr
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name); //check if the interface really exists
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
		return 0;
	}
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if((bind(canbus_ptr->canFd,(struct sockaddr*)&addr,sizeof(addr))) < 0)
    {
        printf("bind err!\n");
    }

    return canbus_ptr;
}

/*
can设备发送线程
*/

void CanSendThread(TaskAdd_t *arg)
{

	while(1)
	{
		pthread_mutex_lock(&arg->arg.can->rdwrLock);

		write(arg->arg.can->canFd, &arg->arg.can->sendBuffer, sizeof(struct can_frame));

		pthread_mutex_unlock(&arg->arg.can->rdwrLock);

		//memset(&fr, 0, sizeof(struct can_frame));
		printf("send thread done!\n");
		//printf_frame(&arg->can->sendBuffer);



		sleep(1);
	}

}


void CanRecvThread(canBus_t *can)
{
	int nbytes;
	int ret;
	struct timeval tv;
	fd_set rset;



	//tv.tv_sec = 1;
	//tv.tv_usec = 0;
	FD_ZERO(&rset);
	FD_SET(can->canFd, &rset);

	ret = select(can->canFd + 1, &rset, NULL, NULL, NULL);
	if (ret == 0)
	{
		myerr("select time out");
		return;
	}

	nbytes = read(can->canFd, &can->recvBuffer, sizeof(struct can_frame));

	//if (ret < sizeof(can->recvBuffer))
	//{
	//	myerr("read failed");
	//	return;
	//}
	if (can->recvBuffer.can_id & CAN_ERR_FLAG)             /* 出错设备错误 */
	{
		handle_err_frame(&can->recvBuffer);
		myerr("CAN device error");
		return;
	}

	if(nbytes > 0)
	{
		print_frame(&can->recvBuffer);

	}




}



int CanBusTestMode()
{
	//timerThread = TimerRoutineInit();   //初始化timer线程
	canBus_t *CAN0 = CanBusInit("can0");       //init can
	pthread_t canRecvCan0, canSendCan0;              //can0接收线程句柄
    pthread_create(&canRecvCan0, NULL, (void *)CanRecvThread, CAN0);

    //create a job

	TaskAdd_t MASTER_CTRL_ESA_Task;  //创建任务名
    //MASTER_CTRL_ESA_Task.task = CanSendThread;   //任务为发送线程
    MASTER_CTRL_ESA_Task.arg.boId = 0x100;
    MASTER_CTRL_ESA_Task.arg.can = CAN0;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.can_dlc = 8;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.can_id = 0x300;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[0] = 1;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[1] = 2;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[2] = 3;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[3] = 4;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[4] = 5;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[5] = 6;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[6] = 7;
    MASTER_CTRL_ESA_Task.arg.can->sendBuffer.data[7] = 8;

/*
    taskArg_t MASTER_CTRL_ESA_Task;
    MASTER_CTRL_ESA_Task.can->sendBuffer.can_dlc = 8;
    MASTER_CTRL_ESA_Task.can->sendBuffer.can_id = 0x300;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[0] = 1;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[1] = 2;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[2] = 3;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[3] = 4;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[4] = 5;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[5] = 6;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[6] = 7;
    MASTER_CTRL_ESA_Task.can->sendBuffer.data[7] = 8;

    struct can_frame fr;
*/
   // AddTimerTask(&MASTER_CTRL_ESA_Task, 1);
    pthread_create(&canSendCan0, NULL, (void *)CanSendThread, &MASTER_CTRL_ESA_Task);

}
