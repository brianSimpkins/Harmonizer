// STM32L432KC_ADC.c
// Source code for ADC functions

#include "STM32L432KC_RCC.h"
#include "STM32L432KC_ADC.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// ADC Helper Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void adc_init(char channel_num) {
  // Enable RCC to ADC
  RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

  // Adjust ADC1_CCR to get Sysclk
  ADC1_COMMON->CCR |= ADC_CCR_CKMODE_0;

  // Set ADC_SQR1_SQ1 to channel 10
  ADC1->SQR1 |= (channel_num << ADC_SQR1_SQ1_Pos);

  // Set resolution to 10-bit
  ADC1->CFGR |= (01 << ADC_CFGR_RES_Pos);

  // Set 512 offset
  ADC1->OFR1 |= ADC_OFR1_OFFSET1_EN;
  ADC1->OFR1 |= _VAL2FLD(ADC_OFR1_OFFSET1_CH, channel_num);
  ADC1->OFR1 |= _VAL2FLD(ADC_OFR1_OFFSET1, (502 << 2));

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
}

void adc_start() {
  // Set CONT = 1
  ADC1->CFGR |= ADC_CFGR_CONT;

  // Set OVRMOD = 1
  ADC1->CFGR |= ADC_CFGR_OVRMOD;

  // Set ADSTART = 1
  ADC1->CR |= ADC_CR_ADSTART;
}

int16_t adc_read() {
  // Acknowledge overrun
  ADC1->ISR |= ADC_ISR_OVR;
  // Wait for EOC == 1
  while (!(ADC1->ISR & ADC_ISR_EOC));
  // Read from ADC_DR
  int16_t curr_data = (int16_t) ADC1->DR;

  return(curr_data);
}