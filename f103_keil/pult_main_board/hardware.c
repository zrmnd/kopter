#include "hardware.h" 
#include "hardware_common.h" 
#include "nmea_parser.h"
#include "base64.h"
#include <string.h>

// from hardware_common.c
extern uint8_t uart2_buff[];
extern uint16_t uart2_buf_index;
extern uint8_t uart2_flagORE;

extern uint8_t uart3_buff[];
extern uint16_t uart3_buf_index;

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




void keys_init(void) {
	// PB8 - Fwd key, PB9 - Rev key, PC13 - stop key, PC14 - joy down, PC15 - joy up; in pull-down
	// Joystick axes: PA0, PA1, PA4 (ADC_IN 0, 1, 4);  speed - PB1 (ADC_IN9)
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	
	GPIOB->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9);  //PB8,9 - pull downed in
	GPIOB->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_1;
  GPIOB->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_MODE9);
	GPIOB->ODR &= ~(GPIO_ODR_ODR8 | GPIO_ODR_ODR8);
	
	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_CNF14 | GPIO_CRH_CNF15);  //PB13, 14, 15 - pull downed in
	GPIOC->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1;
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_MODE14 | GPIO_CRH_MODE15);
	GPIOC->ODR &= ~(GPIO_ODR_ODR13 | GPIO_ODR_ODR14 | GPIO_ODR_ODR15);
	
	GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF4);  //PA0, 1, 4 - analog in
  GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE4);
	
	GPIOB->CRL &= ~GPIO_CRL_CNF1;  //PB1 - analog in
  GPIOB->CRL &= ~GPIO_CRL_MODE1;
}


void keys_read(uint8_t *data) {
	data[0] = 0; // keys: [ 0 | 0 | 0 | Forward | stop | Reverse | joy UP | joy DOWN ]
	if ((GPIOB->IDR & GPIO_IDR_IDR8))
		data[0] |= 1 << KEY_FWD;
	if ((GPIOB->IDR & GPIO_IDR_IDR9))
		data[0] |= 1 << KEY_REV;
	if ((GPIOC->IDR & GPIO_IDR_IDR13))
		data[0] |= 1 << KEY_STOP;
	if ((GPIOC->IDR & GPIO_IDR_IDR14))
		data[0] |= 1 << KEY_JOY_DOWN;
	if ((GPIOC->IDR & GPIO_IDR_IDR15))
		data[0] |= 1 << KEY_JOY_UP;
	
	data[1] = 0; // x axis
	data[2] = 0; // y axis
	data[3] = 0; // z axis
	data[4] = 0; // speed
	data[1] = adcMeasure(0)/16;
	data[3] = adcMeasure(1)/16;
	data[2] = adcMeasure(4)/16;
	data[4] = adcMeasure(9)/16;
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














// есть таблица с количеством принимаемых и отправляемых каждым из контроллеров ввода данных
uint8_t pultIOcontrollersDataLength[][3] = {{0x01, 2, 0}, {0x02, 5, 2}, {0x03, 1, 0}, {0x04, 2, 2}, {0x05, 1, 1}, {0x06, 1, 1}, {0x07, 1, 1}, {0x08, 1, 1}, {0x09, 2, 1}, {0x0A, 4, 2}};





// получить данные (состояния кнопок, ручек) от контроллера органов управления в массив *data
uint8_t pultIOcontrollerRead(char controllerId, uint8_t *data) {
	uint8_t i = 0, maxIndex = sizeof(pultIOcontrollersDataLength)/3, *p;
	uint8_t length = 255;
	for (; i < maxIndex; i++) {
		if (pultIOcontrollersDataLength[i][0] == controllerId)
			length = pultIOcontrollersDataLength[i][1];
	}
	if (length == 255)
		return 0;
	if(device485SendDataReceiveAnswer(0x07, controllerId, 0x0F, &p, 0, length)) {
		for (i = 0; i < length; i++) {
			data[i] = p[i];
		}
		return 1;
	}
	return 0;
}


// отправить данные (состояния светодиодов) контроллеру органов управления из массива *data
uint8_t pultIOcontrollerWrite(char controllerId, uint8_t *data) {
	uint8_t i = 0, maxIndex = sizeof(pultIOcontrollersDataLength)/3;
	uint8_t length = 255;
	for (; i < maxIndex; i++) {
		if (pultIOcontrollersDataLength[i][0] == controllerId)
			length = pultIOcontrollersDataLength[i][2];
	}
	if (length == 255)
		return 0;
	return device485SendDataReceiveAnswer(0x07, controllerId, 0x10, &data, length, 0); 
}





// отправка команды и данных, прием данных
uint8_t device485SendDataReceiveAnswer(char devClass, char devId, char cmd, uint8_t **data, uint8_t txLen, uint8_t rxLen) {
	uint32_t indexNextHandle = 0;
	uint32_t indexNextReceive;
  uint32_t cmdLength;
  uint8_t crc;
  uint8_t cmdData[255], rxData[255], topIndex;
  uint8_t tmpByte;
  uint32_t state = STATE_IDLE;
	uint8_t counterCmdInternal = 0;
	uint8_t flagDataReady = 0; 		// устанавливаем только когда получили корректный ответ от целевого устройства
	uint8_t counterError = 0;
	//0x07, controllerId, 0x0F, &p, 0, length)     M 0x07 0x01 0x0F 
	cmdData[0] = 'M';
	cmdData[1] = devClass;
	cmdData[2] = devId;
	cmdData[3] = cmd; // запрошенная команда
	if (txLen) {
		//cmdData[4] = txLen; // длина аргументов
		topIndex = txLen+4; // если длина 0 - то индекс будет == 4
		cmdData[topIndex] = cmdData[0] + cmdData[1] + cmdData[2] + cmdData[3]; // CRC
	}
	else {
		topIndex = 4;
		cmdData[4] = cmdData[0] + cmdData[1] + cmdData[2] + cmdData[3]; // CRC
	}
	
	for (uint16_t i = 4; i < topIndex; i++) {
		cmdData[i] = (*data)[i-4];
		cmdData[topIndex] += cmdData[i];
	}
	
	
	trySend:
	
	if (counterError > 3)
		return 0; // не подключено устройство
	
	state = STATE_IDLE;
	uart3_buf_index = 0;
	sendUart3((uint8_t*)cmdData, topIndex+1);  
	delay_ms(100);
	if (uart3_buf_index == 0) {
		counterError++;
		goto trySend;
	}
	else {
		
	
		
		indexNextReceive = uart3_buf_index;
		indexNextHandle = 0;
		while (indexNextHandle != indexNextReceive) {                       // есть необработанные данные     
			tmpByte = uart3_buff[indexNextHandle++];
			
			switch (state) {                                               // конечный автомат обработки очередного принятого байта
			case STATE_IDLE:                                                          // в ожидании появления идентификатора команды от мастера на линии
				if (tmpByte == 'S') {
					state = STATE_CMD_preambule;
					counterCmdInternal = 0;
					crc = 'S';
				}
				break;
				
			case STATE_CMD_preambule:                                                 // на линии передается команда. Принимаем класс и номер целевого устройства 
				crc += tmpByte;
				if (counterCmdInternal == 0) { // device class
					if (tmpByte == devClass)  // == 0x07      
						counterCmdInternal += 1;
					else {
						state = STATE_CMD_foreign;                                          // state -> foreign: проигнорируем следующие до паузы байты
						counterCmdInternal = 0;
					}
				}
				else {                          // device number (subclass)
					if (tmpByte == devId) { // cmd for this device 
						if (rxLen) {
							cmdLength = rxLen; 																				// небезопасный потенциально код 
							state = STATE_CMD_body; 
						}
						else {
							state = STATE_CMD_foreign;
							flagDataReady = 1;
						}
						counterCmdInternal = 0;   
						 
						
					}
					else {
						state = STATE_CMD_foreign;                                          // state -> foreign: проигнорируем следующие до паузы байты
						counterCmdInternal = 0;
					}
				}
				break;
			
			case STATE_CMD_body:                                                      // прием аргументов самой команды 
				crc += tmpByte;
				if (counterCmdInternal == 250) {
					state = STATE_CMD_foreign;
					counterCmdInternal = 0;
					
					break;
				}
				rxData[counterCmdInternal++] = tmpByte;
				if (counterCmdInternal == cmdLength)                                    // приняли все байты аргументов, следующий байт - CRC
					state = STATE_CMD_crc;        
				break;
				
			case STATE_CMD_crc:                                                      // приняли CRC
				if (crc != tmpByte) {                                                   // CRC не совпало
					//flagErrorCrc = 1;                                                     // установили флаг ошибки, после паузы на линии отправим перезапрос
					state = STATE_CMD_foreign;                                            // игнорируем последующие байты до паузы на линии
					
				}
				else {                                                                  // CRC верное - данные приняты в целости -> устройство на линии есть
					flagDataReady = 1;
					//flagErrorCrc = 0;                                                     // никогда не нужно? 
					state = STATE_CMD_foreign;                                            // ждем тишины на линии
				}
				break;
			}
			 
		}
			
}
	
		
		

	
	
	*data = rxData;
	delay_us(300);
	return 1;
}




uint8_t calculateCRC(uint8_t *data, uint16_t len) {
	uint8_t crc = 0;
	for (uint16_t i = 0; i < len; i++)
		crc += data[i];
	return crc;	
}


