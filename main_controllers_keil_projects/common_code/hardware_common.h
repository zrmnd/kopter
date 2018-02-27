#ifndef _HARDWARE_COMMON_H_
#define _HARDWARE_COMMON_H_


#include "dwt.h" 

#define SystemCoreClock 72000000UL
#define UART_BUF_SIZE 500



void config_clock_HSE12mhz(void);

void initUart1(uint32_t baud);
void initUart2(uint32_t baud);
void initUart3(uint32_t baud);

void startListeningUart1(void);
void startListeningUart3(void);

void stopListeningUart1(void);
void stopListeningUart3(void);

void sendUart1(uint8_t *data, uint16_t len);
void sendUart2(uint8_t *data, uint16_t len);
void sendUart3(uint8_t *data, uint16_t len);

void handleUart1(void);
void handleUart3(void);


void init_iwdg(void);
void clear_iwdg(void);

#endif
