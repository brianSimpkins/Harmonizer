// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"

void enable_timers() {
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
  RCC->APB2ENR  |= RCC_APB2ENR_TIM15EN;
  RCC->APB2ENR  |= RCC_APB2ENR_TIM16EN;

}

void initTIM(TIM_TypeDef * TIMx){
  // Set prescaler to give 10 miscrosecond time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e5));

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR = (ms * 100);// Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

void init_musical_timer(TIM_TypeDef * TIMx) {
  TIMx->CCMR1 |= (0b0110 << TIM_CCMR1_OC1M_Pos); // Enable PWM mode 1

  TIMx->CCMR1 |= TIM_CCMR1_OC1PE; // Enable CCR preload 
  TIMx->CR1 |= TIM_CR1_ARPE; // Enable ARR preload
  TIMx->CCER |= TIM_CCER_CC1E; // Enable CCR 1 output
}

void play_note(TIM_TypeDef * TIMx, uint32_t freq) {
  TIMx->ARR = freq; // preload arr
  TIMx->CCR1 = freq / 2; // preload ccr

  TIMx->EGR |= 1; // force shadow buffer load
}