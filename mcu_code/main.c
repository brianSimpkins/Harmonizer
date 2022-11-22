/*********************************************************************
*                    Brian Simpkins and Kevin Kim                    *
*                        The Embedded Experts                        *
**********************************************************************

File    : main.c
Purpose : Interface with FPGA and Microphone, control the speakers

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "STM32L432KC.h"


uint16_t samples[64];
uint16_t output_real[32];
uint16_t output_imag[32];

uint16_t volume;

bool output_ready = false;


int main(void) {
  configureFlash();
  configureClock();

  enable_timers();

  // timer 2, 15, 16
  init_music_timers();

  // timer 2 - channel 1 - PA15 AF1

  // timer 15 - channel 1 - PA2 AF14

  // timer 16 - channel 1 - PA6 AF14

  
  // configure PA5 to Analog
  gpioEnable(GPIO_PORT_A);
  pinMode(PA5, GPIO_ANALOG);
  // channel 10 correlates to PA5
  adc_init(10);
  // turn on the peripheral
  adc_start();

  // loop: should be replaced with TIM6 interrupt
  for(int i = 0; i < 64; ++i) {
    samples[i] = adc_read();
  }




  // determine fundamental frequency
  if(output_ready) {

    int32_t max_magnitude = 0;
    int32_t max_index = 0;

    // loop through output
    // 0th index is meaningless for us
    for(int i = 1; i < 32; ++i) {
      int32_t magnitude = (output_real[i] * output_real[i]) + 
                          (output_imag[i] * output_imag[i]);

      if(magnitude > max_magnitude) {
        max_magnitude = magnitude;
        max_index = i;
      }
    }

    // assume 800hz sample rate, 64-point fft
    // freq = i * 400 / 32
    // ms = 1 / freq
    // ms = 32 / i * 400
    int32_t max_frequency = (int) 32 / (max_index * 400);

    set_volume(volume);

    play_note(max_frequency);

  }


  // Enable interrupts globally
    __enable_irq();

    // 1. Configure mask bit
    EXTI->IMR1 |= EXTI_IMR1_IM2;
    // 2. Disable rising edge trigger
    EXTI->RTSR1 &= ~(EXTI_RTSR1_RT2);
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= EXTI_FTSR1_FT2;
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI2_IRQn);

}


void TIM6_IRQHandler(void){
    // Check that the timer is what triggered our interrupt
    if (TIM6->SR & (TIM_SR_CC1IF_Msk)) {

        TIM6->SR &= ~(0x1); // Clear UIF
        TIM6->SR &= ~(TIM_SR_CC1IF); // clear interrupt flag?

        // Then toggle the LED
        togglePin(LED_PIN);

        TIM6->CNT = 0;      // Reset count
    }
}

/*************************** End of file ****************************/
