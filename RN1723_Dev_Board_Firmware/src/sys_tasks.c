/*******************************************************************************
 The Wifly configuration and http client application tasks

  Company:
    Microchip Technology Inc.

  File Name:
    sys_tasks.c

  Summary:
      The function that make up the main application tasks are defined in this file.

  Description:`
     The functions that make up the main http client application are defined in this file.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "../include/app.h"
#include "../include/HardwareProfile.h"
#include "../include/console.h"
#include "../include/Tick.h"

APP_DATA app;
BOOL RN_UartCmdMode = FALSE;
BOOL PIC32_ConsoleMode = FALSE;

/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:

*/
void SYS_Initialize ( void* data )
{
    mJTAGPortEnable(0);

    // Enable multi-vectored interrupts
    INTEnableSystemMultiVectoredInt();

    // Enable optimal performance
    SYSTEMConfigPerformance(GetSystemClock());
    // Use 1:1 CPU Core:Peripheral clocks
    mOSCSetPBDIV(OSC_PB_DIV_1);

    // Initialize the UART Console
    InitConsole(WIFLY_BAUD_RATE_FAST);
    clrscrConsole();
    homeConsole();


    // Initialize the RN-WiFly interface
    Initialize_WiFlyUART(WIFLY_BAUD_RATE_FAST);

    //Initialize GPIO9 Interrupt
    //INTSetVectorPriority(INT_VECTOR_INPUT_CAP(INPUT_CAPTURE3_INT_VECTOR), INT_PRIORITY_LEVEL_1);
    //INTSetVectorSubPriority(INT_VECTOR_INPUT_CAP(INPUT_CAPTURE3_INT_VECTOR), INT_SUB_PRIORITY_LEVEL_3);
    IEC0bits.IC3IE = 1;
    IPC3bits.IC3IP = 1;
    IPC3bits.IC3IS = 3;

    OpenCapture3(IC_ON | IC_FEDGE_RISE | IC_INT_1CAPTURE | IC_EVERY_RISE_EDGE);

    Init_Switches();

    // Initialize LED's
    LEDS_OFF();
    Init_LEDs();

    // Initialize the Timers
    TickInit();

    // Initialize the SPI EEPROM
    XEEInit();

    //Initialize the SPI Flash
    SPIFLASH_CS_IO = 1;
    SPIFLASH_CS_TRIS = 0;     // Drive SPI Flash chip select pin
    
    /* Configure System for Multi-vector Mode*/
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    /* Initialize the Application */
    App_Initialize();
}


/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    
*/
void SYS_Tasks(void)
{
    //While RN is not in the Uart Pass through mode run the application
    if(!RN_UartCmdMode & !PIC32_ConsoleMode)
    {
    /* Maintain the HTTPS Tasks */
    HTTPSTasks(&app.socket);

    /* Maintain the App Tasks*/
    AppTasks(&app);
    }

} 
