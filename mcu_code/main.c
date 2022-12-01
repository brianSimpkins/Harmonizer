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


int16_t samples[64];
int16_t output_real[32];
int16_t output_imag[32];

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


  /* set up ADC */
  // configure PA5 to Analog
  gpioEnable(GPIO_PORT_A);
  pinMode(PA5, GPIO_ANALOG);
  // channel 10 correlates to PA5
  adc_init(10);
  // turn on the peripheral
  adc_start();


  /* set up spi with port B */
  initSPI(1, 0, 0);


  /* set up sampling timer */
  initTIM(TIM6);
  // 800 hz with 10 microsecond time base
  // equals 1/800 * 100,000 cycles
  TIM6->ARR = (uint16_t) 100000 / 800;

  // loop: should be replaced with TIM6 interrupt
  for(uint8_t i = 0; i < 64; ++i) {
    TIM6->SR &= ~(0x1); // Clear UIF
    TIM6->CNT = 0;      // Reset count

    // get a sample from the adc
    samples[i] = (adc_read() - 0x100);

    // insert output into one of the arrays
    uint8_t output_index = i >> 1;

    if(i % 2 == 1) {
      output_real[output_index] = spiSendReceive(samples[i]);
    } else {
      output_imag[output_index] = spiSendReceive(samples[i]);
    }
    
    // wait until the timer is done before sampling again
    if(i < 63) while(!(TIM6->SR & 1)); // Wait for UIF to go high
  }

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
  int32_t fundamental_frequency = (int32_t) (max_index * 400 / 32);

  // if average amplitude is high enough, play note
  play_note(TIM2, fundamental_frequency);

  play_note(TIM15, (int) fundamental_frequency * 3 / 2);

  play_note(TIM16, fundamental_frequency * 2);

  }

  
  // // Enable interrupts globally
  //   __enable_irq();
    
   
  //   // 1. Configure mask bit
  //   EXTI->IMR1 |= EXTI_IMR1_IM2;
  //   // 2. Disable rising edge trigger
  //   EXTI->RTSR1 &= ~(EXTI_RTSR1_RT2);
  //   // 3. Enable falling edge trigger
  //   EXTI->FTSR1 |= EXTI_FTSR1_FT2
  //   // 4. Turn on EXTI interrupt in NVIC_ISER
  //   NVIC->ISER[0] |= (1 << EXTI2_IRQn);

   


     
  //   //initialize TIM6
  //   initTIM(TIM6);

  //   // Turn on TIM6 interrupt in NVIC_ISER
  //   NVIC->ISER[0] |= (1 << TIM6_IRQn);
    
  //   //run at 800Hz
  //   delay_millis(TIM6, 1/800);


void TIM6_IRQHandler(void){
    // Check that the timer is what triggered our interrupt
    if (TIM6->SR & (TIM_SR_CC1IF_Msk)) {

        TIM6->SR &= ~(0x1); // Clear UIF
        TIM6->SR &= ~(TIM_SR_CC1IF); // clear interrupt flag?


        TIM6->CNT = 0;      // Reset count
    }
}

/*************************** End of file ****************************/
