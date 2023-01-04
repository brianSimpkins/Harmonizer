# Harmonizer

A system which utilizes an Upduino 3.1 iCE40UP5k FPGA and a STM32L432KC microcontroller to harmonize with a microphone input.

The microcontroller is the "brains" of the operation, and the FPGA contains a hardware implementation of a 32-bit 64-point Fast Fourier Transform, plus some additional modules on top to assist with data transfer.

The MCU takes a 10-bit sample from the microphone input at a rate of 800Hz. It passes these samples into an FPGA via SPI, where the samples are aggregated.
Once 64 samples have been collected in the FPGA, it loads the input data in the the embedded ram blocks, and then a Fast Fourier Transform is performed.
When the Transform is done, the output is saved in a buffer, and then returned to the MCU upon the next series of SPI transmissions.
There, the output is used to determine the fundamental frequency of the input, and calculations are done to determine the frequencies to harmonize. Timers pass square waves of the appropriate frequencies through low-pass filters and amplifiers, and into a set of 8-ohm speakers.
