/*******************************************************************************
  The Console Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    console.h

  Summary:
    Function prototypes for the Explorer 16 U2/Console interface

  Description:
    When Uart U2 on the Explorer 16 receives data, those data bytes are displayed
    on the attached console.   The function prototypes for reading and writing data
    to the console are described in the header file.
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

#ifndef CONSOLE_U2_H
#define CONSOLE_U2_H
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "HardwareProfile.h"

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/*  These function prototypes are for the number of bytes in the fifo, the fifo
 *  write, read, and initialization functions respectively.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines Group
// *****************************************************************************
// *****************************************************************************
// init the console serial port (UART2, 9600, 8, N, 1, RTS/CTS )
void InitConsole(UINT32 baud);

// send a character to the serial port
void putConsole(char c);

// wait for a new character to arrive to the serial port
char getConsole(void);

// send a null terminated string to the serial port
void putsConsole(char *s);

// receive a null terminated string in a buffer of len char
char *getsnConsole(char *s, UINT16 n);

//print console menu options
void PrintConsoleMenu(void);

//pic32 interrupt handler
void ConsoleUartIntHandler(void);

//Factory Reset for RN
void _Factory_Reset_RN(void);



// useful macros
#define clrscrConsole() putsConsole("\x1b[2J")
#define homeConsole()   putsConsole("\x1b[H")
#define pcrConsole()    putConsole('\r')

#endif // CONSOLE_U2_H


