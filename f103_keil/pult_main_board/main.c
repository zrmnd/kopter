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

volatile uint8_t data[5], data_old[5];
volatile uint8_t x = 90, y = 90, z = 90, x_old = 90, y_old = 90, z_old = 90;
	
uint8_t strSetCamPos[] = "$PUI03,77,1,000,000,000,\n";

int main() {
#if DEV_ID2 == 'B'
	config_clock_HSE12mhz();
#endif	
	initDWT();
	
	initUart1(57600);
	initUart2(115200);
	//initUart3(115200);
	startListeningUart3();
	

	power_init();
	keys_init();
	
	if (!IS_POWER_HOLDED && BAT_IS_NORMAL) {
		power_pult_hold(1);
		power_bullet_ctrl(1);
	}
	
	
	
	
	sendUart2((uint8_t*)boot_str, strlen(boot_str)); 
	
	
	
	
	
	
	
	
	
	
	
	
	
	init_iwdg();
	
	//__enable_irq();
	

	
	keys_read((uint8_t*)data);
	
	while (1) {
		keys_read((uint8_t*)data);
		if (!KEYDATA_NOT_CHANGED(data, data_old)) {
			
			if (data[0] & (1 << KEY_FWD)) {
				sendUart2((uint8_t*)"$PUI12,77,2,\n", 13);
				sendUart1((uint8_t*)"$M1SS\r", 6);
				delay_ms(200);
				sendUart1((uint8_t*)"$M1L!\r", 6);
				delay_ms(100);
			}
			else if (data[0] & (1 << KEY_REV)) {
				sendUart2((uint8_t*)"$PUI12,77,-2,\n", 14);
				sendUart1((uint8_t*)"$M1SS\r", 6);
				delay_ms(200);
				sendUart1((uint8_t*)"$M1R!\r", 6);
				delay_ms(100);
			}
			else if (data[0] & (1 << KEY_STOP)) {
				//sendUart2((uint8_t*)"$PUI12,77,-2,\n", 14);
				sendUart1((uint8_t*)"$M1SS\r", 6);
				delay_ms(100);
				//sendUart1((uint8_t*)"$M1R!\r", 6);
			}
			
			
			if (data[1] < 110 && x >= 5)
				x -= 5;
			if (data[1] > 140 && x <= 250) 
				x += 5;
			
			if (data[2] < 110 && y >= 5)
				y -= 5;
			if (data[2] > 140 && y <= 250) 
				y += 5; 
			
			if (data[3] < 110 && z >= 5)
				z -= 5;
			if (data[3] > 140 && z <= 250) 
				z += 5;
			
			if (x != x_old || y != y_old || z != z_old) {
			//if (data[1] != data_old[1] || data[2] != data_old[2] || data[3] != data_old[3]) {
				strSetCamPos[12] = x/100;
				strSetCamPos[13] = (x - strSetCamPos[12]*100)/10;
				strSetCamPos[14] = (x - strSetCamPos[12]*100 - strSetCamPos[13]*10);
				strSetCamPos[12] += '0';
				strSetCamPos[13] += '0';
				strSetCamPos[14] += '0';
				
				strSetCamPos[16] = y/100;
				strSetCamPos[17] = (y - strSetCamPos[16]*100)/10;
				strSetCamPos[18] = (y - strSetCamPos[16]*100 - strSetCamPos[17]*10);
				strSetCamPos[16] += '0';
				strSetCamPos[17] += '0';
				strSetCamPos[18] += '0';
				
				strSetCamPos[20] = z/100;
				strSetCamPos[21] = (z - strSetCamPos[20]*100)/10;
				strSetCamPos[22] = (z - strSetCamPos[20]*100 - strSetCamPos[21]*10);
				strSetCamPos[20] += '0';
				strSetCamPos[21] += '0';
				strSetCamPos[22] += '0';
				
				x_old = x;
				y_old = y;
				z_old = z;
				
				sendUart2((uint8_t*)strSetCamPos, 25);
				sendUart1((uint8_t*)strSetCamPos, 25);
			}
				
			COPY_TO_OLD(data, data_old);
		}
		
		
		
		
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


		delay_ms(30);
		clear_iwdg();
	}
}














//------------------------------------------------------------------------------
// команды модуля


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

