// STM32L432KC_GPIO.h
// Header for GPIO functions

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>
#include "STM32L432KC_ADC.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

//configure ADC clock
void initADC(){
    //turn on CLK to ADC
    RCC->AHB2ENR |= _VAL2FLD(RCC_AHB2ENR_ADCEN, 0x1);

    //option 1 from p.378 (input clock from specific clock source)
    ADC1_COMMON->CCR |= _VAL2FLD(ADC_CCR_CKMODE, ADC_CCR_CKMODE_0);
    
    // Set ADC_SQR1_SQ1 to channel 10
    ADC1->SQR1 |= (10 << ADC_SQR1_SQ1_Pos);
    //TODO: Add clk prescalar modification code here

    //TODO: Add ADC sample time modification code here

    //enable continuous mode for ADC
    //ADC1->CFGR |= _VAL2FLD(ADC_CFGR_CONT, 0b1);

    //EXTEN = 0x0 on reset

    //set overrun mode 1 (ADC_DR register overwritten with last conversion result when overrun is detected)
    //ADC1->CFGR |= _VAL2FLD(ADC_CFGR_OVRMOD, 0b1);

}

void calibrateADC(){
    //Ensure ADC not in deep power down
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    //while (_FLD2VAL(ADC_CR_DEEPPWD,ADC1->CR));
    //enable ADC Voltage regulator
    ADC1->CR |= _VAL2FLD(ADC_CR_ADVREGEN, 0b1);
    
    //wait for ADVREGEN enable
    //while (!_FLD2VAL(ADC_CR_ADVREGEN, ADC1->CR));
    
    //clear ADCALDIF
    ADC1->CR &= ~(ADC_CR_ADCALDIF);
    //write 1 to calibrate ADC
    ADC1->CR |= _VAL2FLD(ADC_CR_ADCAL, 0b1);
    
    //wait for calibration to finish
    while (_FLD2VAL(ADC_CR_ADCAL, ADC1->CR));

}

void runADC(){
    //enable continuous mode for ADC
    ADC1->CFGR |= _VAL2FLD(ADC_CFGR_CONT, 0b1);

    //EXTEN = 0x0 on reset

    //set overrun mode 1 (ADC_DR register overwritten with last conversion result when overrun is detected)
    ADC1->CFGR |= _VAL2FLD(ADC_CFGR_OVRMOD, 0b1);

    //clear ADRDY by writing 1
    ADC1->ISR |= _VAL2FLD(ADC_ISR_ADRDY, 0b1);

    //set ADEN=1
    ADC1->CR |= _VAL2FLD(ADC_CR_ADEN, 0b1);

    //wait until ADRDY = 1
    while(!_VAL2FLD(ADC_ISR_ADRDY, ADC1->ISR));

    //set ADSTART to start ADC conversions
    ADC1->CR |= _VAL2FLD(ADC_CR_ADSTART, 0b1);
}

uint16_t readADC(){
    //wait for EOS flag to go high
    while (!_FLD2VAL(ADC_ISR_EOS, ADC1->ISR));

    //cast ADC DR to 32 bit integer pointer, dereference, and obtain the rightmost 16 bits
    uint16_t result = *((uint32_t*) &(ADC1->DR)) & 0xFF;
  
    //set overrun flag low by writing 1
    ADC1->ISR |= _VAL2FLD(ADC_ISR_OVR, 0b1);
    return result;

}


