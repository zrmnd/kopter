#include "hardware_common.h"
#include "main.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_iwdg.h"
#include "string.h"
#include "base64.h"

volatile uint8_t uart1_buff[UART_BUF_SIZE];
volatile uint8_t uart2_buff[UART_BUF_SIZE];
volatile uint8_t uart3_buff[UART_BUF_SIZE];
volatile uint16_t uart1_buf_index = 0, uart2_buf_index = 0, uart3_buf_index = 0;
volatile uint8_t uart1_flagORE = 0, uart2_flagORE = 0, uart3_flagORE = 0;

volatile uint16_t receiveIndex_uart1 = 0;
volatile uint16_t receiveIndex_uart3 = 0;

const char uart1_rx_answer_begin[] = {'$', DEV_ID1, DEV_ID2, 'E', '0', '5', ',', '7', '7', ',', '1', ','};
const char uart3_rx_answer_begin[] = {'$', DEV_ID1, DEV_ID2, 'E', '0', '5', ',', '7', '7', ',', '3', ','};
const char uartx_rx_answer_end[] = {',', '\n', 0};
const char boot_str[] = {'$', DEV_ID1, DEV_ID2, 'E', '0', '0', ',', 'R', 'U', 'N', ',', '\n', 0};
//------------------------------------------------------------------------------














//------------------------------------------------------------------------------
void handleUart1(void) { // если набралось 3 байта или более во входном массиве - пакуем base64 и отправляем
	uint16_t copy_receiveIndex_uart1 = receiveIndex_uart1;
	if (uart1_flagORE == 0 && copy_receiveIndex_uart1 != uart1_buf_index) {
		char tmp[50], tmp2[150], size;
		if (copy_receiveIndex_uart1 > uart1_buf_index) {
			size = copy_receiveIndex_uart1-uart1_buf_index;
			if (size >= 3) {
				size = (size / 3) * 3; 
				if (size > 48)
					size = 48;
				memcpy((void*)tmp, (const void*)&uart1_buff[uart1_buf_index], size);
				uart1_buf_index = uart1_buf_index + size;
				tmp[size] = 0;
			}
			else
				size = 0;
		}
		else {
			size = UART_BUF_SIZE-uart1_buf_index+copy_receiveIndex_uart1;
			if (size >= 3) {
				size = (size / 3) * 3;
				if (size > 48)
					size = 48;
				if (uart1_buf_index + size - 1 < UART_BUF_SIZE) {
					memcpy((void*)tmp, (const void*)&uart1_buff[uart1_buf_index], size);
					uart1_buf_index += size; // проверка на размер добавить
					if (uart1_buf_index >= UART_BUF_SIZE)
						uart1_buf_index -= UART_BUF_SIZE;
				}
				else {
					memcpy((void*)tmp, (const void*)&uart1_buff[uart1_buf_index], UART_BUF_SIZE-uart1_buf_index);
					memcpy((void*)&tmp[UART_BUF_SIZE-uart1_buf_index], (const void*)uart1_buff, size + uart1_buf_index - UART_BUF_SIZE);
					uart1_buf_index = size + uart1_buf_index - UART_BUF_SIZE;
				}
				tmp[size] = 0;
			}
			else
				size = 0;
		}
		if (size) {
			size = base64_encode((uint8_t*)tmp, size, (unsigned char*)tmp2, 150);
		sendUart2((uint8_t*)tmp2, size);
		}
	}
	
}
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
void handleUart3(void) { // если набралось 3 байта или более во входном массиве - пакуем base64 и отправляем
	uint16_t copy_receiveIndex_uart3 = receiveIndex_uart3;
	if (uart3_flagORE == 0 && copy_receiveIndex_uart3 != uart3_buf_index) {
		char tmp[50], tmp2[150], size;
		if (copy_receiveIndex_uart3 > uart3_buf_index) {
			size = copy_receiveIndex_uart3-uart3_buf_index;
			if (size >= 3) {
				size = (size / 3) * 3; 
				if (size > 48)
					size = 48;
				memcpy((void*)tmp, (const void*)&uart3_buff[uart3_buf_index], size);
				uart3_buf_index = uart3_buf_index + size;
				tmp[size] = 0;
			}
			else
				size = 0;
		}
		else {
			size = UART_BUF_SIZE-uart3_buf_index+copy_receiveIndex_uart3;
			if (size >= 3) {
				size = (size / 3) * 3;
				if (size > 48)
					size = 48;
				if (uart3_buf_index + size - 1 < UART_BUF_SIZE) {
					memcpy((void*)tmp, (const void*)&uart3_buff[uart3_buf_index], size);
					uart3_buf_index += size; // проверка на размер добавить
					if (uart3_buf_index >= UART_BUF_SIZE)
						uart3_buf_index -= UART_BUF_SIZE;
				}
				else {
					memcpy((void*)tmp, (const void*)&uart3_buff[uart3_buf_index], UART_BUF_SIZE-uart3_buf_index);
					memcpy((void*)&tmp[UART_BUF_SIZE-uart3_buf_index], (const void*)uart3_buff, size + uart3_buf_index - UART_BUF_SIZE);
					uart3_buf_index = size + uart3_buf_index - UART_BUF_SIZE;
				}
				tmp[size] = 0;
			}
			else
				size = 0;
		}
		if (size) {
			size = base64_encode((uint8_t*)tmp, size, (unsigned char*)tmp2, 150);
			sendUart2((uint8_t*)tmp2, size);
		}
	}
	
	
}
//------------------------------------------------------------------------------













//------------------------------------------------------------------------------
void initUart1(uint32_t baud) { // настроить UART1 на передачу, задать скорость
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ; //Включаем тактирование GPIOA 
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;       // подаю такты на DMA1
  
  //GPIOA
  GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9); //PA9 - TX - выход Push_Pull, альтернативная функция, скорость 50 МГц
  GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
  GPIOA->CRH |= GPIO_CRH_CNF10_1; // PA10 - RX - Pull up
  GPIOA->CRH &= ~GPIO_CRH_CNF10_0;  

  //USART2
  USART1->CR1 = 0;
  USART1->CR2 = 0;
  USART1->CR3 = 0;
  USART1->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE; //Включаем USART1
  USART1->CR1 &= ~USART_CR1_M; //Размерность слова данных - 8 бит
  USART1->CR2 &= ~USART_CR2_STOP; //1 стоп-бит
  USART1->BRR = ((uint32_t)72000000+(baud>>1))/(uint32_t)baud; // 0x271; //115200baud 0x139 for APB1 CLK //0x2D; //Скорость обмена 800 000 бод (36МГц/800кГц = 45)
  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; //Включаем передатчик USART1  
  //USART2->CR1 |= USART_CR1_IDLEIE; 
  USART1->CR3 |= USART_CR3_DMAT;	
	
	NVIC_SetPriority(USART1_IRQn, 1);
  NVIC_EnableIRQ(USART1_IRQn);
}


void startListeningUart1(void) { // отправить ответ в UART2 о том, что ретрансляция UART1 начата; включить приемник UART1 
	if (USART3->CR1 & USART_CR1_RE) // если не остановлен прием с UART3, останавливаем и завершаем корректно
		stopListeningUart3();
	receiveIndex_uart1 = 0;
	uart1_flagORE = 0;
	uart1_buf_index = 0;
	
	USART1->CR1 |= USART_CR1_RE;
	sendUart2((uint8_t*)uart1_rx_answer_begin, sizeof(uart1_rx_answer_begin));
	
}

// UART_BUF_SIZE = 6
//  0 1 2 3 4 5
//  r     h
//  h = copy_receiveIndex_uart1 = 3 - первый свободный элемент
//  r = uart1_buf_index = 0 - первый необработанный
//  нужно копировать и обабатывать 0..2 (3 элемента)

//  0 1 2 3 4 5
//    h   r
//  h = copy_receiveIndex_uart1 = 1 - первый свободный элемент
//  r = uart1_buf_index = 3 - первый необработанный
//  нужно копировать и обабатывать 3..5, 0 (4 элемента)



void stopListeningUart1(void) { // упаковать base64 если что-то осталось в буфере и отправить с ответом в UART2 о окончании ретрансляции UART1; выключить приемник UART1 
	USART1->CR1 &= ~USART_CR1_RE;
	uint16_t copy_receiveIndex_uart1 = receiveIndex_uart1;
	// доотправка здесь
	if (uart1_flagORE == 0 && copy_receiveIndex_uart1 != uart1_buf_index) {
		char tmp[50], tmp2[150], size;
		if (copy_receiveIndex_uart1 > uart1_buf_index) {
			size = copy_receiveIndex_uart1-uart1_buf_index;
			memcpy((void*)tmp, (const void*)&uart1_buff[uart1_buf_index], size);
			tmp[size] = 0;
		}
		else {
			memcpy((void*)tmp, (const void*)&uart1_buff[uart1_buf_index], UART_BUF_SIZE-uart1_buf_index);
			memcpy((void*)&tmp[UART_BUF_SIZE-uart1_buf_index], (const void*)uart1_buff, copy_receiveIndex_uart1);
			size = UART_BUF_SIZE-uart1_buf_index+copy_receiveIndex_uart1;
			tmp[size] = 0;
		}
		

		size = base64_encode((uint8_t*)tmp, size, (unsigned char*)tmp2, 150);
		sendUart2((uint8_t*)tmp2, size);
	}
	
	
	sendUart2((uint8_t*)uartx_rx_answer_end, sizeof(uartx_rx_answer_end));
}

void sendUart1(uint8_t *data, uint16_t len) {

  // DMA1 UART2_TX
  DMA1->IFCR |= DMA_IFCR_CTCIF4; 
  DMA1->IFCR |= DMA_IFCR_CTEIF4; 
  DMA1_Channel4->CCR = 0;
  DMA1_Channel4->CPAR = (uint32_t) &USART1->DR;     // адрес периферии
  DMA1_Channel4->CMAR = (uint32_t) &data[0];   // адрес в памяти
  DMA1_Channel4->CNDTR = len;     
  DMA1_Channel4->CCR &= ~DMA_CCR4_CIRC;    // не циклическая передача
  DMA1_Channel4->CCR |= DMA_CCR4_MINC;    // memory increment addr enable
  DMA1_Channel4->CCR &= ~DMA_CCR4_PINC;   // periph increment addr none
  DMA1_Channel4->CCR |= DMA_CCR4_DIR;    // from memory to periph
  DMA1_Channel4->CCR |= DMA_CCR4_EN;      // включаем  
  
  while(!((DMA1->ISR & DMA_ISR_TCIF4) || (DMA1->ISR & DMA_ISR_TEIF4)));
  DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------






//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void initUart3(uint32_t baud) { // настроить UART1 на передачу, задать скорость
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN ; //Включаем тактирование GPIOB

	//GPIOB
	GPIOB->CRH &= ~GPIO_CRH_CNF10_0;
	GPIOB->CRH |= (GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10); //PB10 - TX - выход Push_Pull, альтернативная функция, скорость 50 МГц
	GPIOB->CRH |= GPIO_CRH_CNF11_1; // PB11 - RX - Pull up
  GPIOB->CRH &= ~GPIO_CRH_CNF11_0;
  
#if DEV_ID2 == 'A'
	GPIOB->CRL &= ~GPIO_CRL_CNF1;  //PB1 - push pull out, control receive/transmit 
  GPIOB->CRL |= GPIO_CRL_MODE1;
  GPIOB->ODR &= ~GPIO_ODR_ODR1;  // receive on, transmit off
#endif
	
	//USART3
	USART3->CR1 = 0;
	USART3->CR2 = 0;
	USART3->CR3 = 0;
	USART3->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE; //Включаем USART1
	USART3->CR1 &= ~USART_CR1_M; //Размерность слова данных - 8 бит
	USART3->CR2 &= ~USART_CR2_STOP; //1 стоп-бит
	USART3->BRR = ((uint32_t)36000000+(baud>>1))/(uint32_t)baud; 
	USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; //Включаем передатчик USART1
	USART3->CR3 |= USART_CR3_DMAT;

	NVIC_SetPriority(USART3_IRQn, 1);
  NVIC_EnableIRQ(USART3_IRQn);
}


void startListeningUart3(void) { // отправить ответ в UART2 о том, что ретрансляция UART1 начата; включить приемник UART1 
	if (USART1->CR1 & USART_CR1_RE) // если не остановлен прием с UART1, останавливаем и завершаем корректно
		stopListeningUart1();
	receiveIndex_uart3 = 0;
	uart3_flagORE = 0;
	uart3_buf_index = 0;
	
	USART3->CR1 |= USART_CR1_RE;
	sendUart2((uint8_t*)uart3_rx_answer_begin, sizeof(uart3_rx_answer_begin));
}


void stopListeningUart3(void) { // упаковать base64 если что-то осталось в буфере и отправить с ответом в UART2 о окончании ретрансляции UART1; выключить приемник UART1 
	USART3->CR1 &= ~USART_CR1_RE;
	uint16_t copy_receiveIndex_uart3 = receiveIndex_uart3;
	// доотправка здесь
	if (uart3_flagORE == 0 && copy_receiveIndex_uart3 != uart3_buf_index) {
		char tmp[50], tmp2[150], size;
		if (copy_receiveIndex_uart3 > uart3_buf_index) {
			size = copy_receiveIndex_uart3-uart3_buf_index;
			memcpy((void*)tmp, (const void*)&uart3_buff[uart3_buf_index], size);
			tmp[size] = 0;
		}
		else {
			memcpy((void*)tmp, (const void*)&uart3_buff[uart3_buf_index], UART_BUF_SIZE-uart3_buf_index);
			memcpy((void*)&tmp[UART_BUF_SIZE-uart3_buf_index], (const void*)uart3_buff, copy_receiveIndex_uart3);
			size = UART_BUF_SIZE-uart3_buf_index+copy_receiveIndex_uart3;
			tmp[size] = 0;
		}
		
		
		size = base64_encode((uint8_t*)tmp, size, (unsigned char*)tmp2, 150);
		sendUart2((uint8_t*)tmp2, size);
	}
	
	
	sendUart2((uint8_t*)uartx_rx_answer_end, sizeof(uartx_rx_answer_end));
}


void sendUart3(uint8_t *data, uint16_t len) {
#if DEV_ID2 == 'A'
	uint8_t tmp = 0;
	if (USART3->CR1 & USART_CR1_RE)
		tmp = 1;
  GPIOB->ODR |= GPIO_ODR_ODR1;  //  transmit on
	delay_us(30);
	USART3->CR1 &= ~USART_CR1_RE;
	delay_us(30);
#endif
	
	
  // DMA1 UART3_TX
	DMA1->IFCR |= DMA_IFCR_CTCIF2;
	DMA1->IFCR |= DMA_IFCR_CTEIF2;
	DMA1_Channel2->CCR = 0;
	DMA1_Channel2->CPAR = (uint32_t) &USART3->DR;     // адрес периферии
	DMA1_Channel2->CMAR = (uint32_t) &data[0];   // адрес в памяти
	DMA1_Channel2->CNDTR = len;
	DMA1_Channel2->CCR &= ~DMA_CCR2_CIRC;    // не циклическая передача
	DMA1_Channel2->CCR |= DMA_CCR2_MINC;    // memory increment addr enable
	DMA1_Channel2->CCR &= ~DMA_CCR2_PINC;   // periph increment addr none
	DMA1_Channel2->CCR |= DMA_CCR2_DIR;    // from memory to periph
	DMA1_Channel2->CCR |= DMA_CCR2_EN;      // включаем

	while(!((DMA1->ISR & DMA_ISR_TCIF2) || (DMA1->ISR & DMA_ISR_TEIF2)));
	DMA1_Channel2->CCR &= ~DMA_CCR2_EN;
  
#if DEV_ID2 == 'A'
	delay_us(30);
  GPIOB->ODR &= ~GPIO_ODR_ODR1;  //  transmit off
	delay_us(30);
	if (tmp)
		USART3->CR1 |= USART_CR1_RE;
#endif
}
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void initUart2(uint32_t baud) { // настроить UART1 на передачу, задать скорость
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ; //Включаем тактирование GPIOA 
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;       // подаю такты на DMA1
  
  //GPIOA
  GPIOA->CRL |= (GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2); //PA2 - TX - выход Push_Pull, альтернативная функция, скорость 50 МГц
  GPIOA->CRL &= ~GPIO_CRL_CNF2_0;
  GPIOA->CRL |= GPIO_CRL_CNF3_1; // PA3 - RX - Pull up
  GPIOA->CRL &= ~GPIO_CRL_CNF3_0;  

  //USART2
  USART2->CR1 = 0;
  USART2->CR2 = 0;
  USART2->CR3 = 0;
  USART2->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE; //Включаем USART1
  USART2->CR1 &= ~USART_CR1_M; //Размерность слова данных - 8 бит
  USART2->CR2 &= ~USART_CR2_STOP; //1 стоп-бит
  USART2->BRR = ((uint32_t)36000000+(baud>>1))/(uint32_t)baud; //115200baud //0x2D; //Скорость обмена 800 000 бод (36МГц/800кГц = 45)
  USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; //Включаем передатчик USART2   
  USART2->CR3 |= USART_CR3_DMAT;
	
	
	NVIC_SetPriority(USART2_IRQn, 1);
  NVIC_EnableIRQ(USART2_IRQn);
}




void sendUart2(uint8_t *data, uint16_t len) {

  // DMA1 UART2_TX
  DMA1->IFCR |= DMA_IFCR_CTCIF7; 
  DMA1->IFCR |= DMA_IFCR_CTEIF7; 
  DMA1_Channel7->CCR = 0;
  DMA1_Channel7->CPAR = (uint32_t) &USART2->DR;     // адрес периферии
  DMA1_Channel7->CMAR = (uint32_t) &data[0];   // адрес в памяти
  DMA1_Channel7->CNDTR = len;     
  DMA1_Channel7->CCR &= ~DMA_CCR7_CIRC;    // не циклическая передача
  DMA1_Channel7->CCR |= DMA_CCR7_MINC;    // memory increment addr enable
  DMA1_Channel7->CCR &= ~DMA_CCR7_PINC;   // periph increment addr none
  DMA1_Channel7->CCR |= DMA_CCR7_DIR;    // from memory to periph
  DMA1_Channel7->CCR |= DMA_CCR7_EN;      // включаем  
  
  while(!((DMA1->ISR & DMA_ISR_TCIF7) || (DMA1->ISR & DMA_ISR_TEIF7)));
  DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------







//------------------------------------------------------------------------------
void config_clock_HSE12mhz(void)
{
    ErrorStatus HSEStartUpStatus;
 
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* Системный RESET RCC (делать не обязательно, но полезно на этапе отладки) */
    RCC_DeInit();
 
    /* Включаем HSE (внешний кварц) */
    RCC_HSEConfig( RCC_HSE_ON);
 
    /* Ждем пока HSE будет готов */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
 
    /* Если с HSE все в порядке */
    if (HSEStartUpStatus == SUCCESS)
    {
    /* Следующие две команды касаются исключительно работы с FLASH.
    Если вы не собираетесь использовать в своей программе функций работы с Flash,
    FLASH_PrefetchBufferCmd( ) та FLASH_SetLatency( ) можно закомментировать */
 
        /* Включаем Prefetch Buffer */
        FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);
 
        /* FLASH Latency.
    Рекомендовано устанавливать:
        FLASH_Latency_0 - 0 < SYSCLK? 24 MHz
        FLASH_Latency_1 - 24 MHz < SYSCLK ? 48 MHz
        FLASH_Latency_2 - 48 MHz < SYSCLK ? 72 MHz */
        FLASH_SetLatency( FLASH_Latency_2);
 
        /* HCLK = SYSCLK */ /* Смотри на схеме AHB Prescaler. Частота не делится (RCC_SYSCLK_Div1) */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);
 
        /* PCLK2 = HCLK */ /* Смотри на схеме APB2 Prescaler. Частота не делится (RCC_HCLK_Div1)  */
        RCC_PCLK2Config( RCC_HCLK_Div1);
 
        /* PCLK1 = HCLK/2 */ /* Смотри на схеме APB1 Prescaler. Частота делится на 2 (RCC_HCLK_Div2)
        потому что на выходе APB1 должно быть не более 36МГц (смотри схему) */
        RCC_PCLK1Config( RCC_HCLK_Div2);
 
        /* PLLCLK = 8MHz * 9 = 72 MHz */
        /* Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) */
        /* PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
 
        /* Включаем PLL */
        RCC_PLLCmd( ENABLE);
 
        /* Ждем пока PLL будет готов */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }
 
        /* Переключаем системное тактирование на PLL */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);
 
        /* Ждем пока переключиться */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* Проблемы с HSE. Тут можно написать свой код, если надо что-то делать когда микроконтроллер не смог перейти на работу с внешним кварцом */
 
        /* Пока тут заглушка - вечный цикл*/
        while (1)
        {
        }
    }
}
//------------------------------------------------------------------------------




void init_iwdg(void) {
	RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
 
  /* Enable Watchdog*/
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_4); // 4, 8, 16 ... 256
  IWDG_SetReload(0x0FFF);//This parameter must be a number between 0 and 0x0FFF.
  IWDG_ReloadCounter();
  IWDG_Enable();
}


void clear_iwdg(void) {
	IWDG_ReloadCounter();
}










