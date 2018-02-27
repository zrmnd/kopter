#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "stm32f10x.h" 


#define IS_CONNECTED_220V (GPIOA->IDR & GPIO_IDR_IDR6)
#define BAT_VOLTAGE_ON (((float)adcMeasure(5)*17.544)/4095.0)
#define BAT_VOLTAGE_OFF (((float)adcMeasure(5)*15.944)/4095.0)	
#define BAT_VOLTAGE ((IS_POWER_HOLDED || IS_CONNECTED_220V)) ? BAT_VOLTAGE_ON : BAT_VOLTAGE_OFF
#define BAT_IS_NORMAL (BAT_VOLTAGE_ON > 11.0)
//#define BAT_IS_NORMAL (((BAT_VOLTAGE_ON > 11.0) && (IS_POWER_HOLDED || IS_CONNECTED_220V)) || ((BAT_VOLTAGE_OFF > 11.0) && (!IS_POWER_HOLDED && !IS_CONNECTED_220V)))
#define IS_POWER_HOLDED (GPIOB->ODR & GPIO_ODR_ODR4)
#define IS_PWRKEY_PRESSED ((GPIOB->IDR & GPIO_IDR_IDR3))

uint16_t adcMeasure(uint8_t channel);
void initADC(void);
void initModule1CCRChangeTim(void);
void restartModule1CCRChangeTim(void);

void power_init(void);
void power_bullet_ctrl(uint8_t on);
void power_pult_hold(uint8_t on);







 




#endif 
