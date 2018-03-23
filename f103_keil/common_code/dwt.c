#include "dwt.h" 



uint32_t delta(uint32_t t0, uint32_t t1);




// DWT functions 
//------------------------------------------------------------------------------
void initDWT(void)
{
  //разрешаем использовать счётчик
  SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
   //обнуляем значение счётного регистра
  DWT_CYCCNT  = 0;
   //запускаем счётчик
  DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk; 
}

__inline uint32_t delta(uint32_t t0, uint32_t t1)
{
  return (t1 - t0); 
}

void delay_us(uint32_t us)
{
  uint32_t t0 =  DWT->CYCCNT;
  uint32_t us_count_tic =  us * (SystemCoreClock/1000000);
  while (delta(t0, DWT->CYCCNT) < us_count_tic) ;
}


void delay_ms(uint32_t ms)
{
  uint32_t t0 =  DWT->CYCCNT;
  uint32_t ms_count_tic =  ms * (SystemCoreClock/1000);
  while (delta(t0, DWT->CYCCNT) < ms_count_tic) ;
}
//------------------------------------------------------------------------------
