/*
 * UART.h
 *
 *  Created on: Jun 26, 2019
 *      Author: siasunhebo
 */

#ifndef UART_H_
#define UART_H_

#define DEV_UART "/dev/ttymxc3"


int open_com_dev(char *dev_name);   //打开串口并返回串口设备文件描述
int init_com_dev(int fd, int nspeed, int nbits, char parity, int nstop);   //串口初始化

#endif /* UART_H_ */
