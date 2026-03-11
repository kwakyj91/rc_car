/*
 * ultrasonic.c
 *
 *  Created on: Feb 26, 2026
 *      Author: kwakyj91
 */

#include "ultrasonic.h"

static TIM_HandleTypeDef *us_htim;
static uint32_t ic_val1[3], ic_val2[3];
static uint8_t  is_first_capture[3] = {0};
static float    distance[3] = {0.0f};

void Ultrasonic_Init(TIM_HandleTypeDef *htim) { us_htim = htim; }

void Ultrasonic_Start(void) {
    HAL_TIM_IC_Start_IT(us_htim, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(us_htim, TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(us_htim, TIM_CHANNEL_3);
}

void Ultrasonic_Trigger(uint8_t sensor) {
    GPIO_TypeDef *port; uint16_t pin; uint32_t chan;
    if (sensor == US_LEFT) { port = GPIOB; pin = GPIO_PIN_2; chan = TIM_CHANNEL_1; }
    else if (sensor == US_CENTER) { port = GPIOB; pin = GPIO_PIN_12; chan = TIM_CHANNEL_2; }
    else { port = GPIOA; pin = GPIO_PIN_11; chan = TIM_CHANNEL_3; }

    is_first_capture[sensor] = 0;
    __HAL_TIM_SET_CAPTUREPOLARITY(us_htim, chan, TIM_INPUTCHANNELPOLARITY_RISING);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    for(volatile int i=0; i<3000; i++);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

float Ultrasonic_GetDistance(uint8_t sensor) { return distance[sensor]; }

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        uint8_t idx; uint32_t ch;
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) { idx = US_LEFT; ch = TIM_CHANNEL_1; }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) { idx = US_CENTER; ch = TIM_CHANNEL_2; }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) { idx = US_RIGHT; ch = TIM_CHANNEL_3; }
        else return;

        if (!is_first_capture[idx]) {
            ic_val1[idx] = HAL_TIM_ReadCapturedValue(htim, ch);
            is_first_capture[idx] = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ch, TIM_INPUTCHANNELPOLARITY_FALLING);
        } else {
            ic_val2[idx] = HAL_TIM_ReadCapturedValue(htim, ch);
            uint32_t diff = (ic_val2[idx] >= ic_val1[idx]) ? (ic_val2[idx]-ic_val1[idx]) : (0xFFFF-ic_val1[idx]+ic_val2[idx]);
            distance[idx] = diff * 0.017f;
            is_first_capture[idx] = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ch, TIM_INPUTCHANNELPOLARITY_RISING);
            if(idx == US_CENTER) HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    }
}
