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
#include <stm32l432xx.h>

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void) {
    configureClock();
    //turn on clk to GPIOA
    RCC->AHB2ENR |= _VAL2FLD(RCC_AHB2ENR_GPIOAEN,0b1);

    //set pin A0 to analog mode (this is done on RESET)
    //GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODE0_0,0b0);

    //configure channel 5 for adc regular sequence thing
    ADC1->SQR1 |= _VAL2FLD(ADC_SQR1_SQ1,0x5);
    initADC();
    calibrateADC();
    runADC();
    while (1){
      uint16_t val = readADC();
    }
  
}

/*************************** End of file ****************************/
