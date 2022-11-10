// STM32L432KC_ADC.h
// Header for ADC functions

#ifndef STM32L4_ADC_H
#define STM32L4_ADC_H

#include <stdint.h>
#include <stm32l432xx.h>

///////////////
// Functions //
///////////////

void adc_init(int channel_num);

void adc_start();

uint16_t adc_read();

#endif