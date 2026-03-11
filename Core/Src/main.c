/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "motor.h"
#include "bluetooth.h"
#include "ultrasonic.h"



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart1;
extern int drive_mode;

uint32_t last_sensor_time = 0;
uint32_t turn_start_time = 0;
int is_turning = 0; // 0: 주행모드, 1: 90도 회전모드


float L_sum = 0, C_sum = 0, R_sum = 0;
int sample_count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  Motor_Init(&htim3);
  Bluetooth_Init(&huart1);

  Ultrasonic_Init(&htim4);
  Ultrasonic_Start();
  HAL_Delay(800);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint32_t current_time = HAL_GetTick();

	      if (drive_mode == 1)
	      {
	          // [STEP 1] 센서 순차 트리거 (20ms마다 하나씩, 총 60ms 주기)
	          if (current_time - last_sensor_time >= 20)
	          {
	              static int sensor_step = 0;

	              if (sensor_step == 0) Ultrasonic_Trigger(US_LEFT);
	              else if (sensor_step == 1) Ultrasonic_Trigger(US_CENTER);
	              else {
	                  Ultrasonic_Trigger(US_RIGHT);

	                  // 3개 센서 데이터 즉시 수집 (60ms 주기)
	                  float L = Ultrasonic_GetDistance(US_LEFT);
	                  float C = Ultrasonic_GetDistance(US_CENTER);
	                  float R = Ultrasonic_GetDistance(US_RIGHT);

	                  // [수정] 유효 범위 필터링: 너무 가까우면(데드존) 0.0으로 처리하여 벽으로 인식하게 함
	                  if (L <= 2.5f) L = 0.0f; else if (L > 120.0f) L = 100.0f;
	                  if (C <= 2.5f) C = 0.0f; else if (C > 120.0f) C = 100.0f;
	                  if (R <= 2.5f) R = 0.0f; else if (R > 120.0f) R = 100.0f;

	                  // [제어 로직 시작] - 벽 긁기 방지 및 정밀 주행 최적화
	                  static uint32_t turn_finish_time = 0; // 회전이 끝난 시점 기록

	                  if (is_turning)
	                  {
	                      // 회전 탈출 조건: 정면 32cm 확보 시 직진 모드 복귀
	                      if (C > 32.0f || current_time - turn_start_time > 450)
	                      {
	                          Motor_Stop();
	                          is_turning = 0;
	                          turn_finish_time = current_time;
	                      }
	                  }
	                  else
	                  {
	                      // 1. 코너 감지 (26cm로 상향하여 미리 회전 시작)
	                      if (C < 26.0f && (current_time - turn_finish_time > 600))
	                      {
	                          if (L >= R) Motor_Left(); else Motor_Right();
	                          
	                          Motor_SetSpeed(650, 650); // 회전 속도 최적화
	                          turn_start_time = current_time;
	                          is_turning = 1;
	                      }
	                      // 2. 정밀 중앙 유지 주행
	                      else
	                      {
	                          float diff = L - R;
	                          Motor_Forward();

	                          int base_speed = 400; // 정밀도를 위해 400으로 하향
	                          int correction = 0;

	                          // [비선형 회피] 벽에 아주 가까울 때(11cm) 강력하게 조향
	                          if (L < 11.0f) {
	                              correction = -250; // 오른쪽으로 강력하게 꺾음
	                          } else if (R < 11.0f) {
	                              correction = 250;  // 왼쪽으로 강력하게 꺾음
	                          } else {
	                              // 평상시 선형 보정 (감도 18)
	                              correction = (int)(diff * 18);
	                              if (correction > 180) correction = 180;
	                              if (correction < -180) correction = -180;
	                          }

	                          // 최종 속도 적용
	                          Motor_SetSpeed(base_speed - correction, base_speed + correction);
	                      }
	                  }
	              }

	              sensor_step = (sensor_step + 1) % 3;
	              last_sensor_time = current_time;
	          }
	      }
	      else 
	      { 
	          // 수동 모드일 때는 주행 플래그만 초기화하고 모터 제어는 bluetooth.c에 맡김
	          is_turning = 0; 
	          sample_count = 0; 
	          HAL_Delay(100); 
	      }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
