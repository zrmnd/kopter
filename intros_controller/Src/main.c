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
//#include "stm32f0xx_hal.h"

volatile uint32_t flagrx = 0, flagusbrx = 0;
volatile uint8_t ch;
#define UART_BUF_SIZE 100
/* Private variables ---------------------------------------------------------*/
volatile uint8_t cmd_buff[100];
volatile uint8_t cmd_buff2[100];
volatile uint32_t ind1 = 0, ind2 = 0;

volatile uint16_t cmd_buf_index = 0;
/* Private function prototypes -----------------------------------------------*/
void Configure_USART2(uint32_t baud)
{ 
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER2|GPIO_MODER_MODER3)) | (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
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
    //flagrx = 0;
    USART2->TDR = (data[i] & (uint16_t)0x00FF);
   /* for (uint32_t k=0; k<500000; k++) {
      if (USART2->ISR & USART_ISR_RXNE) {
        uint8_t t = USART2->RDR;
        while (CDC_Transmit_FS(&t, 1) != USBD_OK);
        //flagrx = 1;
        USART2->ICR |= USART_ICR_TCCF | USART_ICR_RTOCF | USART_ICR_ORECF;
        k = 500000;
      }
      
      
      asm("nop");
      //if (flagrx) 
       // break;
    }*/
    i++;
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
         POWER_INTROS_ON();
         //CDC_Transmit_FS("Pwr cmd ON\n", 11);
      }
      else {
        POWER_INTROS_OFF();
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



int main(void) {
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();
  SystemClock_Config();
  init_mcu();
  
  Configure_USART2(115200);
  __enable_irq();
  
  
  POWER_INTROS_ON(); 
  for (uint32_t i=0; i<3000000; i++) {asm("nop");}
  POWER_INTROS_OFF();
  for (uint32_t i=0; i<3000000; i++) {asm("nop");}
  for (uint32_t i=0; i<3000000; i++) {asm("nop");}
  
  // add power checking and auto power on
  // measure time of low level on UART RX with timeout and turn intros on if it in off state
  // func togglePower(), isOn() 
  uint32_t j = 0, time = 0, flagrx_for_tx_continue = 0;
  while (1) {
  //  if (
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


