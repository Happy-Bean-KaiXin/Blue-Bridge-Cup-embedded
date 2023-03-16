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

/* ledר�ñ��� */


/* lcdר�ñ��� */
int freq = 1000;
uint8_t D_Pwm = 50;
uint8_t pwm_flag = 0;   /* 0Ϊ���������1Ϊ������� */
uint8_t Lcd_Disp_String[21];
uint8_t lcd_flag_show;

/* ����ר�ñ��� */
uint8_t rx, rx_buf[7];
uint8_t dex;  /* �����޸��������� */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LCD_Proc(void);
void LED_Proc(void);
int isRxCplt(void);
void Uart_Proc(void);

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
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();  /*  LCD��ʼ�� */
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  LCD_Clear(White);
	LCD_SetBackColor(White);   // ������ɫ
	LCD_SetTextColor(Blue);    // �ı���ɫ
	HAL_TIM_Base_Start_IT(&htim6);  /* ������ʱ��6(1ms)�жϿ��� */
	HAL_UART_Receive_IT(&huart1, &rx, 1);  /* ���첽����1ͨ�� */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	__HAL_TIM_SET_AUTORELOAD(&htim2, 999);  /* ����Ƶ��1Khz */ 
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500); /* ����ռ�ձ�Ϊ50% = 500/1000 */ 
	
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		LED_Proc();
    LCD_Proc();
		Key_Proc();
		Uart_Proc();
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
	
	if(uwTick - uwTick_LCD_Speed < 100) return;  /* ����ˢ��Ƶ�� */
	uwTick_LCD_Speed = uwTick;
	
	if(lcd_flag_show == 1) {  /* ��STA�����л�PSD���� ��Ҫ����һ�� */
		lcd_flag_show = 0;
		LCD_Clear(White);
	}
	
	/* Ĭ�����������ռ�ձ�Ϊ50% */ 
	/* ��LED_Proc()��ʵ��pwm_flag�Ŀ��� */
	if(pwm_flag == 0) {  /* ���������ռ�ձ�Ϊ50% */ 
		__HAL_TIM_SET_AUTORELOAD(&htim2, 999);  /* ����Ƶ��1Khz */ 
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500); /* ����ռ�ձ�Ϊ50% = 500/1000 */ 
	}
	else if(pwm_flag == 1) {  /* ������ȷ������壬ռ�ձ�Ϊ50% */ 
		__HAL_TIM_SET_AUTORELOAD(&htim2, 499);  /* ����Ƶ��2Khz = 80Mhz/[(AUTORELOAD+1) + 80] ����80Ϊcubemx���� */ 
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 50); /* ����ռ�ձ�Ϊ10% = 50/500*/ 			
	}
	
	if(PSD_STA == 0) {  /* ��PSD���� */
		sprintf((char* )Lcd_Disp_String, (const char* )"       P S D       ");
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);
		
		sprintf((char* )Lcd_Disp_String, (const char* )"    B1 : %c", mima[0]);
		LCD_DisplayStringLine(Line4, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    B2 : %c", mima[1]);
		LCD_DisplayStringLine(Line5, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    B3 : %c", mima[2]);
		LCD_DisplayStringLine(Line6, Lcd_Disp_String);
		
	}
	else if(PSD_STA == 1) {  /* ��STA���� */
		memset(Lcd_Disp_String, '\0', sizeof(Lcd_Disp_String));
		sprintf((char* )Lcd_Disp_String, (const char* )"       S T A       ");
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);
		
		if(pwm_flag == 1) { freq = 2000; D_Pwm = 10; } /* ������ȷ��������壬ռ�ձ�Ϊ10% */ 
		else { freq = 1000; D_Pwm = 50; }  /* 5s�����������ռ�ձ�Ϊ50% */ 

		sprintf((char* )Lcd_Disp_String, (const char* )"    F : %04dHz", freq);
		LCD_DisplayStringLine(Line4, Lcd_Disp_String);
		sprintf((char* )Lcd_Disp_String, (const char* )"    D : %02d%%", D_Pwm);
		LCD_DisplayStringLine(Line5, Lcd_Disp_String);
	}
	

}

void LED_Proc(void) {
    static __IO uint32_t uwTick_LED_Speed;
    if((uwTick - uwTick_LED_Speed) < 100)  return;
		uwTick_LED_Speed = uwTick;
	
		LED_CLOSE();  // �ص�
	
		if(right_flag) {  /* ����������ȷ */
			LED_Disp_OP(LD1);
			pwm_flag = 1;  /* ������� */
		}
		else {
			LED_Disp_CL(LD1);
			pwm_flag = 0;  /* ������� */
		}

		if(ld2_YN_flag) {
			if(flag)
				LED_Disp_OP(LD2);
			else 
				LED_Disp_CL(LD2);
		}
		else LED_Disp_CL(LD2);
}

int isRxCplt(void) {
	uint8_t i;
	
	if(dex == 0) /* û�н��ܵ����� */
		return 0;
	else if(dex != 7) { /* ���ղ���7λ���� */ 
		return -1;
	}
	if(rx_buf[3] == '-') {  /* ��������������ȷ */
		for(i = 0; i < 3; i++) {
			if(rx_buf[i] < '0' || rx_buf[i] > '9')  /* ǰ��λ���պϷ����ش��� */ 
				return -1;
		}
		for(i = 4; i < 7; i++) {
			if(rx_buf[i] < '0' || rx_buf[i] > '9')  /* ����λ���պϷ����ش��� */ 
				return -1;
		}
		return 1;
	}
	else return -1; /* ���յ����������ݲ�Ϊ '-' ���ش��� */ 
	
	return 0;
}

void Uart_Proc(void) {  
	static __IO uint32_t uwTick_UART_Speed;
	uint8_t tx[21];
	
	if((uwTick - uwTick_UART_Speed) < 100)  return;
	uwTick_UART_Speed = uwTick;
	
	if(isRxCplt() == 1) {
		if((rx_buf[0] == right_mima[0])&&(rx_buf[1] == right_mima[1])&&(rx_buf[2] == right_mima[2])) { /* ����ǰ��λ������ԭʼ������ͬ����������ȷ */
			/* �޸����� */
			right_mima[0] = rx_buf[4];
			right_mima[1] = rx_buf[5];
			right_mima[2] = rx_buf[6];
			
			sprintf((char* )tx, "OK\r\n");
			HAL_UART_Transmit(&huart1, tx, strlen((const char* )tx), 50);/* ����λ������OK��ʾ�޸ĳɹ� */
		}
		else {  /* ����ԭʼ������� */
			sprintf((char* )tx, "ERROR\r\n");
			HAL_UART_Transmit(&huart1, tx, strlen((const char* )tx), 50);/* ����λ�����ʹ��� */
		}
	}
	else if(isRxCplt() == -1){  /* �������ݲ��Ϸ� */
		sprintf((char* )tx, "ERROR\r\n");
		HAL_UART_Transmit(&huart1, tx, strlen((const char* )tx), 50);/* ����λ�����ʹ��� */
	}
	dex = 0;  /* ������ڽ����������� */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {  /* ��ʱ���жϻص����� */
	static uint16_t u16_ms = 0;
	static uint16_t u16_100ms = 0;
	static uint16_t u16_5000ms = 0;
	
	if(htim->Instance == TIM6) {
		
		if(right_flag) { /* ����������ȷ��ʼ��ʱ */
			u16_ms++;
			if(u16_ms == 5000) {
				right_flag = 0;
				PSD_STA = 0;   /* �лؽ��� */
				mima[0] = mima[1] = mima[2] = '@';
				lcd_flag_show = 1;  /* LCD���� */
				u16_ms = 0;
			}
		}
		
		if(ld2_YN_flag) {
			u16_5000ms++;
			if(u16_5000ms == 5000) {
				ld2_YN_flag = 0;
				u16_5000ms = 0;
			}
		}
		
		if(++u16_100ms == 100) {
			flag ^= 1;
			u16_100ms = 0;
		}
	}
	HAL_TIM_Base_Start_IT(&htim6);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {  /* �����жϻص����� */
	if(huart -> Instance == USART1)  /* �жϴ��ں� */
		rx_buf[dex++] = rx;            /* ������һλһλ���뻺������  �������ݺ�������1 */
	HAL_UART_Receive_IT(&huart1, &rx, 1);  /* ���¿��������ж� */
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
