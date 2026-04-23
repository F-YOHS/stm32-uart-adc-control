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
#include "main.h"

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);

uint8_t buffer[1024] = {0,};
uint8_t  data_rx;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit_IT(&huart2, &data_rx, 1);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart2, &data_rx, 1);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	int16_t adc_data = HAL_ADC_GetValue(&hadc1);
	adc_data = adc_data * 100 / 4095;

	char str[20];
	sprintf(str, "adc_data = %i%%\r\n", adc_data);
	HAL_UART_Transmit_IT(&huart2, (uint8_t*)str, strlen(str));
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();

  HAL_StatusTypeDef result;
  int button_state;
  int flag = 0;

  int read_button()
  {
	  button_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
	  if(button_state == 0 && flag == 0) { flag = 1; return 1; }
	  if(button_state == 1 && flag == 1) { flag = 0; return 0; }
	  return 0;
  }

  int data;
  int state = 0;
  int speed;
  int counter = 0;

  result = HAL_UART_Receive_IT(&huart2, &data_rx, 1);

  while (1)
  {
	  data = read_button();
	  HAL_Delay(50);

	  if(result == HAL_OK)
	  {
		  switch(data_rx)
		  {
			  case 48: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0); state = 0; data_rx = 0; break; // 0 LED off
			  case 49: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1); state = 0; data_rx = 0; break; // 1 LED on
			  case 50: state = 1; data_rx = 0; break;                                           // 2 blink fast
			  case 51: state = 2; speed = 10; data_rx = 0; break;                               // 3 blink slow
			  case 52: state = 3; speed = 5;  data_rx = 0; break;                               // 4 blink medium
			  case 53: state = 4; HAL_ADC_Start_IT(&hadc1); break;                              // 5 ADC read
		  }
	  }

	  if(state == 1) HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	  counter++;
	  if((state == 2 || state == 3) && counter == speed)
	  {
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		  counter = 0;
	  }
	  if(counter > 10) counter = 0;

	  if(data != 0)
	  {
		  char string[100] = "";
		  sprintf(string, "button is pressed\n\r");
		  HAL_UART_Transmit(&huart2, (uint8_t*)string, strlen(string), -1);
	  }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) Error_Handler();
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) Error_Handler();
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = s1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(s1_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = s2_Pin|s4_Pin|s3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif