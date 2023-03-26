/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bsp_key.h
  * @brief          : Header for bsp_key.c file.
  ******************************************************************************
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Function ------------------------------------------------------------------*/
uint8_t Key_Sacn(void);
void Key_Proc(void);

void B1(void);
void B2(void);
void B3(void);
void B4(void);
	
#ifdef __cplusplus
}
#endif

#endif /* __BSP_KEY_H */
