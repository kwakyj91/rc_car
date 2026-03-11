/*
 * bluetooth.c
 *
 *  Created on: Feb 24, 2026
 *      Author: kwakyj91
 */


#include "bluetooth.h"
#include "motor.h"

static UART_HandleTypeDef *bt_huart;
static uint8_t rxData;
int drive_mode = 0; // 0: 수동, 1: 자동

void Bluetooth_Init(UART_HandleTypeDef *huart)
{
    bt_huart = huart;
    HAL_UART_Receive_IT(bt_huart, &rxData, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // 1. 모드 전환 로직 (A 누를 때마다 토글)
        if (rxData == 'A' || rxData == 'a')
        {
            if (drive_mode == 0) drive_mode = 1;
            else {
                drive_mode = 0;
                Motor_Stop();
            }
        }
        // 'M'이나 'S'를 누르면 무조건 수동 모드로 복귀
        else if (rxData == 'M' || rxData == 'm' || rxData == 'S' || rxData == 's')
        {
            drive_mode = 0;
            Motor_Stop();
        }

        // 2. 수동 모드일 때만 방향키 작동
        if (drive_mode == 0)
        {
            switch(rxData)
            {
                case 'F': Motor_Forward();  Motor_SetSpeed(800, 800); break;
                case 'B': Motor_Backward(); Motor_SetSpeed(800, 800); break;
                case 'L': Motor_Left();     Motor_SetSpeed(800, 800); break; // 제자리 회전으로 통일
                case 'R': Motor_Right();    Motor_SetSpeed(800, 800); break;
                case 'S': Motor_Stop();     break;
            }
        }

        // 3. 다음 데이터 수신을 위해 인터럽트 재활성화
        HAL_UART_Receive_IT(bt_huart, &rxData, 1);
    }
}
