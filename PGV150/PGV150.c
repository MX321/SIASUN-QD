/*
 * PGV150.c
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */


#include <UART.h>
#include <stdio.h>
#include <PGV150.h>


int fd, fd_r;    //文件描述符
int err, err_r;                           //返回调用函数的状态
char PGV_Send_buff[] = {0xC8, 0x37};
char PGV_Rev_buff[];
static int len_s = 0;
static int len_r = 0;


void PGV_init()
{
	fd = open_com_dev(DEV_UART);
	do{
		err = init_com_dev(fd,115200,8,'e',1);
		printf("Set Port Exactly!\n");
		} while(0 == err || 0 == fd);
	fd_r = open_com_dev(DEV_UART);

	err_r = init_com_dev(fd,115200,8,'e',1);
}

void PGV_Send()
{
	len_s = send_com_dev(fd,PGV_Send_buff,2);
	if(len_s > 0)
	{
		//printf(" %d send data successful\n", len_s);
		//return len_s;
	}
	else
	{
		printf("send data failed!\n");
		//return -1;
	}

}


void PGV_Rev()
{
	len_r = rcv_com_dev(fd_r, PGV_Rev_buff,21);
	if(len_r > 0)
	{
		PGV_Rev_buff[len_r] = '\0';

		/*
		printf("receive data is %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"
				"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",PGV_Rev_buff[0], PGV_Rev_buff[1], PGV_Rev_buff[2], PGV_Rev_buff[3],
				PGV_Rev_buff[4], PGV_Rev_buff[5], PGV_Rev_buff[6], PGV_Rev_buff[7], PGV_Rev_buff[8], PGV_Rev_buff[9], PGV_Rev_buff[10],PGV_Rev_buff[11],
				PGV_Rev_buff[12], PGV_Rev_buff[13], PGV_Rev_buff[14], PGV_Rev_buff[15], PGV_Rev_buff[16], PGV_Rev_buff[17], PGV_Rev_buff[18], PGV_Rev_buff[19],
				PGV_Rev_buff[20], PGV_Rev_buff[21]);
		printf("len = %d\n",len_r);
*/
		//return len_r;
	}
	else
	{
		printf("cannot receive data\n");
		//return -1;
	}
}

int PGV_AnalyzeData()
{
	unsigned int xor_temp = 0;
	int rx_xor_flag = 0;
	int i;
	int data_temp;

	for (i = 0; i < 20; i++)
	{
		xor_temp ^= PGV_Rev_buff[i]; //前20个字节的异或校验
	}

	if ((xor_temp == PGV_Rev_buff[20]) && (PGV_Rev_buff[2] != 0x02))
	{
		result = 1;

	}
	else
	{
		result = 0;
		return 0;
	}

/*
	rx_xor_flag = (xor_temp == PGV_Rev_buff[20]); //检验校验码
    if (rx_xor_flag == 0)
    {
    	return 0;

    }
*/
	//校验通过
	warn_flag = (int)(PGV_Rev_buff[0] & _BV(2)); //提取错误标志
	warn = PGV_Rev_buff[18] << 7 | PGV_Rev_buff[19];	//错误代码

	data_temp = (PGV_Rev_buff[2] & 0x07) * 0x80 * 0x4000 + PGV_Rev_buff[3] * 0x4000 + PGV_Rev_buff[4] * 0x80 + PGV_Rev_buff[5];
	if (data_temp > 0x800000)
		x_temp = (0x1000000 - data_temp) * -1;
	else
		x_temp = data_temp;

	data_temp = PGV_Rev_buff[6] * 0x80 + PGV_Rev_buff[7];

	if (data_temp > 0x2000)
		y_temp = (0x4000 - data_temp) * -1;
	else
		y_temp = data_temp;


	data_temp = PGV_Rev_buff[10] * 0x80 + PGV_Rev_buff[11];
	if (data_temp > 0x2000)
		angle_temp = (0x4000 - data_temp) * -1;
	else
		angle_temp = data_temp;

	data_temp = PGV_Rev_buff[14] * 0x80 * 0x4000 + PGV_Rev_buff[15] * 0x4000 + PGV_Rev_buff[16] * 0x80 + PGV_Rev_buff[17];
	tag_control_num = data_temp;

	x_deviation     = x_temp / 10.0;
	y_deviation     = y_temp / 10.0;
	angle_deviation = angle_temp;
	//printf("x_deviation=%f, y_deviation=%f, angle_deviation=%f\n", x_deviation, y_deviation, angle_deviation);


	return 1;
}

void PGV_Cal_Coor()
{
	PGV150_coor.x_coor = (tag_control_num % 5) * 100.0 + x_deviation;
	PGV150_coor.y_coor = (tag_control_num / 5) * 100.0 + y_deviation;
	if ((angle_deviation > 180.0) && (angle_deviation < 360.0))
	{
		PGV150_coor.angle_coor = 360.0 - angle_deviation;
	}
	else if (0 < angle_deviation <= 180.0)
	{
		PGV150_coor.angle_coor = - angle_deviation;
	}
	data_ok = 1;

}




