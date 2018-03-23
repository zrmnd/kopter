#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "stm32f10x.h" 






void initModule1GPIO(void);
void initModule1ADC(void);
void initModule1CCRChangeTim(void);
void restartModule1CCRChangeTim(void);

void power_init(void);
void power_odroid_ctrl(uint8_t on);
void power_bullet_ctrl(uint8_t on);
void power_rfmodule_ctrl(uint8_t on);
void power_periph_ctrl(uint8_t on);








 




#endif 
