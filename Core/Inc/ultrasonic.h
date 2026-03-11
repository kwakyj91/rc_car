/*
 * ultrasonic.h
 *
 *  Created on: Feb 26, 2026
 *      Author: kwakyj91
 */

#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_


#include "stm32f4xx_hal.h"

#define US_LEFT     0
#define US_CENTER   1
#define US_RIGHT    2

void Ultrasonic_Init(TIM_HandleTypeDef *htim);
void Ultrasonic_Start(void);
void Ultrasonic_Trigger(uint8_t sensor);
float Ultrasonic_GetDistance(uint8_t sensor);

#endif /* INC_ULTRASONIC_H_ */
