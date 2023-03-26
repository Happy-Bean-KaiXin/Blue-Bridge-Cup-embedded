/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_lcd.h"
#include "bsp_iic.h"

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

/* lcd专用变量 */
uint8_t ucLED;  /* 使用此变量将数据进行与或操作来进行开关灯 */
uint8_t LD2_flag;
uint8_t LD1_flag;
uint16_t LD1_cnt_5000ms;

/* pwm专用变量 */
uint8_t pwm_flag = 0;  /* 默认输出脉冲为2000hz，占空比为5% */
uint16_t pwm_cnt_5000ms;

/* EEPROM专用变量 */
int rand_num;

/* lcd专用变量 */
uint8_t Lcd_Disp_String[21];
uint8_t COM_FACE;
uint8_t COMMODx = 0, COMMODy = 0;    /* 商品x，y */
uint8_t COMMODpx = 10, COMMODpy = 10;  /* 商品价格x，y 的价格扩大10倍 显示的时候进行缩小 */
uint8_t COMMODrx = 10, COMMODry = 10;    /* 商品库存x，y */

/* UART专用变量 */
uint8_t tx[21], rx, rx_buf[7];
uint8_t index;  /* 读取数组下标索引 */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LCD_Proc(void);
void LED_Proc(void);
void Uart_Proc(void);
void PWM_Proc(void);
int isRxCplt(void);

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
	uint8_t a;
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
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();  /*  LCD初始化 */
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  LCD_Clear(White);
	LCD_SetBackColor(White);   // 背景颜色
	LCD_SetTextColor(Blue);  // 文本颜色
	HAL_TIM_Base_Start_IT(&htim6);  /* 开启定时器6中断 */
	
	IIC_GPIO_Init();
	
	a = EEPROM_read(0x08);  /* 读取EEPROM的值 */
	if(a=='a') {            /* 判断板子是不是第一次上电 板子不是新板，就读取数据 */
	COMMODrx = EEPROM_read(0x00);
	COMMODry = EEPROM_read(0x01);
	COMMODpx = EEPROM_read(0x02);
	COMMODpy = EEPROM_read(0x03);
	}
	else  {                /* 如果板子是第一次上电 ，将商品赋值为初值 */
		COMMODrx = 10;
		COMMODry = 10;
		COMMODpx = 10;
		COMMODpy = 10;
		EEPROM_write(0x08,'a');
	}
	
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  /* 定时器2通道2 PWM */
	__HAL_TIM_SET_AUTORELOAD(&htim2, 499);  /* 设置频率为2000hz */
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 25);  /* 设置占空比5% = 25/500*/
	
	HAL_UART_Receive_IT(&huart1, &rx, 1);  /* 开启串口中断 */

  while(1)
  {
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    LCD_Proc();
		PWM_Proc();
		LED_Proc();
		Key_Proc();
		Uart_Proc();
		
  }
	
	return 0;
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void LCD_Proc(void) {
	static __IO uint32_t uwTick_LCD_Speed = 0; 
	
  if((uwTick - uwTick_LCD_Speed) < 100 ) return;  /* 100ms进入一次此函数 */
  uwTick_LCD_Speed = uwTick;
	
	if(COM_FACE == 0) {	  /* 购买界面0 */
		sprintf((char* )Lcd_Disp_String, (const char* )"        SHOP");
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    X : %d", COMMODx);
		LCD_DisplayStringLine(Line4, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    Y : %d", COMMODy);
		LCD_DisplayStringLine(Line5, Lcd_Disp_String);
	}
	else if(COM_FACE == 1) {  /* 价格界面1 */
		sprintf((char* )Lcd_Disp_String, (const char* )"        PRICE");
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    X : %.1f", (float)(0.1*COMMODpx));
		LCD_DisplayStringLine(Line4, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    Y : %.1f", (float)(0.1*COMMODpy));
		LCD_DisplayStringLine(Line5, Lcd_Disp_String);
	}
	else if(COM_FACE == 2) {  /* 库存界面2 */
		sprintf((char* )Lcd_Disp_String, (const char* )"        REP");
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    X : %d", COMMODrx);
		LCD_DisplayStringLine(Line4, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    Y : %d", COMMODry);
		LCD_DisplayStringLine(Line5, Lcd_Disp_String);
	}

}

void PWM_Proc(void) {
	if(pwm_flag) {
		__HAL_TIM_SET_AUTORELOAD(&htim2, 499);  /* 设置频率为2000hz */
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 150);  /* 设置占空比30% = 150/500*/
	}
	else {
		__HAL_TIM_SET_AUTORELOAD(&htim2, 499);  /* 设置频率为2000hz */
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 25);  /* 设置占空比5% = 25/500*/
	}
	
}
 
int isRxCplt(void) {
	
	if(index == 0)  /* 未接受到数据 */
		return 0;
	
	if(index != 1)  /* 接收数据长度不为1 */
		return -1;
	
	if(rx_buf[0] == 0x3F)  /* 接受数据为‘？’*/
		return 1;
	else 
		return -1;
	
}

void Uart_Proc(void) {
	static __IO uint32_t uwTick_UART_Speed = 0; 
	
  if((uwTick - uwTick_UART_Speed) < 100 ) return; 
  uwTick_UART_Speed = uwTick;
	
	if(isRxCplt() == 1)  /* 接受数据合法’*/
	{
		sprintf((char* )tx, "X:%.1f,Y:%.1f\n", (float)(0.1*COMMODpx), (float)(0.1*COMMODpy));
		HAL_UART_Transmit(&huart1, tx, strlen((const char* )tx), 1000);
	}
	else if(isRxCplt() == -1) {
		sprintf((char* )tx, "ERROR!\n");
		HAL_UART_Transmit(&huart1, tx, strlen((const char* )tx), 1000);
	}
	
	index = 0;
}

void LED_Proc(void) {
	static __IO uint32_t uwTick_LED_Speed;
	if((uwTick - uwTick_LED_Speed) < 10 )  return;
	uwTick_LED_Speed = uwTick;
	
	if(LD1_flag == 0) {
		ucLED &= 0xFE;
	}
	
	if((COMMODrx == 0)&&(COMMODry == 0)) {
		if(LD2_flag){
			ucLED |= LD2;
		}
		else {
			ucLED &= 0xFD;
		}
	}
  	
	LED_Disp(ucLED);	
}

/* 串口的中断回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
	if(huart->Instance == USART1) {
		rx_buf[index++] = rx;
	}
	
	HAL_UART_Receive_IT(&huart1, &rx, 1);
}

/* 定时器的中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	static uint8_t cnt_100ms;
	if(htim -> Instance == TIM6) {
		if(++cnt_100ms >= 100) {
			cnt_100ms = 0;
			LD2_flag ^= 1;
		}
		
		if(pwm_flag) {
			pwm_cnt_5000ms++;
			if(pwm_cnt_5000ms > 5000) {
				pwm_cnt_5000ms = 0;
				pwm_flag = 0;
			}
		}
		
		if(LD1_flag == 1) {
			LD1_cnt_5000ms++;
			if(LD1_cnt_5000ms >= 5000) {
				LD1_flag = 0;
				LD1_cnt_5000ms = 0;
			}
			
		}
	}
}

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

#ifdef  USE_FULL_ASSERT
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
