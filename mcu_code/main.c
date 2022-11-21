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

char output_ready = 1;


int main(void) {
  configureFlash();
  configureClock();
  enable_timers();

  gpioEnable(GPIO_PORT_A);

  // configure PA5 to Analog
  pinMode(PA5, GPIO_ANALOG);

  // channel 10 correlates to PA5
  adc_init(10);

  // turn on the peripheral
  adc_start();

  // loop:
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

    play_note(max_frequency);

  }

}

/*************************** End of file ****************************/
