/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bsp_led.h
  * @brief          : Header for bsp_led.c file.
  ******************************************************************************
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_LED_H
#define __BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

#define LD1 0X01
#define LD2 0X02
#define LD3 0X04
#define LD4 0X08
#define LD5 0X10
#define LD6 0X20
#define LD7 0X40
#define LD8 0X80
#define LD_OP 0XFF  // open all LED  apply LED_Disp_OP()
#define LD_CL 0X00  // clse all LED  apply LED_Disp_OP()

void LED_CLOSE(void);
void LED_Disp_OP(unsigned char ucLED);
void LED_Disp_CL(unsigned char ucLED);
void LED_Disp(unsigned char ucLED);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LED_H */
