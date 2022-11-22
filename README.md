# Harmonizer

A system which utilizes two Upduino 3.1 iCE40UP5k FPGAs and a STM32L432KC microcontroller to harmonize with a microphone input.

The microcontroller is the "brains" of the operation, one of the FPGAs is used as a data middle-man, and the other FPGA is used as a FFT hardware accelerator.

The MCU takes a 10-bit sample from an analog input at a rate of 800Hz. It passes these samples into an FPGA via SPI, where the samples are aggregated.
Once 64 samples have been collected in the FPGA, it loads the other FPGA with the data, and then a Fast Fourier Transform is performed.
When the Transform is done, the output is sent back to the "middleman", and then returned to the MCU. 
There, the output is used to determine the fundamental frequency of the input, and calculations are done to determine the frequencies to harmonize.
