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

  

  // Loop:
  while (1) {
    
  }

}

/*************************** End of file ****************************/
