#include "init.h"

// local functions
static void init_rcc(void);
static void init_gpio(void);

void init_mcu(void) {
  init_rcc();
  MX_USB_DEVICE_Init(); // USB
  init_gpio();

}

static void init_rcc(void) {
  // common part: USB
  //RCC_GPIOA();
  RCC_GPIOB();
  //RCC_UART2();

}

static void init_gpio(void) {
  GPIO_InitTypeDef gpio;
  POWER_INTROS(gpio);
  
}


/* ****************************************************** */
void _Error_Handler(char *file, int line) {
  while(1);
}
