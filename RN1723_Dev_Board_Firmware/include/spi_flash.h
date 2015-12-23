/* 
 * File:   spi_flash.h
 * Author: c13839
 *
 * Created on September 23, 2015, 2:16 PM
 */

#ifndef SPI_FLASH_H
#define	SPI_FLASH_H

#include "HardwareProfile.h"

void SPIFlash_Read(BYTE *dest, BYTE *addr , BYTE count);
void SPIFlash_Write(BYTE *src, BYTE *addr, BYTE count);
void SPIFlash_GetID(BYTE *dest, BYTE *deviceId);
void SPIFlash_Init(void);
void SPIPut4(BYTE data);
BYTE SPIGet4(void);

#endif	/* SPI_FLASH_H */

