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

int main(void) {
  configureFlash();
  configureClock();
  gpioEnable(GPIO_PORT_A);

  enable_timers();

  // timer 2, 15, 16
  init_musical_timer(TIM2);
  init_musical_timer(TIM15);
  init_musical_timer(TIM16);

  // timer 2 - channel 1 - PA5 AF1
  pinMode(PA5, GPIO_ALT);
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 1);

  // timer 15 - channel 1 - PA2 AF14
  pinMode(PA2, GPIO_ALT);
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL2, 14);

  // timer 16 - channel 1 - PA6 AF14
  pinMode(PA6, GPIO_ALT);
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL6, 14);

  /* set up ADC */
  // configure PA0 to Analog
  pinMode(PA0, GPIO_ANALOG);
  // channel 5 correlates to PA0
  adc_init(5);
  // turn on the peripheral
  adc_start();


  /* set up spi with port B */
  initSPI(5, 0, 0);


  /* set up sampling timer */
  initTIM(TIM6);
  // 800 hz with 10 microsecond time base
  // equals 1/800 * 100,000 cycles
  // make it slightly slower to account for extra cycles
  TIM6->ARR = 100000 / 802;

  togglePin(RST);
  togglePin(FULL_RST); // pulse full reset
  togglePin(FULL_RST);
  togglePin(RST);

  int32_t prev_ave_mag = 0;
  uint16_t prev_freq = 0;

  int16_t samples[64];

  char sampleChoose = 0;

  int16_t output_real[32];
  int16_t output_imag[32];

  while (1) {

    sampleChoose = !sampleChoose;

    int32_t curr_ave_mag = 0;

    for(uint8_t i = 0; i < 64; ++i) {

      TIM6->SR &= ~(0x1); // Clear UIF
      TIM6->CNT = 0;      // Reset count

      // get a sample from the adc
      samples[i] = adc_read();

      // insert output into one of the arrays
      uint8_t output_index = i >> 1;

      //if (sampleChoose) {
      //  sample = samples0[i];
      //} else {
      //  sample = samples1[i];
      //}

      if(samples[i] < 0) {
        curr_ave_mag -= samples[i];
      } else {
        curr_ave_mag += samples[i];
      }

      if(i % 2 == 1) {
        output_imag[output_index] = spiSendReceive(samples[i]);
      } else {
        output_real[output_index] = spiSendReceive(samples[i]);
      }
    
      // wait until the timer is done before sampling again
      if(i < 63) while(!(TIM6->SR & 1)); // Wait for UIF to go high

    }

    curr_ave_mag = curr_ave_mag / 64;

    if (prev_ave_mag > 40) {

      int32_t max_magnitude = 0;
      uint8_t max_index = 0;

      // loop through output
      // 0th index is meaningless for us
      for(uint8_t i = 1; i < 32; ++i) {
        int32_t magnitude = (output_real[i] * output_real[i]) + 
                            (output_imag[i] * output_imag[i]);

        if(magnitude > max_magnitude) {
          max_magnitude = magnitude;
          max_index = i;
        }
      }

      // assume 800hz sample rate, 64-point fft
      // freq = i * 400 / 32
      uint16_t fundamental_frequency = (int16_t) (max_index * 400 / 32);

      if (fundamental_frequency != prev_freq) {

        // if average amplitude is high enough, play note
        play_note(TIM2, fundamental_frequency);

        play_note(TIM15, fundamental_frequency * 3 / 2);

        play_note(TIM16, fundamental_frequency * 2);

        prev_freq = fundamental_frequency;

      }

    } else {

      play_note(TIM2, 0);

      play_note(TIM15, 0);

      play_note(TIM16, 0);

    }

    prev_ave_mag = curr_ave_mag;


    while( !digitalRead(DONE) );

    togglePin(RST);
    togglePin(RST);

  }

  }


/*************************** End of file ****************************/
