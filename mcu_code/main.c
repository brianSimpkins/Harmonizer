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

  // Configure PA5 to Analog
  pinMode(PA5, GPIO_ANALOG);

  uint16_t samples[128];


  adc_init(10);

  adc_start();

  // Loop:
  for(int i = 0; i < 128; ++i) {
    samples[i] = adc_read();
  }

}

/*************************** End of file ****************************/
