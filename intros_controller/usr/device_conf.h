#ifndef __DEVICE_CONF_H__
#define __DEVICE_CONF_H__

#include "stm32f0xx_hal.h"

/* CONSTANTS *********************** */

#define INTROS                  3
// etc...

/* DEVICE CONFIGURATION ************ */
#define DEVICE_ID               0x00
#define DEVICE_TYPE             INTROS

// RCC Macros
#define RCC_GPIOA()             (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define RCC_GPIOB()             (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)



#define POWER_INTROS_PORT       GPIOB
#define POWER_INTROS_PIN        GPIO_PIN_0
#define POWER_INTROS(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = POWER_INTROS_PIN; \
                                x.Pull = GPIO_NOPULL; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(POWER_INTROS_PORT, &x);
#define POWER_INTROS_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define POWER_INTROS_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)

#endif /* __DEVICE_CONF_H__ */
