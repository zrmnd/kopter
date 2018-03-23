#include "hardware_common.h" 
#include "main.h" 
#include "hardware.h"
#include "string.h"


extern uint8_t uart1_buff[500];
extern uint8_t uart2_buff[500];
extern uint8_t uart3_buff[500];

//------------------------------------------------------------------------------
// прототипы функций-обработчиков команд 

// массив поддерживаемых команд
// id команды, количество байт аргументов, указатель на функцию-обработчик; id последнего элемента массива - обязательно 0 - признак конца массива. !!! id < 255       
//------------------------------------------------------------------------------	

	
	
//------------------------------------------------------------------------------	
// глобальные переменные 
extern const char boot_str[];
//------------------------------------------------------------------------------	
char cmdInit[] = "UART\r";	

int main() {
#if DEV_ID2 == 'B'
	power_init();
	config_clock_HSE12mhz();	
#endif	
	initDWT();
	
	initUart1(57600);
	//initUart2(115200);
	initUart3(9600);
	
	power_rfmodule_ctrl(1);
	power_bullet_ctrl(1);
	
	//sendUart2((uint8_t*)boot_str, strlen(boot_str)); 
	delay_ms(1000);
	sendUart3((uint8_t*)cmdInit, strlen(cmdInit)); 
	delay_ms(500);
	sendUart3((uint8_t*)cmdInit, strlen(cmdInit)); 
	
	init_iwdg();
	
	__enable_irq();
	
	while (1) {
		if (USART1->CR1 & USART_CR1_RE) {
			//handleUart1();
		}
		
		if (USART3->CR1 & USART_CR1_RE) {
			//handleUart3();
		}
		//sendUart1((uint8_t*)str1, 16); 
    //sendUart2((uint8_t*)str2, 16); 
		//sendUart3((uint8_t*)str3, 16); 
		delay_ms(10);
		clear_iwdg();
	}
}














//------------------------------------------------------------------------------
// команды модуля


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

