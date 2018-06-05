#ifndef __DEVICE_CONF_H__
#define __DEVICE_CONF_H__

#include "stm32f0xx_hal.h"

/* CONSTANTS *********************** */
// DEVICE_TYPE
#define UNIVERSAL               1
#define LIDAR                   2
#define INTROS                  3
// etc...

/* DEVICE CONFIGURATION ************ */
#define DEVICE_ID               0x00
#define DEVICE_TYPE             LIDAR

// RCC Macros
#define RCC_GPIOA()             (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define RCC_GPIOB()             (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define RCC_GPIOF()             (RCC->AHBENR |= RCC_AHBENR_GPIOFEN)
#define RCC_TIM2()              (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define RCC_TIM3()              (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define RCC_UART1()             (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define RCC_UART2()             (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define RCC_TIMEOUTER()         (RCC->APB2ENR |= RCC_APB2ENR_TIM17EN)

#if DEVICE_TYPE == UNIVERSAL
#define PPM_TIM       TIM2
#elif DEVICE_TYPE == LIDAR
#define PPM_TIM       TIM3
#endif

#define PPM_TIM_PRESCALER       (48 - 1)
#define PPM_TIM_PERIOD          (20000 - 1)
#define MINIMAL_DUTY            544
// MAXIMUM_DUTY(2400) = 180 * x + MINIMAL_DUTY => ~10.31f
#define DUTY_PARAM              (10.31f)
#define CALC_DUTY(x)            ((uint32_t)(DUTY_PARAM * x) + MINIMAL_DUTY)
#define UART_BAUDRATE           38400

#define PPM1_ANGLE(x)           (PPM_TIM->CCR1 = CALC_DUTY(x))
#define PPM2_ANGLE(x)           (PPM_TIM->CCR2 = CALC_DUTY(x))
#define PPM3_ANGLE(x)           (PPM_TIM->CCR3 = CALC_DUTY(x))
#define PPM4_ANGLE(x)           (PPM_TIM->CCR4 = CALC_DUTY(x))

/* COMMON PART ******************** */
#define USB_POWER_PORT          GPIOA
#define UBS_POWER_PIN           GPIO_PIN_5
#define USB_POWER_INIT(x)       x.Mode = GPIO_MODE_OUTPUT_OD; \
                                x.Pin = UBS_POWER_PIN; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(USB_POWER_PORT, &x);
#define USB_POWER_ON()          HAL_GPIO_WritePin(USB_POWER_PORT, UBS_POWER_PIN, GPIO_PIN_SET)
#define USB_POWER_OFF()         HAL_GPIO_WritePin(USB_POWER_PORT, UBS_POWER_PIN, GPIO_PIN_RESET)

#define POWER_OUTPUT_1_PORT     GPIOA
#define POWER_OUTPUT_1_PIN      GPIO_PIN_6
#define POWER_OUTPUT_1_INIT(x)  x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = POWER_OUTPUT_1_PIN; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(POWER_OUTPUT_1_PORT, &x);
#define POWER_OUTPUT_1_ON()     HAL_GPIO_WritePin(POWER_OUTPUT_1_PORT, POWER_OUTPUT_1_PIN, GPIO_PIN_SET)
#define POWER_OUTPUT_1_OFF()    HAL_GPIO_WritePin(POWER_OUTPUT_1_PORT, POWER_OUTPUT_1_PIN, GPIO_PIN_RESET)

#define POWER_OUTPUT_2_PORT     GPIOA
#define POWER_OUTPUT_2_PIN      GPIO_PIN_7
#define POWER_OUTPUT_2_INIT(x)  x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = POWER_OUTPUT_2_PIN; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(POWER_OUTPUT_2_PORT, &x);
#define POWER_OUTPUT_2_ON()     HAL_GPIO_WritePin(POWER_OUTPUT_2_PORT, POWER_OUTPUT_2_PIN, GPIO_PIN_SET)
#define POWER_OUTPUT_2_OFF()    HAL_GPIO_WritePin(POWER_OUTPUT_2_PORT, POWER_OUTPUT_2_PIN, GPIO_PIN_RESET)

#define DC_12V_PORT             GPIOA
#define DC_12V_PIN              GPIO_PIN_4
#define DC_12V_INIT(x)          x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = DC_12V_PIN; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(DC_12V_PORT, &x);
#define DC_12V_ON()             HAL_GPIO_WritePin(DC_12V_PORT, DC_12V_PIN, GPIO_PIN_SET)
#define DC_12V_OFF()            HAL_GPIO_WritePin(DC_12V_PORT, DC_12V_PIN, GPIO_PIN_RESET)

#define DC_5V_PORT              GPIOB
#define DC_5V_PIN               GPIO_PIN_0
#define DC_5V_INIT(x)           x.Mode = GPIO_MODE_OUTPUT_OD; \
                                x.Pin = DC_5V_PIN; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(DC_5V_PORT, &x);
#define DC_5V_ON()              HAL_GPIO_WritePin(DC_5V_PORT, DC_5V_PIN, GPIO_PIN_SET)
#define DC_5V_OFF()             HAL_GPIO_WritePin(DC_5V_PORT, DC_5V_PIN, GPIO_PIN_RESET)

#define PWR_1                   1
#define PWR_2                   2
#define DC_5V                   3
#define DC_12V                  4

// FIXME: update TERMINAL_x PORTs and PINs
#define TERMINAL_PA15(x)        x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_15; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOA, &x);
#define TERMINAL_PA15_ON()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PA15_OFF()     HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)

#define TERMINAL_PB3(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_3; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOB, &x);
#define TERMINAL_PB3_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PB3_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)
#define TERMINAL_PB4(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_4; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOB, &x);
#define TERMINAL_PB4_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PB4_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)
#define TERMINAL_PB5(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_5; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOB, &x);
#define TERMINAL_PB5_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PB5_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)
#define TERMINAL_PB6(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_6; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOB, &x);
#define TERMINAL_PB6_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PB6_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)
#define TERMINAL_PB7(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_7; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOB, &x);
#define TERMINAL_PB7_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PB7_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)
#define TERMINAL_PB8(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_8; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOB, &x);
#define TERMINAL_PB8_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PB8_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)
#define TERMINAL_PF0(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = GPIO_PIN_0; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(GPIOF, &x);
#define TERMINAL_PF0_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define TERMINAL_PF0_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)

#define POWER_INTROS_PORT       GPIOB
#define POWER_INTROS_PIN        GPIO_PIN_0
#define POWER_INTROS(x)         x.Mode = GPIO_MODE_OUTPUT_PP; \
                                x.Pin = POWER_INTROS_PIN; \
                                x.Pull = GPIO_PULLDOWN; \
                                x.Speed = GPIO_SPEED_FREQ_HIGH; \
                                HAL_GPIO_Init(POWER_INTROS_PORT, &x);
#define POWER_INTROS_ON()       HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_SET)
#define POWER_INTROS_OFF()      HAL_GPIO_WritePin(POWER_INTROS_PORT, POWER_INTROS_PIN, GPIO_PIN_RESET)

// 10 ms
#define TIMEOUTER_TIM           TIM17
#define TIMEOUTER_PRESCALER     (48000 - 1)
#define TIMEIYTER_PERIOD        (10 - 1)

#define TIMEOUTER_ON()          (TIMEOUTER_TIM->CR1 |= TIM_CR1_CEN)
#define TIMEOUTER_OFF()         (TIMEOUTER_TIM->CR1 &= ~TIM_CR1_CEN)

/* DEVICE DEPENDED PART ********** */
#if DEVICE_TYPE == UNIVERSAL
#define UART                    USART1
#define UART_IRQ                USART1_IRQn
#elif DEVICE_TYPE == LIDAR
// definitions for lidar pcb
#elif DEVICE_TYPE == INTROS
#define UART                    USART2
#define UART_IRQ                USART2_IRQn
#endif

#define SEND_UART(x)            while((UART->ISR & USART_ISR_TXE) != USART_ISR_TXE); \
                                UART->TDR = (x & (uint16_t)0x01FF);
#define UART_SET_BAUDRATE(x)    NVIC_DisableIRQ(UART_IRQ); \
                                UART->CR1 &= USART_CR1_UE; \
                                UART->BRR = 480000 / (x / 100); \
                                UART->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE; \
                                UART->ICR |= USART_ICR_TCCF; \
                                NVIC_SetPriority(UART_IRQ, 1); \
                                NVIC_EnableIRQ(UART_IRQ); \

#endif /* __DEVICE_CONF_H__ */
