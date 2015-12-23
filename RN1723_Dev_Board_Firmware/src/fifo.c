/*******************************************************************************
  The UART Receive Buffer FIFO Interface  File

  Company:
    Microchip Technology Inc.

  File Name:
    fifo.c

  Summary:
    Functions for the PIC's Uart receive buffer  FIFO

  Description:
    When Uart's 1 receiver interrupts are enabled, the input bytes received are placed
    into a circular FIFO. The functions for managing the FIFO are defined
    in this  file.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#include <stdio.h>
#include "../include/fifo.h"

/* the spi bus fifo */
unsigned char fifo[FIFO_MAX_ENTRIES];
unsigned char rn_fifo[RN_FIFO_MAX_ENTRIES];

volatile unsigned char wrIdx;
volatile unsigned char rdIdx;

volatile unsigned short int rnwrIdx;
volatile unsigned short int rnrdIdx;

void fifoInit()
{
    wrIdx = 0;
    rdIdx = 0;
}

void rnFifoInit()
{
    rnwrIdx = 0;
    rnrdIdx = 0;
}

// fifoCount() - return the number of entries in the FIFO

unsigned char fifoCount()
{
    return wrIdx - rdIdx;
}

unsigned short int rnFifoCount()
{
    return rnwrIdx - rnrdIdx;
}

void fifoWr(unsigned char c)
{
    fifo[wrIdx++&FIFO_IDX_MASK] = c;
    if (fifoCount() > FIFO_HIGH_WATER_MARK)
    {
        //FIFO_SIGNAL_FULL;
    }
}

void rnFifoWrite(unsigned char c)
{
    rn_fifo[rnwrIdx++ & RN_FIFO_IDX_MASK] = c;
    if (rnFifoCount() > RN_FIFO_HIGH_WATER_MARK)
    {
        //FIFO_SIGNAL_FULL;
    }

}

unsigned char fifoRd()
{
    if (fifoCount() < FIFO_LOW_WATER_MARK)
    {
       // FIFO_SIGNAL_NOT_FULL;
    }
    return fifo[rdIdx++&FIFO_IDX_MASK];
}

unsigned char rnFifoRead()
{
    if (rnFifoCount() < RN_FIFO_LOW_WATER_MARK)
    {
       // FIFO_SIGNAL_NOT_FULL;
    }
    return rn_fifo[rnrdIdx++&RN_FIFO_IDX_MASK];
}


