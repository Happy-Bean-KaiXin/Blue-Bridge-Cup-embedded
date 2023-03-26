#include "bsp_key.h"
#include "bsp_lcd.h"
#include "bsp_iic.h"
#include "bsp_led.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

extern uint8_t COM_FACE;
extern uint8_t Lcd_Disp_String[21];
extern uint8_t COMMODx, COMMODy;    /* 商品x，y */
extern uint8_t COMMODpx, COMMODpy;  /* 商品价格x，y 的数据扩大10倍 */
extern uint8_t COMMODrx, COMMODry;    /* 商品库存x，y */

extern uint8_t tx[21];

extern uint8_t ucLED; 
extern uint8_t pwm_flag;
extern uint8_t LD1_flag;

/**
 * @brief Key scan function
 * 
 * @return uint8_t Key_Val 
 *         The key value pressed by the key
 */
uint8_t Key_Sacn(void) {
    static uint8_t Key_Val = 0;

    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == RESET) { 
        Key_Val = 4;  /* B4 is pressed key */
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == RESET) {
        Key_Val = 1;
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == RESET) {
        Key_Val = 2;
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == RESET) {
        Key_Val = 3;
    }
    else {
        Key_Val = 0;  /* There is no key value for key presses */
    }

    return Key_Val;
}

/**
 * @brief Key press handling functions
 * 
 */
void Key_Proc(void) {
    static __IO uint32_t uwTick_Key_Speed;
    uint8_t uc_KeyVal, uc_KeyUp, uc_KeyDown;
    static uint8_t uc_KeyOld;

    if((uwTick - uwTick_Key_Speed) < 50) return;  // Controls how often keystrokes are executed
    uwTick_Key_Speed = uwTick;

    uc_KeyVal = Key_Sacn();
    uc_KeyDown = uc_KeyVal & (uc_KeyOld ^ uc_KeyVal);  // Press the key value
    uc_KeyUp = ~ uc_KeyVal & (uc_KeyOld ^ uc_KeyVal);  // Press the key to lift the key value
    uc_KeyOld = uc_KeyVal;
		
	if(uc_KeyDown) {
		switch(uc_KeyDown) {
			case 4: B4(); break;
			case 3: B3(); break;
			case 2: B2(); break;
			case 1: B1(); break;
		}
	}
}

void B1(void) {
	if(++COM_FACE > 2) 
		COM_FACE = 0;
	LCD_Clear(White);
}

/* 定义商品x */
void B2(void) {
	if(COM_FACE == 0) {
		COMMODx++;
		if(COMMODx > COMMODrx)
			COMMODx = COMMODrx;
	}
	else if(COM_FACE == 1) {
	  COMMODpx += 1;
		if(COMMODpx > 20)  /* 当价格超过2.0时，又回到1.0的价格进行重新调整 */
			COMMODpx = 10;
		EEPROM_write(0x02, COMMODpx);
	}
	else if(COM_FACE == 2) {
		COMMODrx += 1;
		EEPROM_write(0x00, COMMODrx);
	}
}

/* 定义商品y */
void B3(void) {
	switch(COM_FACE) {
		case 0: 
			COMMODy++; 
			if(COMMODy > COMMODry)
				COMMODy = COMMODry;
			break;
		
		case 1: 
			COMMODpy = COMMODpy + 1; 
			if(COMMODpy > 20)
				COMMODpy = 10;
			EEPROM_write(0x03, COMMODpy);
			break;
			
		case 2:
			COMMODry++; 
			EEPROM_write(0x01, COMMODry);
			break;
	}
}

void B4(void) {
	float Z;
	Z = 0.1*(COMMODx*COMMODpx) + 0.1*(COMMODy*COMMODpy);
	
	if(COM_FACE == 0) {
		sprintf((char* )tx, (const char* )"X:%d, Y:%d, Z:%.1f\n", (int)COMMODx, (int)COMMODy, (float)Z);
		HAL_UART_Transmit(&huart1, tx, strlen((const char* )tx), 1000);
		
		ucLED |= LD1; /* 点亮LD1 */
		LD1_flag = 1; /* LD1显示计时 */
		pwm_flag = 1; /* 输出30%的脉冲 计时开始 */
		
		/* 库存减去购买量 */
		COMMODrx = COMMODrx - COMMODx; 
		COMMODry = COMMODry - COMMODy;
				
		/* 在存取一遍库存 */
		EEPROM_write(0x00, COMMODrx);
		HAL_Delay(1);
		EEPROM_write(0x01, COMMODry);
		HAL_Delay(1);
		
		/* 清除购买量 */
		COMMODx = 0; COMMODy = 0;  
	}	
	LCD_Clear(White);
}
