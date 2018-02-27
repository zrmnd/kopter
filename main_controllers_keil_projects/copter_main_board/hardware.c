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
					
				case CMD_POWER_SWITCH:
					if (msg.data.arg1 == 1) 
						power_periph_ctrl(msg.data.arg2);
					if (msg.data.arg1 == 2) 
						power_rfmodule_ctrl(msg.data.arg2); 
					if (msg.data.arg1 == 3) 
						power_bullet_ctrl(msg.data.arg2);
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
	•PA5 - питание Bullet
	•PA7 - питание ODROID
	•PB0 - питание модулей периферии (15В)
	•PB1 - питание SONAR'ов и радиомодуля (5В)*/
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	power_odroid_ctrl(1);
	
	GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1);  //PB0,1 - push pull out
  GPIOB->CRL |= GPIO_CRL_MODE0 | GPIO_CRL_MODE1;
  GPIOB->ODR &= ~(GPIO_ODR_ODR0 | GPIO_ODR_ODR1);  // off
		
	GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_CNF7);  //PA5,7 - push pull out
  GPIOA->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_MODE7;
  GPIOA->ODR &= ~(GPIO_ODR_ODR5);  // off
}


 

void power_odroid_ctrl(uint8_t on) {
	if (on) 
		GPIOA->ODR |= GPIO_ODR_ODR7;
	else 
		GPIOA->ODR &= ~GPIO_ODR_ODR7;	
}

void power_bullet_ctrl(uint8_t on) {
	if (on) 
		GPIOA->ODR |= GPIO_ODR_ODR5;
	else 
		GPIOA->ODR &= ~GPIO_ODR_ODR5;	
}


void power_rfmodule_ctrl(uint8_t on) {
	if (on) 
		GPIOB->ODR |= GPIO_ODR_ODR1;
	else 
		GPIOB->ODR &= ~GPIO_ODR_ODR1;		
}


void power_periph_ctrl(uint8_t on) {
	if (on) 
		GPIOB->ODR |= GPIO_ODR_ODR0;
	else 
		GPIOB->ODR &= ~GPIO_ODR_ODR0;	
}





void initModule1GPIO(void) {
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ; //Включаем тактирование GPIOA 
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN ;
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN ;

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
  
  // Включаем порт ввода-вывода и настраиваем PA2 - I, PA4 - U -> ADC 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // Включаем порт ввода-вывода и настраиваем ножку PB13 - Out ON/OFF 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // Включаем порт ввода-вывода и настраиваем ножку PB14 - вход для EXTI-прерывания по превышению тока
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // Включаем порт ввода-вывода и настраиваем ножку PB0,1, PA6,7 - TIM3 CH1-4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // Включаем тактирование таймера
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3->CR1 = 0;
  TIM3->CCER = 0;
  TIM3->CCMR1 = 0;
  TIM3->CCMR2 = 0;
  // Настраиваем таймер на использование 1,2 канала 
  TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // | TIM_CCER_CC4E | TIM_CCER_CC3E;
  // Переводим 4,3 канал в режим ШИМ2
  //TIM3->CCMR2|= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2  |  TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
  // Переводим 1,2 канал в режим ШИМ2
  TIM3->CCMR1|= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2  |  TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
  // Настраиваем прескалер
  TIM3->PSC = 21;
  // Настраиваем период таймера = 1000 циклов
  TIM3->ARR = 65535;
  // начальные значения заполнения 
  TIM3->CCR1 = 4909;
  TIM3->CCR2 = 4909;
  //TIM3->CCR3 = 4909;
  //TIM3->CCR4 = 4909;
  // Включаем таймер
  TIM3->CR1 |= TIM_CR1_CEN;
 
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 
}



void initModule1ADC(void) {
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

