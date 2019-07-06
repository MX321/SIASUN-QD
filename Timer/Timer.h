/*
 * Timer.h
 *
 *  Created on: Jun 25, 2019
 *      Author: siasunhebo
 */

#ifndef TIMER_H_
#define TIMER_H_

int init_timer(timer_t *timerid, struct sigevent *evp, struct itimerspec *it);
int start_timer(timer_t *timerid, struct itimerspec *it);

void thread_func(void *param);

#endif /* TIMER_H_ */
