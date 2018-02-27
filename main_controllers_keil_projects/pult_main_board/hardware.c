#include "hardware.h" 
#include "hardware_common.h" 
#include "nmea_parser.h"
#include "base64.h"
#include <string.h>

// from hardware_common.c
extern uint8_t uart2_buff[];
extern uint16_t uart2_buf_index;
extern uint8_t uart2_flagORE;

uint8_t tmp_buff[500];

extern volatile MESSAGE_t msg;

void USART2_IRQHandler(void) {
	static uint16_t receiveIndex = 0;
	uint8_t t;
	if (USART2->SR & USART_SR_RXNE) {
		t = USART2->DR;
		if (t == '$')
			receiveIndex = 0;
		
			
		uart2_buff[receiveIndex++] = t;
		
		if (t == '\n' && receiveIndex < UART_BUF_SIZE) { // msg received, parse; 
			uart2_buff[receiveIndex] = 0;
			//sendUart1(uart2_buff, receiveIndex);
			nmea_parse_msg(uart2_buff);
			switch(msg.cmd) {
				case CMD_NONE:
					
					break;
				
				case CMD_UART_SPEED:
					switch(msg.data.arg1) {
						case 1:
							initUart1(msg.data.arg2);
							break;
						case 3:
							initUart3(msg.data.arg2);
							break;						
					}
					break;
					
				case CMD_UART_TX:
					uint16_t len;
					len = base64_decode((const uint8_t*)msg.data.str, strlen((const char*)msg.data.str),  tmp_buff, sizeof(tmp_buff));
					if (msg.data.arg1 == 1) 
						sendUart1(tmp_buff, len);
					if (msg.data.arg1 == 3) 
						sendUart3(tmp_buff, len);	
					break;
				
				case CMD_UART_TRASNSLATOR_START:
					if (msg.data.arg1 == 1) 
						startListeningUart1();
					if (msg.data.arg1 == 3) 
						startListeningUart3();
					break;
					
				case CMD_UART_TRANSLATOR_STOP:
					if (msg.data.arg1 == 1) 
						stopListeningUart1();
					if (msg.data.arg1 == 3) 
						stopListeningUart3();
					break;
				
			}
		}
		
		if (receiveIndex >= UART_BUF_SIZE)
			receiveIndex = 0;
		if (receiveIndex == uart2_buf_index) // обработка ситуации, когда получили новое быстрее, чем обработали старое
			uart2_flagORE = 1;
	}
	else 
		t = USART2->DR;
	USART2->SR &= ~ (USART_SR_IDLE | USART_SR_ORE | USART_SR_RXNE | USART_SR_TXE);
}


 


void power_init(void) {
	/*
 PA5 - измерение напряжения на АКБ - 100к:26.1к
 PA6 - проверка наличия напряжения на БП IN (floating, есть = 1)
 PA7 - проверка наличия напряжения на DC IN (floating, есть = 1)
 PB4 - разрешение питания от аккума (PP, разрешено = 1)
 PB3 - измерения состояния кнопки включения (вход с подтяжкой к +, нажата = 0)
 PB0 - питание Bullet (PP, 1 - вкл) 
*/
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;		
	
	GPIOB->CRL &= ~(GPIO_CRL_CNF4);  //PB4 - push pull out
  GPIOB->CRL |= GPIO_CRL_MODE4;
  GPIOB->ODR &= ~(GPIO_ODR_ODR4);  // off
	
	GPIOB->CRL &= ~(GPIO_CRL_CNF0);  //PB0 - push pull out
  GPIOB->CRL |= GPIO_CRL_MODE0;
  GPIOB->ODR &= ~(GPIO_ODR_ODR0);  // off
	
	GPIOB->CRL &= ~(GPIO_CRL_CNF3);  //PB3 - pull up in
	GPIOB->CRL |= GPIO_CRL_CNF3_1;
  GPIOB->CRL &= ~GPIO_CRL_MODE3;
	GPIOB->ODR |= (GPIO_ODR_ODR3);
	
	GPIOA->CRL &= ~GPIO_CRL_CNF5;  //PA5 - analog in
  GPIOA->CRL &= ~GPIO_CRL_MODE5;
  
		
	GPIOA->CRL &= ~(GPIO_CRL_CNF7);  //PA7 - push pull out
  GPIOA->CRL |= GPIO_CRL_MODE7;
  GPIOA->ODR &= ~(GPIO_ODR_ODR5);  // off
	
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 
	
	initADC();
}





void power_bullet_ctrl(uint8_t on) {
	if (on) 
		GPIOB->ODR |= GPIO_ODR_ODR0;
	else 
		GPIOB->ODR &= ~GPIO_ODR_ODR0;	
}



void power_pult_hold(uint8_t on) {
	if (on) 
		GPIOB->ODR |= GPIO_ODR_ODR4;
	else 
		GPIOB->ODR &= ~GPIO_ODR_ODR4;	
}








void initADC(void) {
	//так как тактовая частота АЦП не должна превышать 14MHz
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
  //разрешаем тактирование АЦП
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN ;
  
  ADC1->CR2 = 0;
  ADC1->CR1 = 0;
  
  //запускаем калибровку и ждем пока завершится,  в симуляторе это не работает, в железе делать обязательно
  ADC1->CR2 |= ADC_CR2_CAL; 
  while (!(ADC1->CR2 & ADC_CR2_CAL));
  
  ADC1->CR2 |= ADC_CR2_ADON;              // подаем питание на ФЦП
  ADC1->CR2 |= ADC_CR2_EXTSEL;    // запуск преобразования по установки бита swstart      
  ADC1->CR2 |= ADC_CR2_EXTTRIG;   // включаем запуск от внешнего события (у нас это свтарт)
}


uint16_t adcMeasure(uint8_t channel) {
	uint32_t tmp = 0, i;
 
	//длина последовательности равна 1, первый канал
  ADC1->SQR1 = 0;
  ADC1->SQR2 = 0;
  ADC1->SQR3 = channel & 0x0F;
  tmp = 1000;
  //запускаем АЦП
  while(tmp--) { __nop(); }
 
  ADC1->CR2 |= ADC_CR2_SWSTART;
  while(!(ADC1->SR & ADC_SR_EOC)); 
  tmp = ADC1->DR & 0xFFF; 
  ADC1->SR = 0;
  
  for (i = 0; i < 15; i++) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC)); 
    tmp = ((uint32_t)tmp*(uint32_t)9 + (uint32_t)((uint32_t)ADC1->DR & 0x000FFF));
    tmp = (uint32_t)tmp/(uint32_t)10;
    ADC1->SR = 0;    
  }
  return (0xFFFF & (uint16_t)tmp);
}



//------------------------------------------------------------------------------
void initModule1CCRChangeTim(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;     // уже привычное, ключаю тактирование модуля
  TIM4->PSC = 35;                // пределитель 36000000/36 -> f = 1MHz
  TIM4->CR1 |= TIM_CR1_OPM;       // One Pulse Mode
  TIM4->ARR = 400; // 400us (6000 за 3сек) 
  TIM4->DIER |= TIM_DIER_UIE;     // прерывание по обновлению     
  NVIC_DisableIRQ(TIM4_IRQn);
}

__inline void restartModule1CCRChangeTim(void) {
  TIM4->CR1 &= ~TIM_CR1_CEN;
  TIM4->CNT = 0;
  TIM4->CR1 |= TIM_CR1_CEN; 
}
//------------------------------------------------------------------------------

