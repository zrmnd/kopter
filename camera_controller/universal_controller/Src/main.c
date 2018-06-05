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

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

// local functions
void forwarding_finish(void);

// global vars:
extern MESSAGE_t msg;
//extern uint8_t UserRxBufferFS[256];

static volatile uint32_t index = 0;

void process_routine(void) {
#if DEVICE_TYPE != LIDAR
  uint8_t buf[128] = { '\0' }; // figure out about this buffer
  uint32_t n = 0; // number of the last char
#endif
  // handle source if you need it!
  switch(msg.cmd) {
    case CMD_NONE:
    default:
      // do nothing
      break;
#if DEVICE_TYPE != LIDAR
    case CMD_UART_SPEED:
      UART_SET_BAUDRATE(msg.data.arg2);
      break;
    case CMD_UART_TX:
      // decode message and send it to UART
      n = 0;
      do n++; while (msg.data.str[n] != '\0');
      base64_decode(msg.data.str, n, buf, 128);
      n = 0; // last char of the decoded string
      do n++; while (msg.data.str[n] != '\0');
      for (uint32_t i = 0; i < n; i++) {
        SEND_UART(buf[i]);
      }
      break;
    case CMD_UART_RX:
      // TODO: handle this issue???
      // dunno what to do!
      break;
#endif
    case CMD_PPM_SET: // set angle of PPM channel
#if DEVICE_TYPE == UNIVERSAL
    // TODO: rewrite more optimaly ??
      switch(msg.data.arg1) {
        case 1:
        default:
          PPM1_ANGLE(CALC_DUTY(msg.data.arg2));
          break;
        case 2:
          PPM2_ANGLE(CALC_DUTY(msg.data.arg2));
          break;
        case 3:
          PPM3_ANGLE(CALC_DUTY(msg.data.arg2));
          break;
        case 4:
          PPM4_ANGLE(CALC_DUTY(msg.data.arg2));
          break;
      }
#elif DEVICE_TYPE == LIDAR
      // only one chennal, so work directly with it
      PPM1_ANGLE(CALC_DUTY(msg.data.arg2));
#endif 
      break;
    case CMD_BATTERY:
      // TODO: handle battary voltage here
      // dunno what to do!
      break;
    case CMD_POWER_SWITCH:
      // power switch!
#if DEVICE_TYPE == UNIVERSAL
      switch(msg.data.arg1) { // N
        default:
        case PWR_1:
          (msg.data.arg2) ? POWER_OUTPUT_1_ON() : POWER_OUTPUT_1_OFF();
          break;
        case PWR_2:
          (msg.data.arg2) ? POWER_OUTPUT_2_ON() : POWER_OUTPUT_2_OFF();
          break;
        case DC_5V:
          (msg.data.arg2) ? DC_5V_ON() : DC_5V_OFF();
          break;
        case DC_12V:
          (msg.data.arg2) ? DC_12V_ON() : DC_12V_OFF();
          break;
      }
#elif DEVICE_TYPE == INTROS
      // INTROS pcb has only one optocoupler ouput, work directly with it 
      (msg.data.arg2) ? POWER_INTROS_ON() : POWER_INTROS_OFF();
#endif
      break;
    case CMD_UART_TRASNSLATOR_START:
      state_set(ST_FORWARDING_MODE);
      break;
    case CMD_UART_TRANSLATOR_STOP:
      state_set(ST_NORMAL_MODE);
      forwarding_finish();
      break;
  }
}

int main(void) {
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();
  SystemClock_Config();
  init_mcu();

  while (1) {
    switch(event_get()) {
      case EV_NONE:
      default:
        // do nothing (:
        break;
      case EV_USB_MESSAGE_RECEIVED:
        // process buffer and react
        process_routine();
        event_set(EV_NONE);
        break;
      case EV_UART_BUFFER_OVERFLOW:
        // handle this issue
      event_set(EV_NONE);
        break;
    }
    // END OF MAIN LOOP
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
uint8_t uart_buffer[UART_BUFFER_SIZE] = { 0 };
uint32_t b_index = 0;

//uint32_t cnt = 0x00;

void forwarding_finish(void) {
  static uint32_t i;
  if (b_index) {
    for (i = 0; i < FORWARDING_BUFFER_SIZE + 1; i++)
      base64_data[i] = '\0';
    base64_encode(uart_data, b_index, base64_data, FORWARDING_BUFFER_SIZE + 1);
    CDC_Transmit_FS(base64_data, FORWARDING_BUFFER_SIZE + 1);
    b_index = 0;
  }
}

#if DEVICE_TYPE != LIDAR
#if DEVICE_TYPE == UNIVERSAL
void USART1_IRQHandler(void)
#elif DEVICE_TYPE == INTROS
void USART2_IRQHandler(void)
#endif // DEVICE_TYPE == UNIVERSAL | INTROS
{
  if (state_get() == ST_FORWARDING_MODE) {
    if (b_index < FORWARDING_BUFFER_SIZE) {
      uart_data[b_index++] = UART->RDR;
    } else if (b_index == FORWARDING_BUFFER_SIZE) {
      b_index = 0;
      base64_encode(uart_data, FORWARDING_BUFFER_SIZE, base64_data, FORWARDING_BUFFER_SIZE + 1);
      CDC_Transmit_FS(base64_data, FORWARDING_BUFFER_SIZE + 1);
    }
  } else {
    uart_buffer[b_index] = UART->RDR;
    b_index = (b_index < UART_BUFFER_SIZE) ? b_index + 1 : 0;
  }
}
#endif /* LIDAR */

/****************************************************************/

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  while(1);
}
#endif /* USE_FULL_ASSERT */



/* SNIPETS
// for (uint32_t i = 0; i < 180; i++) {
//   for (uint32_t j = 0; j < 1e5; j++) {
//     asm("nop");
//   }
//   PPM1_ANGLE(i);PPM2_ANGLE(i);
//   PPM3_ANGLE(i);PPM4_ANGLE(i);
// }
//SEND_UART(x);
*/

//        index = 0;
//        SEND_UART(msg.data.str[index]);
//        while (msg.data.str[index + 1] != '\0') {
//          index++;
//          SEND_UART(msg.data.str[index]);
//        }
//        SEND_UART('\n');
//        SEND_UART('\r');
    // for (uint32_t i = 0; i < 1e5; i++)
    //   asm("nop");
//    uint8_t cmd[] = "chrns.com";
    //CDC_Transmit_FS(cmd, 9);
    // if (flag) {
    //   nmea_parse_msg(UserRxBufferFS);
    //   index = 0;
    //   SEND_UART(msg.data.str[index]);
    //   while (msg.data.str[index++]) {
    //     SEND_UART(msg.data.str[index]);
    //   }
    //   SEND_UART('\n');
    //   SEND_UART('\r');
    //   flag = 0; // reset
    // }
    // State ??