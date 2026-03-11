/*
 * motor.c
 *
 *  Created on: Feb 24, 2026
 *      Author: kwakyj91
 */

#include "motor.h"

// ===== 방향 핀 정의 (하드웨어 연결 반영) =====
#define RIGHT_MOTOR_IN1_Port GPIOC
#define RIGHT_MOTOR_IN1_Pin  GPIO_PIN_8
#define RIGHT_MOTOR_IN2_Port GPIOC
#define RIGHT_MOTOR_IN2_Pin  GPIO_PIN_6

#define LEFT_MOTOR_IN3_Port  GPIOC
#define LEFT_MOTOR_IN3_Pin   GPIO_PIN_5
#define LEFT_MOTOR_IN4_Port  GPIOA
#define LEFT_MOTOR_IN4_Pin   GPIO_PIN_12

static TIM_HandleTypeDef *motor_htim;

// ================= 초기화 =================
void Motor_Init(TIM_HandleTypeDef *htim)
{
    motor_htim = htim;
    HAL_TIM_PWM_Start(motor_htim, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(motor_htim, TIM_CHANNEL_2); 
    Motor_Stop();
}

// ================= 속도 설정 =================
void Motor_SetSpeed(uint16_t left, uint16_t right)
{
    __HAL_TIM_SET_COMPARE(motor_htim, TIM_CHANNEL_1, right);  // ENA (Right)
    __HAL_TIM_SET_COMPARE(motor_htim, TIM_CHANNEL_2, left);   // ENB (Left)
}

// ================= 전진 (방향만 설정) =================
void Motor_Forward(void)
{
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN1_Port, RIGHT_MOTOR_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN2_Port, RIGHT_MOTOR_IN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN3_Port, LEFT_MOTOR_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN4_Port, LEFT_MOTOR_IN4_Pin, GPIO_PIN_RESET);
}

// ================= 후진 (방향만 설정) =================
void Motor_Backward(void)
{
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN1_Port, RIGHT_MOTOR_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN2_Port, RIGHT_MOTOR_IN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN3_Port, LEFT_MOTOR_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN4_Port, LEFT_MOTOR_IN4_Pin, GPIO_PIN_SET);
}

// ================= 좌회전 =================
void Motor_Left(void)
{
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN1_Port, RIGHT_MOTOR_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN2_Port, RIGHT_MOTOR_IN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN3_Port, LEFT_MOTOR_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN4_Port, LEFT_MOTOR_IN4_Pin, GPIO_PIN_SET);
}

// ================= 우회전 =================
void Motor_Right(void)
{
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN1_Port, RIGHT_MOTOR_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN2_Port, RIGHT_MOTOR_IN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN3_Port, LEFT_MOTOR_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN4_Port, LEFT_MOTOR_IN4_Pin, GPIO_PIN_RESET);
}

// ================= 정지 =================
void Motor_Stop(void)
{
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN1_Port, RIGHT_MOTOR_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_MOTOR_IN2_Port, RIGHT_MOTOR_IN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN3_Port, LEFT_MOTOR_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_MOTOR_IN4_Port, LEFT_MOTOR_IN4_Pin, GPIO_PIN_RESET);
    Motor_SetSpeed(0, 0);
}
