/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

#include <stdio.h>
#include <stdlib.h>

#include "STM32L432KC.h"

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);

  // Configure PA0 to Analog
  pinMode(PA0, GPIO_ANALOG);

  // Enable RCC to ADC
  RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

  // Adjust ADC1_CCR to get Sysclk
  ADC1_COMMON->CCR |= ADC_CCR_CKMODE_0;

  // Set ADC_SQR1_SQ1 to channel 5
  ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);

  // Ensure DEEPPWD = 0 and ADVREGEN = 1
  ADC1->CR &= ~(ADC_CR_DEEPPWD);
  ADC1->CR |= ADC_CR_ADVREGEN;

  // Clear ADCALDIF and write 1 to ADCAL
  ADC1->CR &= ~(ADC_CR_ADCALDIF);
  ADC1->CR |= ADC_CR_ADCAL;

  // Wait for ADCAL to read 0
  while (ADC1->CR & ADC_CR_ADCAL);

  // Set ADEN = 1
  ADC1->CR |= ADC_CR_ADEN;

  // Wait for ADRDY = 1
  while (!(ADC1->ISR & ADC_ISR_ADRDY));

  // Clear ADRDY by writing 1
  ADC1->ISR |= ADC_ISR_ADRDY;

  // Set SMP0 to 111 in ADC_SMPR1
  // ADC1->SMPR1 |= ADC_SMPR1_SMP0;

  // Set CONT = 1
  ADC1->CFGR |= ADC_CFGR_CONT;

  // Set ADSTART = 1
  ADC1->CR |= ADC_CR_ADSTART;

  // Loop:
  while (1) {
    // Wait for EOC == 1
    while (!(ADC1->ISR & ADC_ISR_EOC));
    // Read from ADC_DR
    uint16_t curr_data = (uint16_t) ADC1->DR;
  }

}

/*************************** End of file ****************************/
