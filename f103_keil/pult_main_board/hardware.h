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

#define KEYDATA_NOT_CHANGED(x, y) (x[0] == y[0] && x[1] == y[1] && x[2] == y[2] && x[3] == y[3] && x[4] == y[4])
#define COPY_TO_OLD(x, y) y[0] = x[0]; y[1] = x[1]; y[2] = x[2]; y[3] = x[3]; y[4] = x[3]
#define KEY_STOP 3
#define KEY_FWD 4
#define KEY_REV 2
#define KEY_JOY_UP 1
#define KEY_JOY_DOWN 0

uint16_t adcMeasure(uint8_t channel);
void initADC(void);
void initModule1CCRChangeTim(void);
void restartModule1CCRChangeTim(void);

void power_init(void);
void power_bullet_ctrl(uint8_t on);
void power_pult_hold(uint8_t on);


void keys_init(void);
void keys_read(uint8_t *data);



// определяют состояние конечного автомата (к чему относится новый прибывший байт) 
//------------------------------------------------------------------------------
#define STATE_IDLE 0x00                                                         // готовы принять команду 
#define STATE_CMD_foreign 0x01                                                  // принимаем команду, но она не наша. Проверим потом - широковещательная или нет
#define STATE_CMD_preambule 0x02                                                // 
#define STATE_CMD_cmdtype 0x03
#define STATE_CMD_crc 0x04
#define STATE_CMD_body 0x05
#define STATE_CMD_responsewaiting 0xFF
#define STATE_CMD_broadcast 0xFE

uint8_t device485SendDataReceiveAnswer(char devClass, char devId, char cmd, uint8_t **data, uint8_t txLen, uint8_t rxLen);
uint8_t pultIOcontrollerRead(char controllerId, uint8_t *data);
uint8_t pultIOcontrollerWrite(char controllerId, uint8_t *data);

uint8_t calculateCRC(uint8_t *data, uint16_t len);


 




#endif 
