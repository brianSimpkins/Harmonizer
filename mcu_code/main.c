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
  init_musical_timer(TIM2);
  init_musical_timer(TIM15);
  init_musical_timer(TIM16);

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
    // sec = 1 / freq
    // sec = 32 / i * 400
    // 10 microsec = 100,000 * (32 / i * 400)
    int32_t max_period = (int) 1000000 * (32 / ((double) (max_index * 400));

    // set_volume(volume); how??

    play_note(TIM2, max_period);

    play_note(TIM15, (int) max_period * 3 / 2);

    play_note(TIM16, max_period * 2);

  }

}

/*************************** End of file ****************************/
