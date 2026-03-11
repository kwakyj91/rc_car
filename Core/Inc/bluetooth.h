/*
 * bluetooth.h
 *
 *  Created on: Feb 24, 2026
 *      Author: kwakyj91
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "stm32f4xx_hal.h"

extern int drive_mode;
void Bluetooth_Init(UART_HandleTypeDef *huart);

#endif /* INC_BLUETOOTH_H_ */
