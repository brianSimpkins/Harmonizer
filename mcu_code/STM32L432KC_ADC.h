// STM32L432KC_GPIO.h
// Header for GPIO functions

#ifndef STM32L4_ADC_H
#define STM32L4_ADC_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
void initADC();
void calibrateADC();
void runADC();
uint16_t readADC();


#endif