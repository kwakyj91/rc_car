/*
 * motor.h
 *
 *  Created on: Feb 24, 2026
 *      Author: kwakyj91
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_


#include "stm32f4xx_hal.h"

void Motor_Init(TIM_HandleTypeDef *htim);
void Motor_SetSpeed(uint16_t left, uint16_t right);

void Motor_Forward(void);
void Motor_Backward(void);
void Motor_Left(void);
void Motor_Right(void);
void Motor_Stop(void);







#endif /* INC_MOTOR_H_ */
