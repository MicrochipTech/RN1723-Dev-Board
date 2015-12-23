/*******************************************************************************
  The UART Receive Buffer FIFO Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    fifo.h

  Summary:
    Function prototypes for the PIC's Uart receive buffer  FIFO

  Description:
    When Uart's 1 receiver interrupts are enabled, the input bytes received are placed
    into a circular FIFO. The function prototypes for managing the FIFO are defined
    in this header file.
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



#ifndef __FIFO_H
#define __FIFO_H


// *****************************************************************************
// Section: Included Files
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************
/*  These constants define the size (128) and tracking level indexes for the
 *  FIFO
*/
#define FIFO_MAX_ENTRIES     128
#define RN_FIFO_MAX_ENTRIES   512

#define FIFO_IDX_MASK        (FIFO_MAX_ENTRIES-1)
#define RN_FIFO_IDX_MASK     (RN_FIFO_MAX_ENTRIES-1)

#define FIFO_HIGH_WATER_MARK 120
#define RN_FIFO_HIGH_WATER_MARK 246

#define FIFO_LOW_WATER_MARK  96
#define RN_FIFO_LOW_WATER_MARK  26

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/*  These function prototypes are for the number of bytes in the fifo, the fifo
 *  write, read, and initialization functions respectively.
*/
unsigned char   fifoCount();
void            fifoWr(unsigned char);
unsigned char   fifoRd();
void            fifoInit();

#endif