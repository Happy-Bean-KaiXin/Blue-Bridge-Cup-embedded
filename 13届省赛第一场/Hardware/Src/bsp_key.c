#include "bsp_key.h"
#include "bsp_lcd.h"

uint8_t PSD_STA = 0;
char mima[3] = {'@', '@', '@'};  // 初始化密码显示
char right_mima[3] = {'1', '2', '3'};  //初始化正确密码
int error_num;
int flag;
uint8_t right_flag = 0;
uint8_t ld2_YN_flag;

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

    switch(uc_KeyDown) {
        case 4: B4(); break;
        case 3: B1(); break;
        case 2: B2(); break;
        case 1: B3(); break;
    }

}

void B4(void) {
    if(PSD_STA == 0) {
        if((right_mima[0] == mima[0])&&(right_mima[1] == mima[1])&&(right_mima[2] == mima[2])) { /* 密码正确 */
            error_num = 0;
						right_flag = 1;
            PSD_STA = 1;  // 密码正确进入界面二
            LCD_Clear(White);  /* 密码正确进行清屏 */
        }
        else {  /* 密码错误 */
            if(++error_num >= 3) {  /* 密码错误大于3次 */
								ld2_YN_flag = 1;    /* LD2闪烁报警 */ 
            }
						right_flag = 0;
            mima[0] = mima[1] = mima[2] = '@';
        }
    }
}

void B3(void) {
	if(PSD_STA == 0) {
		if(++mima[0] > '9')
            mima[0] = '0';
	}
}

void B2(void) {
	if(PSD_STA == 0) {
		if(++mima[1] > '9')
            mima[1] = '0';
	}
}

void B1(void) {
	if(PSD_STA == 0) {
		if(++mima[2] > '9')
            mima[2] = '0';
	}
}