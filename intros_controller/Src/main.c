#include "main.h"
#include "stm32f0xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

/* User includes -------------------------------------------------------------*/
#include "device_conf.h"
#include "init.h"
#include "nmea_parser.h"
#include "state_machine.h"
#include "base64.h"



void introsOn(void);
volatile uint32_t flagrx = 0, flagusbrx = 0;
volatile uint8_t ch;
#define UART_BUF_SIZE 100
/* Private variables ---------------------------------------------------------*/
volatile uint8_t cmd_buff[100];
volatile uint8_t cmd_buff2[100];
volatile uint8_t cmd_tr_buff[30];
volatile uint32_t ind1 = 0, ind2 = 0, ind_tr = 0;
volatile uint32_t off_state_counter = 0;

volatile uint16_t cmd_buf_index = 0;
/* Private function prototypes -----------------------------------------------*/
void Configure_USART2(uint32_t baud)
{ 
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  
  //GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
  //GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1); 
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER2|GPIO_MODER_MODER3)) | (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
  //GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_1; // PUll-down
  GPIOA->AFR[0] |= (0x01 << 8)|(0x01 << 12);
  
  /* Configure USART1 */
  /* (1) oversampling by 16, 9600 baud */
  /* (2) 8 data bit, 1 start bit, 1 stop bit, no parity */
  USART2->CR1 = 0;
  USART2->CR2 = 0;
  USART2->CR3 = 0;
  
  
  USART2->BRR = ((uint32_t)48000000 + (baud>>1) )/ (uint32_t)baud; /* (1) */
  USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; /* (2) */
  USART2->CR1 &= ~USART_CR1_M; // 8 bit
  USART2->CR2 |= USART_CR2_STOP_1; //2 stop bit

  USART2->ICR |= USART_ICR_TCCF;
  USART2->CR1 |= USART_CR1_RXNEIE;

  NVIC_SetPriority(USART2_IRQn, 1); 
  NVIC_EnableIRQ(USART2_IRQn); 
}






void sendUart2(uint8_t *data, uint16_t len) {
  uint16_t i = 0;
  while ((len == 0 && data[i] != 0) || i < len) {
    while(!(USART2->ISR & USART_ISR_TXE)); 
    USART2->TDR = (data[i] & (uint16_t)0x00FF);
    i++;
    if (data[i] != '\r')
      off_state_counter++;
  }
  while(!(USART2->ISR & USART_ISR_TC)); 
  USART2->ICR |= USART_ICR_TCCF; 
  
}



void SystemClock_Config(void);


// global vars:
extern MESSAGE_t msg;
//extern uint8_t UserRxBufferFS[256];

static volatile uint32_t index = 0;

void process_routine(void) {
  uint8_t buf[128]; // figure out about this buffer
  buf[0] = 0;
  uint32_t n = 0; // number of the last char


  switch(msg.cmd) {
    case CMD_NONE:
    default:
      // do nothing
      break;
    case CMD_UART_SPEED:
      //UART_SET_BAUDRATE(msg.data.arg2);
      break;
    case CMD_UART_TX:
      // decode message and send it to UART
      n = 0;
      
      n = base64_decode(msg.data.str, strlen((char const*)msg.data.str), buf, sizeof(buf));
       // last char of the decoded string
      //CDC_Transmit_FS(buf, n);
      sendUart2(buf, n);
      //CDC_Transmit_FS(buf, n);
      break;
    case CMD_UART_RX:

      break;

    case CMD_PPM_SET: // set angle of PPM channel

      break;
    case CMD_BATTERY:

      break;
    case CMD_POWER_SWITCH:
      // power switch!

      // INTROS pcb has only one optocoupler ouput, work directly with it 
      if (msg.data.arg2) { 
         introsOn() ;
         //CDC_Transmit_FS("Pwr cmd ON\n", 11);
      }
      else {
        introsOn() ;
         //CDC_Transmit_FS("Pwr cmd OFF\n", 12);
      }
     
      break;
    case CMD_UART_TRASNSLATOR_START:
      //CDC_Transmit_FS("$C0E05,55,2,", 12);
      //state_set(ST_FORWARDING_MODE);
      break;
    case CMD_UART_TRANSLATOR_STOP:
      //state_set(ST_NORMAL_MODE);
      //forwarding_finish();
      //CDC_Transmit_FS(",\n", 2);
      break;
  }
}


// free line detect functions 
//------------------------------------------------------------------------------
void initFreeLineDetectTim(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;     // ??? ?????????, ?????? ???????????? ??????
  TIM2->PSC = 47;                // ??????????? 48000000/48 -> f = 1MHz
  TIM2->CR1 |= TIM_CR1_OPM;       // One Pulse Mode
  TIM2->ARR = 1000; // 1ms
  TIM2->DIER |= TIM_DIER_UIE;     // ?????????? ?? ??????????     
  NVIC_DisableIRQ(TIM2_IRQn);
}

void restartFreeLineDetectTim(uint16_t ms)
{
  TIM2->CR1 &= ~TIM_CR1_CEN;
  TIM2->CNT = 0;
  TIM2->ARR = ms*1000;
  TIM2->CR1 |= TIM_CR1_CEN; 
  TIM2->SR &= ~TIM_SR_UIF;
////////////////////////////////////////////????????????? ???????? ??? ??? ???? ????????????? 
}










//------------------------------------------------------------------------------
uint32_t txAndRxByte(uint8_t chr, uint16_t timeout_ms) {
  flagrx = 0;
  sendUart2((uint8_t*)&chr, 1);
  if (timeout_ms == 0)
    return 1;
  restartFreeLineDetectTim(timeout_ms);
  while (!(TIM2->SR & TIM_SR_UIF) && !flagrx);
  if ((flagrx && (ch == chr))) {  
    return 1;
  }
  return 0;
}


void pushRxInBuff() {
  if (ind2 < sizeof(cmd_buff2) && ch != 0) {
    flagrx = 0;
    cmd_buff2[ind2++] = ch;
  }
}

void delay_ms(uint16_t ms) {
  restartFreeLineDetectTim(ms);
  while (!(TIM2->SR & TIM_SR_UIF));
  TIM2->SR &= ~TIM_SR_UIF;
}



void sendString(uint8_t *str, uint16_t len, uint16_t timeout_ms) {
  uint16_t i = 0;
  uint8_t errors = 0;
  while (i < len && errors < 5) {
    if (txAndRxByte(*(str+i), timeout_ms) ) {
      i++;
      pushRxInBuff();
    }
    else if (*(str+i) != '\r') {
      i = 0;
      txAndRxByte('\r', 0);
      delay_ms(100);
      txAndRxByte('\r', 0);
      delay_ms(100);
      ind2 = 0;
      errors++;
      continue;      
    }  	
  }
  if (i == len) {
    delay_ms(2);
  }
  else {
    delay_ms(1);
  }
}







void introsOn(void) {
  delay_ms(100);
  POWER_INTROS_ON(); 
  delay_ms(1500);
  POWER_INTROS_OFF();
  delay_ms(600);
  txAndRxByte('\r', 0);
  delay_ms(100);
  txAndRxByte('\r', 0);
  delay_ms(500);
  sendString((uint8_t*)"KEYBOARD", 8, 300);  
  delay_ms(100);
  txAndRxByte('\r', 0);
  delay_ms(300);
  txAndRxByte('7', 0); delay_ms(200);
  txAndRxByte('7', 0); delay_ms(200);
  txAndRxByte('\r', 0); delay_ms(200);
  txAndRxByte('\r', 0); delay_ms(100);
  delay_ms(400);
  sendString((uint8_t*)"12 0 0 9 4 18", 13, 600);
  txAndRxByte('\r', 0); delay_ms(300);
  txAndRxByte('\r', 0);
  delay_ms(500);
  sendString((uint8_t*)"TIME!\r", 6, 500);
  //sendString((uint8_t*)"12\r0\r\0\r1\r4\r18\rTIME!\r", 20, 300);  
  delay_ms(200);
}



int main(void) {
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();
  SystemClock_Config();
  init_mcu();
  initFreeLineDetectTim(); restartFreeLineDetectTim(100);
  
  Configure_USART2(115200);
  __enable_irq();
  
  if (!txAndRxByte('1', 300))
   introsOn();
  else 
    txAndRxByte(0x08, 300);
  

  uint32_t j = 0, time = 0, flagrx_for_tx_continue = 0;
  while (1) {
    if (off_state_counter > 10) {
      introsOn();
      off_state_counter = 0;
    }
      
  
    if (flagusbrx) { // was received data by USB 
      if (time) 
        time--;
      if (flagrx_for_tx_continue) {
        time = 0;
        flagrx_for_tx_continue = 0;
      }
      if (j < ind1 && time == 0) {
        sendUart2((uint8_t*)&cmd_buff[j++], 1);
        time = 1000000;
      }
      else if (j >= ind1 && time == 0 ) {
        j = 0;
        flagusbrx = 0;
        ind1 = 0;
      }
    }
    if (flagrx && flagusbrx) {
      
      if (ind2 < sizeof(cmd_buff2) && ch != 0) {
        flagrx = 0;
        cmd_buff2[ind2++] = ch;
        flagrx_for_tx_continue = 1;
      }

    } else if (flagrx && flagusbrx == 0) {
      if (CDC_Transmit_FS((uint8_t*)&ch, 1)== USBD_OK)
        flagrx = 0;
    }
    
    if (flagusbrx == 0 && ind2) {
        if (CDC_Transmit_FS((uint8_t*)cmd_buff2, ind2)== USBD_OK)
          ind2 = 0;
    }
    
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;
    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/* ----------------------------------------------------- */
/* --- Interrupts                                    --- */
/* ----------------------------------------------------- */
#define UART_BUFFER_SIZE                                128
#define FORWARDING_BUFFER_SIZE                          3
uint8_t uart_data[FORWARDING_BUFFER_SIZE]               = { 0 };
uint8_t base64_data[FORWARDING_BUFFER_SIZE + 1]         = { 0 };

uint32_t b_index = 0;






void USART2_IRQHandler(void) {
  if (USART2->ISR & USART_ISR_RXNE) {
    ch = USART2->RDR;
    flagrx = 1;
    if (ch != 0)
      off_state_counter = 0;
    
    
    //CDC_Transmit_FS(&t, 1);
    //flagrx = 1;
  }
  USART2->ICR |= USART_ICR_TCCF | USART_ICR_RTOCF | USART_ICR_ORECF;
  //USART2->SR &= ~ (USART_SR_IDLE | USART_SR_ORE | USART_SR_RXNE | USART_SR_TXE);
}




/****************************************************************/

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  while(1);
}
#endif /* USE_FULL_ASSERT */


