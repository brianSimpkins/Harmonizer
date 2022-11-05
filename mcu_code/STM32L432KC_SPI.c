// STM32L432KC_SPI.c
// Brian Simpkins
// bsimpkins@hmc.edu
// 10/8/2022
// Initialize and control the SPI peripheral on the MCU

#include "STM32L432KC_SPI.h"

void initSPI(int br, int cpol, int cpha) {
  // baud rate
  SPI1->CR1 &= ~SPI_CR1_BR;
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);
  // cpol
  SPI1->CR1 &= ~SPI_CR1_CPOL;
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);
  // cpha
  SPI1->CR1 &= ~SPI_CR1_CPHA;
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);
  // enable ssm
  SPI1->CR1 &= ~SPI_CR1_SSM;

  SPI1->CR2 |= SPI_CR2_SSOE;
  // dataframe size to 8
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 7);
  // FIFO reception threshold
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1);

  // controller config
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1);
  // enable SPI1
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 1);
}

char spiSendReceive(char send) {

  // Wait for the send register to be empty
  while (!(SPI1->SR & SPI_SR_TXE));
  
  // Transmit the character over SPI
  *((volatile char *) (&SPI1->DR)) = send;

  // Wait until the recieve register is full
  while ((SPI1->SR) & SPI_SR_BSY);

  // Return the bottom 8 bits of the data register
  return (char) (SPI1->DR);
}