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
	


volatile float v = 0;

int main() {
#if DEV_ID2 == 'B'
	config_clock_HSE12mhz();
#endif	
	initDWT();
	
	initUart1(57600);
	initUart2(115200);
	initUart3(115200);

	power_init();
	
	if (!IS_POWER_HOLDED && BAT_IS_NORMAL) {
		power_pult_hold(1);
		power_bullet_ctrl(1);
	}
	
	
	
	
	sendUart2((uint8_t*)boot_str, strlen(boot_str)); 
	
	init_iwdg();
	
	__enable_irq();
	
	uint16_t cnt = 0;
	
	while (1) {
		if (USART1->CR1 & USART_CR1_RE) {
			handleUart1();
		}
		
		if (USART3->CR1 & USART_CR1_RE) {
			handleUart3();
		}
		
		 v = BAT_VOLTAGE;
		
		if (IS_POWER_HOLDED && !BAT_IS_NORMAL) {
			power_pult_hold(0);
			power_bullet_ctrl(0);
		}
		
		if (!IS_PWRKEY_PRESSED && IS_POWER_HOLDED)  {
			power_pult_hold(0);	
			power_bullet_ctrl(0);
			
		}
		
		if (!IS_PWRKEY_PRESSED && !IS_POWER_HOLDED)  {
			power_pult_hold(0);	
			power_bullet_ctrl(0);			
		}
		
		if (IS_PWRKEY_PRESSED && !IS_POWER_HOLDED) {
			power_pult_hold(1);	
			power_bullet_ctrl(1);
		}


		delay_ms(1);
		clear_iwdg();
	}
}














//------------------------------------------------------------------------------
// команды модуля


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

