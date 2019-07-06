/*
 * UART.c
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */



#include <fcntl.h>  //文件控制定义
#include <stdio.h>  //标准输入输出定义
#include <unistd.h>  //Unix标准函数定义
#include <errno.h>   //错误定义
#include <termios.h>  //POSIX终端控制定义
#include <sys/ioctl.h>  //ioctl定义
#include <string.h>     //字符串定义
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <UART.h>

//时间戳
long long getSystemTime()
{
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

long long start;
long long end;

//定义互斥量
pthread_mutex_t mutex;
int fd_gpio;

struct termios newtio, oldtio;
typedef struct {
	int pin_idx;
	int pin_dir;
	int pin_sta;
} davinci_gio_arg;

typedef enum {
	AT91PIO_DIR_OUT = 0,
	AT91PIO_DIR_IN
} davinci_gio_dir;

davinci_gio_arg arg;

#define DEV_PIO_LED "/dev/pio"
// 需要手动添加设备号 mknod /dev/pio c 203 0

#define PIO_NUM 47
// 47pin 为控制输入输出方向引脚


// /dev/ttyS1 为串口设备

#define IOCTL_PIO_SETDIR 1    //set gpio direct
#define IOCTL_PIO_GETDIR 2    //get gpio direct
#define IOCTL_PIO_SETSTA 3    //set gpio status
#define IOCTL_PIO_GETSTA 4    //get gpio status

//保存信息
int log_init(const char *strFilename)
{
	int fdLog = -1;

	fdLog = open(strFilename, O_CREAT|O_TRUNC);
	if (-1 == fdLog)
	{

	}
	close(fdLog);
}


int log_out(const char *strFilename, const char *szlog)
{
	int fdLog = -1;
	fdLog = open(strFilename, O_CREAT|O_WRONLY|O_APPEND);

	if (-1 == fdLog)
	{
		printf("LOG (%s) open error!", strFilename);
		return -1;
	}

	write(fdLog, szlog, strlen(szlog));
	close(fdLog);
	return 0;
}

//配置串口
/*
  参数说明：fd 设备文件描述符，nspeed 波特率，nbits 数据位数（7位或8位），
         parity 奇偶校验位（'n'或'N'为无校验位，'o'或'O'为偶校验，'e'或'E'奇校验），
		 nstop 停止位（1位或2位）
  成功返回1，失败返回-1。
*/

int set_com_opt(int fd, int nspeed, int nbits, char parity, int nstop)
{
	int i;
	int status;
	int speed_arr[] = {B115200, B19200, B9600, B4800, B2400, B1200, B300};
	int name_arr[]  = {115200,  19200,  9600,  4800,  2400,  1200,  300};

	struct termios options;


	char szTmp[128];
	//打印配置信息
	sprintf(szTmp, "set_com_opt - speed:%d, bits:%d, parity:%char, stop:%d\n",
			nspeed, nbits, parity, nstop);

	log_out("./485.log", szTmp);
	//保存并测试现在有串口参数设置，在这里如果串口号出错，会有相关的出错信息
	if ( tcgetattr(fd, &options) != 0)
	{
		sprintf(szTmp, "SetupSerial 1");

		log_out("./485.log", szTmp);
		perror("SetupSerial 1");
		return -1;
	}

	//设置串口输入波特率和输出波特率
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
	{
		if  (nspeed == name_arr[i])
	    {
			cfsetispeed(&options, speed_arr[i]);
			cfsetospeed(&options, speed_arr[i]);
	    }
	}

	//修改控制模式，保证程序不会占用串口
	options.c_cflag |= CLOCAL;
	//修改控制模式，使得能够从串口中读取输入数据
	options.c_cflag |= CREAD;



	options.c_cflag &= ~(CSIZE);          //屏蔽其他标志位

	//设置数据位
	switch (nbits)
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		perror("Unsupported date bit!\n");
		return -1;
	}

	//设置校验位
	switch(parity)
	{
	case 'n':
	case 'N':     //无奇偶校验
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
		break;
	case 'o':
	case 'O':     //设置为奇校验
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'e':
	case 'E':
		options.c_iflag |= ( INPCK |ISTRIP );
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		break;
	default:
		perror("unsupported parity\n");
		return -1;
	}

	//设置停止位
	switch (nstop)
	{
	case 1:
		options.c_cflag &= ~CSTOPB; break;
	case 2:
		options.c_cflag |= CSTOPB; break;
	default:
		fprintf(stderr,"Unsupported stop bits\n");
		return -1;
	}

	//修改输出模式，原始数据输出
	options.c_oflag &= ~OPOST;
	//输入模式
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的

	options.c_cflag &= ~CRTSCTS;


	//设置等待时间和最小接收字符

	//VTIME=0，VMIN=0，不管能否读取到数据，read都会立即返回。
	options.c_cc[VTIME] = 1;
	options.c_cc[VMIN] = 1;


	//设置数据流控制
	options.c_lflag &= ~(IXON|IXOFF|IXANY);

	//如果发生数据溢出，接收数据，但是不再读取
	tcflush(fd, TCIFLUSH);

	//激活配置 (将修改后的termios数据设置到串口中）

	if ( tcsetattr(fd, TCSANOW, &options) != 0)
	{
		sprintf(szTmp, "serial set error!\n");
		log_out("./485.log", szTmp);
		perror("serial set error!");
		return -1;
	}

	log_out("./485.log", "serial set ok!\n");
	return 1;



}

//串口初始化
/*
  参数说明：fd 设备文件描述符，nspeed 波特率，nbits 数据位数（7位或8位），
         parity 奇偶校验位（'n'或'N'为无校验位，'o'或'O'为偶校验，'e'或'E'奇校验），
		 nstop 停止位（1位或2位）
  成功返回1，失败返回-1。
*/
int init_com_dev(int fd, int nspeed, int nbits, char parity, int nstop)
{
	int err;

	if (set_com_opt(fd, nspeed, nbits, parity, nstop) == -1)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}


//串口接收
/*
 *
 *  参数说明：fd 设备文件描述符，rcv_buf 接收串口中数据存入rcv_buf缓冲区中，
            data_len 一帧数据的长度
  成功返回1，失败返回-1。
 *
 *
 */
int rcv_com_dev(int fd, char *rcv_buf,int data_len)
{
	int len,fs_sel;
	fd_set fs_read;

	struct timeval time;

	FD_ZERO(&fs_read);
	FD_SET(fd,&fs_read);

	time.tv_sec = 10;
	time.tv_usec = 0;

	//使用select实现串口的多路通信
	fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
	if(fs_sel >= 0)
	{
		if (FD_ISSET(fd,&fs_read))
		{
			len = read(fd,rcv_buf,data_len);
			//printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);
			return len;
		}
		else
			return -1;
	}
	else
	{
		printf("Sorry,I am wrong!");
		return -1;
	}
}


//串口发送
/*
 *
 *  参数说明：fd 设备文件描述符，send_buf 存放串口发送数据
            data_len 一帧数据的长度
  成功返回1，失败返回-1。
 *
 *
 */
int send_com_dev(int fd, char *send_buf,int data_len)
{
	int len = 0;

	len = write(fd, send_buf, data_len);
	if (len == data_len )
	{
		return len;
	}
	else
	{
		tcflush(fd, TCOFLUSH);
		return -1;
	}

}


//打开串口并返回串口设备文件描述
int open_com_dev(char *dev_name)
{
	int fd;
	char szTmp[128];
	log_init("./485.log");
	fd = open(dev_name, O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd == -1)
	{
		perror("open\n");
		sprintf(szTmp, "Can't open Serial %s Port!\n", dev_name);
		log_out("./485.log", szTmp);
		return -1;
	}
	sprintf(szTmp, "Open Serial %s Port ok!\n", dev_name);
	log_out("./485.log", szTmp);

	//判断串口是否为阻塞状态
	if (fcntl(fd, F_SETFL, 0) < 0)
	{
		printf("fcntl failed!\n");
		return -1;
	}
	return fd;
}

