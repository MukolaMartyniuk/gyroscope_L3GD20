# Gyroscope L3GD20 SPI

## Description
An application for working with the L3GD20 gyroscope. The STM32F411E DISCO board was used for work. The SPI interface and the HAL library were used for data transfer.

## Hardware requirements
+ STM32F411E DISCO
+ Gyroscope L3GD20

## Software requirements
+ Programming language C
+ Keil uVision 5

## Instructions for use
To work with the application, you need to download it and open gyroscope_L3GD20.uvprojx in Keil uVision. To check whether the gyroscope on the board is correct, use the function `uint8_t L3GD20_ReadWHOAMI(void)`.
If the gyroscope is correct, the variable `who_am_i` will be set to 0xD4.
