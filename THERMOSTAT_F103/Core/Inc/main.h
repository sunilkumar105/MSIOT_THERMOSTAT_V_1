/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_HEALTH_PIN_Pin GPIO_PIN_13
#define LED_HEALTH_PIN_GPIO_Port GPIOC
#define NRF_IRQ_Pin GPIO_PIN_10
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_CSN_Pin GPIO_PIN_8
#define NRF_CSN_GPIO_Port GPIOA
#define NRF_CE_Pin GPIO_PIN_9
#define NRF_CE_GPIO_Port GPIOA
#define RELAY_COMPRESSOR_Pin GPIO_PIN_15
#define RELAY_COMPRESSOR_GPIO_Port GPIOA
#define RELAY_FAN_3_Pin GPIO_PIN_3
#define RELAY_FAN_3_GPIO_Port GPIOB
#define RELAY_FAN_2_Pin GPIO_PIN_4
#define RELAY_FAN_2_GPIO_Port GPIOB
#define RELAY_FAN_1_Pin GPIO_PIN_5
#define RELAY_FAN_1_GPIO_Port GPIOB
#define AHT_SCL_Pin GPIO_PIN_8
#define AHT_SCL_GPIO_Port GPIOB
#define AHT_SDA_Pin GPIO_PIN_9
#define AHT_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
