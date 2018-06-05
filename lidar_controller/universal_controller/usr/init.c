#include "init.h"

// local functions
static void init_rcc(void);
static void init_gpio(void);
static void init_ppm(void);
static void init_uart(void);
static void init_timeouter(void);

void init_mcu(void) {
  init_rcc();
  MX_USB_DEVICE_Init(); // USB
  init_gpio();
  init_ppm();
  init_uart();
  init_timeouter();
}

static void init_rcc(void) {
  // common part: USB
#if DEVICE_TYPE == UNIVERSAL
  RCC_TIM2();
  RCC_GPIOA();
  RCC_GPIOB();
  RCC_GPIOF();
  RCC_UART1();
  RCC_TIMEOUTER();
#elif DEVICE_TYPE == LIDAR
  RCC_TIM3();
  RCC_GPIOA();
#elif DEVICE_TYPE == INTROS
  RCC_GPIOA();
  RCC_UART2();
  RCC_TIMEOUTER();
#endif
}

static void init_gpio(void) {
  GPIO_InitTypeDef gpio;
  USB_POWER_INIT(gpio);

#if DEVICE_TYPE == UNIVERSAL
  POWER_OUTPUT_1_INIT(gpio);  POWER_OUTPUT_1_OFF();
  POWER_OUTPUT_2_INIT(gpio);  POWER_OUTPUT_2_OFF();
  DC_12V_INIT(gpio);          DC_12V_OFF();
  DC_5V_INIT(gpio);           DC_5V_OFF();
  // terminal
  TERMINAL_PA15(gpio);        TERMINAL_PA15_OFF();
  TERMINAL_PB3(gpio);         TERMINAL_PB3_OFF();
  TERMINAL_PB4(gpio);         TERMINAL_PB4_OFF();
  TERMINAL_PB5(gpio);         TERMINAL_PB5_OFF();
  TERMINAL_PB6(gpio);         TERMINAL_PB6_OFF();
  TERMINAL_PB7(gpio);         TERMINAL_PB7_OFF();
  TERMINAL_PB8(gpio);         TERMINAL_PB8_OFF();
  TERMINAL_PF0(gpio);         TERMINAL_PF0_OFF();
  // GPIOs: PA0-PA3
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER0)) | (GPIO_MODER_MODER0_1);
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER1)) | (GPIO_MODER_MODER1_1);
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER2)) | (GPIO_MODER_MODER2_1);
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER3)) | (GPIO_MODER_MODER3_1);
  GPIOA->AFR[0] |= (0x02 << 0)|(0x02 << 4);
  GPIOA->AFR[0] |= (0x02 << 8)|(0x02 << 12);
  // USART1
  GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODER6|GPIO_MODER_MODER7))\
                  | (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
  GPIOB->AFR[0] = (GPIOB->AFR[0] &~ (GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7));
#elif DEVICE_TYPE == LIDAR
  // nothing here
#elif DEVICE_TYPE == INTROS
  POWER_INTROS(gpio);
  // PA6 (TIM3_CH1, AF1) or PA7 (TIM3_CH2, AF1)
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER6)) | (GPIO_MODER_MODER6_1);
  GPIOA->AFR[0] |= (0x01 << 24);
  // USART2: PA2 (TX, AF1), PA3 (RX, AF1)
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER2|GPIO_MODER_MODER3))\
                  | (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
  GPIOA->AFR[0] |= (0x01 << 8)|(0x01 << 12);
#endif
}

static void init_ppm(void) {
#if DEVICE_TYPE == UNIVERSAL
  // TIM2: channels 1-4
  TIM2->PSC = PPM_TIM_PRESCALER;
  TIM2->ARR = PPM_TIM_PERIOD;

  TIM2->CCR1 = MINIMAL_DUTY;
  TIM2->CCR2 = MINIMAL_DUTY;
  TIM2->CCR3 = MINIMAL_DUTY;
  TIM2->CCR4 = MINIMAL_DUTY;
  
  TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE; // PA0
  TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE; // PA1
  TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE; // PA2
  TIM2->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE; // PA3
  
  TIM2->CCER |= TIM_CCER_CC1E;
  TIM2->CCER |= TIM_CCER_CC2E;
  TIM2->CCER |= TIM_CCER_CC3E;
  TIM2->CCER |= TIM_CCER_CC4E;

  TIM2->BDTR |= TIM_BDTR_MOE;
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM2->EGR |= TIM_EGR_UG;
#elif DEVICE_TYPE == LIDAR
  // TIM3: channels 1(2)
  TIM3->PSC = PPM_TIM_PRESCALER;
  TIM3->ARR = PPM_TIM_PERIOD;

  TIM3->CCR1 = MINIMAL_DUTY;
  
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
  TIM3->CCER |= TIM_CCER_CC1E;
  TIM3->BDTR |= TIM_BDTR_MOE;
  TIM3->CR1 |= TIM_CR1_CEN;
  TIM3->EGR |= TIM_EGR_UG;
#elif DEVICE_TYPE == INTROS
  // nothing here
#endif
}

static void init_uart(void) {
#if DEVICE_TYPE != LIDAR
  UART->BRR = 480000 / 96;
  UART->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
  UART->ICR |= USART_ICR_TCCF;
  NVIC_SetPriority(UART_IRQ, 1);
  NVIC_EnableIRQ(UART_IRQ);
#endif
// #if DEVICE_TYPE == UNIVERSAL
//   USART1->BRR = 480000 / 96;
//   USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
//   USART1->ICR |= USART_ICR_TCCF;
//   NVIC_SetPriority(USART1_IRQn, 1);
//   NVIC_EnableIRQ(USART1_IRQn);
// #elif DEVICE_TYPE == INTROS
//   USART2->BRR = 480000 / 96;
//   USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
//   USART2->ICR |= USART_ICR_TCCF;
//   NVIC_SetPriority(USART2_IRQn, 1);
//   NVIC_EnableIRQ(USART2_IRQn);
// #endif
}

void init_timeouter(void) {
  // 10 ms => 48 MHz / 48000 => 1000 => 1 ms
  TIMEOUTER_TIM->PSC = TIMEOUTER_PRESCALER;
  TIMEOUTER_TIM->ARR = TIMEIYTER_PERIOD;
  TIMEOUTER_TIM->DIER |= TIM_DIER_UIE;
  TIMEOUTER_TIM->CR1 = TIM_CR1_ARPE; // | TIM_CR1_CEN

  NVIC_SetPriority(TIM17_IRQn, 1);
  NVIC_EnableIRQ(TIM17_IRQn);
}

/* ****************************************************** */
void _Error_Handler(char *file, int line) {
  while(1);
}
