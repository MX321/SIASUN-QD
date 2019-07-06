/*
 * MPI204A.h
 *
 *  Created on: Jun 24, 2019
 *      Author: siasunhebo
 */

#ifndef MPI204A_H_
#define MPI204A_H_

#include <can.h>

void MPI204A_init();
void MPI204A_Analyze_Data(float angle, float angleRate);

#endif /* MPI204A_H_ */
