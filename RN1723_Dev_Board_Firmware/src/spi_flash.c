/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/
#include "../include/HardwareProfile.h"
#include "../include/spi_flash.h"


#define SPI_READ            0x03
#define SPI_SECTOR_ERASE    0x20
#define SPI_BLOCK_ERASE     0x52
#define SPI_CHIP_ERASE      0x60
#define SPI_BYTE_PROGRAM    0x02
#define SPI_AUTO_ADDRESS_INC 0xAF
#define SPI_READ_STATUS_REG 0x05
#define SPI_WRITE_ENABLE    0x06
#define SPI_READ_ID         0x90
#define SPI_WRITE_DISABLE   0x04

#define READ_MANUFACTURER_ID 0xBF
#define READ_DEVICE_ID      0x49


#define PROPER_SPICON4  (_SPI4CON_ON_MASK | _SPI4CON_CKE_MASK | _SPI4CON_MSTEN_MASK)

#define SPI_ON_BIT      (SPIFLASH_SPICON1bits.ON)


/*********************************************************************
* Function:         SPIFlash_Read(BYTE *dest, BYTE *addr, BYTE count)
*
* PreCondition:     none
*
* Input:            BYTE *dest  - Destination buffer.
*                   BYTE *addr   - Address to start reading from.
*                   BYTE count  - Number of bytes to read.
*
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads bytes from the EEProm and puts
*                   them in a buffer.
*                    
*
* Note:			    
**********************************************************************/   
void SPIFlash_Read(BYTE *dest, BYTE *addr, BYTE count)
{
    SPIFLASH_CS_IO = 0;
    SPIPut4(SPI_READ);
    SPIPut4(*addr);
    SPIPut4(*addr++);
    SPIPut4(*addr++);
    while( count )
    {
        *dest++ = SPIGet4();
        count--;
    }
   SPIFLASH_CS_IO = 1;
} 

/*********************************************************************
* Function:         void SPIFlash_Write(BYTE *src, BYTE *addr, BYTE count)
*
* PreCondition:     none
*
* Input:            BYTE *src  - Source buffer.
*                   BYTE *addr   - Address to start reading from.
*                   BYTE count  - Number of bytes to read.
*
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads bytes from the EEProm and puts
*                   them in a buffer.
*                    
*
* Note:			    
**********************************************************************/   
void SPIFlash_Write(BYTE *src, BYTE *addr, BYTE count)
{
    SPIFLASH_CS_IO = 0;
    SPIPut4(SPI_WRITE_ENABLE);
    SPIPut4(SPI_AUTO_ADDRESS_INC);
    SPIPut4(*addr);
    SPIPut4(*addr++);
    SPIPut4(*addr++);
    while( count )
    {
        SPIPut4(*src++);
        count--;
        if(count != 0) SPIPut4(SPI_AUTO_ADDRESS_INC);
    }
    SPIPut4(SPI_WRITE_DISABLE);
    SPIFLASH_CS_IO = 1;
} 

/*********************************************************************
* Function:         void SSTGetID(BYTE dest)
*
* PreCondition:     none
*
* Input:            BYTE dest  - Destination buffer.
*                   
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads bytes from the SST Flash and puts
*                   them in a buffer.
*                    
*
* Note:			    
**********************************************************************/   

void SPIFlash_Init(void) {
    SPIFLASH_CS_IO = 1;
    SPIFLASH_CS_TRIS = 0;     // Drive SPI EEPROM chip select pin

    SPIFLASH_SCK_TRIS = 0;    // Set SCK pin as an output
    SPIFLASH_SDI_TRIS = 1;    // Make sure SDI pin is an input
    SPIFLASH_SDO_TRIS = 0;    // Set SDO pin as an output


    #if defined(__C30__)
        SPIFLASH_SPICON1 = PROPER_SPICON4; // See PROPER_SPICON1 definition above
        SPIFLASH_SPICON2 = 0;
        SPIFLASH_SPISTAT = 0;    // clear SPI
        SPIFLASH_SPISTATbits.SPIEN = 1;
    #elif defined(__C32__)
        SPIFLASH_SPIBRG = (GetPeripheralClock()-1ul)/2ul/FLASH_MAX_SPI_FREQ;
        SPIFLASH_SPICON1 = PROPER_SPICON4;
    #elif defined(__18CXX)
        SPIFLASH_SPICON1 = PROPER_SPICON4; // See PROPER_SPICON1 definition above
        SPIFLASH_SPISTATbits.CKE = 1;     // Transmit data on rising edge of clock
        SPIFLASH_SPISTATbits.SMP = 0;     // Input sampled at middle of data output time
    #endif
}

/*********************************************************************
* Function:         void SPIPut4 (BYTE v)
*
* PreCondition:     none
*
* Input:            BYTE data
*                   
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine writes to flash
*                    
*
* Note:			    
**********************************************************************/   
void SPIPut4(BYTE v) {
    BYTE tmp;

    SpiChnPutC(SPI_CHANNEL4, v);
    tmp = SpiChnGetC(SPI_CHANNEL4);
}
/*********************************************************************
* Function:         BYTE SSTGet4(void)
*
* PreCondition:     none
*
* Input:            None
*                   
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads bytes from the SST Flash and returns
*                    
*
* Note:			    
**********************************************************************/   
BYTE SPIGet4(void) {
    BYTE data;

    SpiChnPutC(SPI_CHANNEL4, 0x00);
    data = SpiChnGetC(SPI_CHANNEL4);

    return data;
}
/*********************************************************************
* Function:         BYTE SPIFlash_GetID(unit8_t *dest)
*
* PreCondition:     none
*
* Input:            None
*                   
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads ID from the SST Flash and returns
*                    
*
* Note:			    
**********************************************************************/   
void SPIFlash_GetID(BYTE *dest, BYTE *deviceId)
{
    SPIFLASH_CS_IO = 0;
    SPIPut4(SPI_READ_ID);
    SPIPut4(0x00);
    SPIPut4(0x00);
    SPIPut4(0x00);
    *dest = SPIGet4();
    *deviceId = SPIGet4();
    SPIFLASH_CS_IO = 1;
} 
