 #include "bsp_led.h" 

void LED_CLOSE(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_SET);  // 将所有灯熄灭
    // 先开锁存器，在关锁存器  PD2
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

/**
 * @brief Control the LED
 * 
 * @note LD1-LD8 corresponds to the 8 bits of the ucLED
 * 
 * @param ucLED Controls the on/off of a certain LED
 * @retval None
 */
void LED_Disp_OP(unsigned char ucLED) {

    // 根据ucLED的数值点亮相应的灯
    HAL_GPIO_WritePin(GPIOC, ucLED << 8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}


void LED_Disp_CL(unsigned char ucLED) {

    // 根据ucLED的数值点亮相应的灯
    HAL_GPIO_WritePin(GPIOC, ucLED << 8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void LED_Disp(unsigned char ucLED) {
	
		LED_CLOSE();
			
    // 根据ucLED的数值点亮相应的灯
    HAL_GPIO_WritePin(GPIOC, ucLED << 8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}



