/*******************************************************************************
  The Console Interface functions definitions

  Company:
    Microchip Technology Inc.

  File Name:
    console.c

  Summary:
      The functions that are used to interface with the PIC's console are defined
      in this file.

  Description:
    The PIC's UART 2 is used as a console.  The functions that are used to initialize,
    read and write to the console are defined in this file.
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
#include "../include/console.h"
#include "../include/wifly_UART_drv.h"
#include "../include/HardwareProfile.h"
#include "../include/Tick.h"
#include "../include/GenericTypeDefs.h"
#include "../include/app.h"
#include "self_test.h"
#include <time.h>
// *****************************************************************************
// *****************************************************************************
// Section: File Scope Helper Macros
// *****************************************************************************
// *****************************************************************************

#define BACKSPACE   0x8         //ASCII backspace character code
#define CR          0x0D        //ASCII Carriage Return
#define LF          0x0A        //ASCII Line Feed
#define ESC         0x1B        //ASCII ESC


unsigned char consoleMsg[100] = {'\0'};
static unsigned char consoleState = 0;
struct tm timeNow;
extern long PIC32_time;

extern char moduleBaseURI[];
extern char hostName[];
extern BOOL PIC32_ConsoleMode;
extern BOOL RN_UartCmdMode;
extern BOOL self_TESTMode;
// *****************************************************************************
// Initialize the Console serial port (BAUDRATE)
// *****************************************************************************
void InitConsole(UINT32 baud)
{
    UARTConfigure(UART_CONSOLE, UART_ENABLE_PINS_TX_RX_ONLY | UART_ENABLE_HIGH_SPEED);
    UARTSetFifoMode(UART_CONSOLE, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_CONSOLE, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_CONSOLE, GetPeripheralClock(), baud);
#ifdef DEBUG_ALLOW_USER_INPUT
    INTEnable(INT_SOURCE_UART_RX(UART_CONSOLE), INT_ENABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART_CONSOLE), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_CONSOLE), INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_SOURCE_UART_TX(UART_CONSOLE), INT_DISABLED);//Disable TX interrupt!
#endif
    UARTEnable(UART_CONSOLE, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

}

void ConsoleTasks(void)
{
    
}


// *****************************************************************************
// Send a character to the Console serial port
// *****************************************************************************
void putConsole(char c)
{
    while (!UARTTransmitterIsReady(UART_CONSOLE));

    UARTSendDataByte(UART_CONSOLE, c);
}

// *****************************************************************************
// Wait for a new character to arrive on the Console serial port
// *****************************************************************************
char getConsole(void)
{
    char character;

    while(!UARTReceivedDataIsAvailable(UART_CONSOLE));

    character = UARTGetDataByte(UART_CONSOLE);

    return character;		// read the character from the receive buffer
}

// *****************************************************************************
// Send a null terminated string to the Console serial port
// *****************************************************************************
void putsConsole(char *s)
{
    while(*s)                   // loop until *s == '\0' the  end of the string
        putConsole(*s++);	// send the character and point to the next one

}


/*******************************************************************************
  Function:
    void ConsoleUartIntHandler(void)

  Remarks:
 UART 1 interrupt handler is set at priority level 2 with software context saving
 */
void ConsoleUartIntHandler(void)
{
    unsigned char byteReceived;

    // Is this an RX interrupt?
    if (INTGetFlag(INT_SOURCE_UART_RX(UART_CONSOLE)))
    {
        while (UARTReceivedDataIsAvailable(UART_CONSOLE) != 0)
        {
            byteReceived = UARTGetDataByte(UART_CONSOLE);
            //Console Interrupt - In Self Test Mode
            if(self_TESTMode == TRUE)
            {
                ConsoleMsgCallback_selfTest();
            }
            //Console Interrupt - RN1723 Command Mode
            else if(RN_UartCmdMode == 1)
            {
                //Exit console mode if 'ESC'
                if(byteReceived == ESC)
                {
                    RN_UartCmdMode = FALSE;
                    RN_DATA_MODE();
                    memset(consoleMsg, '\0', sizeof(consoleMsg));
                }
                else
                {
                    while(!UARTTransmitterIsReady(UART_WIFLY));
                    UARTSendDataByte(UART_WIFLY, byteReceived);
                }

             }
            else
            {
                PIC32_ConsoleMode = TRUE;
                putConsole(byteReceived);
                if(byteReceived == CR)
                {
                    PrintConsoleMenu();
                }
                else if(byteReceived == LF)
                {
                    //Ignore Line Feed
                }
                else if(byteReceived == BACKSPACE)
                {
                    consoleMsg[(strlen(consoleMsg)-1)] = '\0';
                }
                else if(byteReceived == ESC)
                {
                    PIC32_ConsoleMode = FALSE;
                    memset(consoleMsg, '\0', sizeof(consoleMsg));

                }
                else
                {
                    consoleMsg[strlen(consoleMsg)] = byteReceived;
                }

            }

        }

        // Clear the RX interrupt Flag
        INTClearFlag(INT_SOURCE_UART_RX(UART_CONSOLE));
    }

    // We don't care about the TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART_CONSOLE)) )
    {
      INTClearFlag(INT_SOURCE_UART_TX(UART_CONSOLE));
    }
}

/*******************************************************************************
  Function:
    void PrintConsoleMenu(void)

  Remarks:
    This routine prints the default console menu
    User if using terminal software application can provision RN module using the Console
*/

void PrintConsoleMenu(void)
{
    unsigned char byteReceived;
    static BOOL waitingResponse = FALSE;
    WIFLY_Result_t result;
    char  OutString[128];
    
  

    if((strlen(consoleMsg) <= 1) & (!waitingResponse))
    {
        byteReceived = consoleMsg[0];
        switch(byteReceived)
        {
            case 'a':
                putsConsole("\r\nEnter BAUDRATE\r\n");
                waitingResponse = TRUE;
                consoleState = 'a';
                break;

            case 'b':
                putsConsole("\r\nEnabling Pass-thru mode for RN1723\r\n");
                RN_UartCmdMode = TRUE;
                RN_COMMAND_MODE();
                {
                    unsigned long int timeout;
                    timeout = TickGet() + TICK_SECOND * 1;
                    while ((UINT32)TickGet() < timeout);
                }
                RN_DATA_MODE();

                //XEEWriteCompleteArray(USER_RN_UART_SETTING, (unsigned char *) &RN_UartCmdMode, sizeof(RN_UartCmdMode));
                break;
            case 'c':
                putsConsole("\r\nResetting System to RN1723 Factory Defaults\r\n");
                _Factory_Reset_RN();
                break;
            case '0':
                self_test();
            case '1':
                
                //putsConsole("\r\nProgramming RN Application Default Settings");
                RN_UartCmdMode = TRUE;
                Wifly_Default_Config(FALSE);        //web app mode turned off
                RN_RESET_LOW();
                {
                    //pulse of atleast 160 us
                    unsigned long int timeout = TickGet();
                    while(TickGet() < (timeout + (1 * TICK_MILLISECOND)));
                }

                RN_RESET_HIGH();
                RN_UartCmdMode = FALSE;
                putsConsole("\r\n>>");
                Reset();
                break;
            case '2':
                putsConsole("\r\nScanning for networks\r\n");
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("scan\r\n","END:",SEC(10))==Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                putsConsole("\r\n>>");
                break;
            case '3':
                putsConsole("\r\nEnter SSID\r\n");
                waitingResponse = TRUE;
                consoleState = 0x03;

                break;
            case '4':
                putsConsole("\r\nEnter DNS name\r\n");
                waitingResponse = TRUE;
                consoleState = 0x04;
                break;
            case '5':
                putsConsole("\r\nEnter Host IP Address\r\n");
                waitingResponse = TRUE;
                consoleState = 0x05;
                break;
            case '6':
                putsConsole("\r\nEnter destination port number\r\n");
                waitingResponse = TRUE;
                consoleState = 0x06;
                break;
            case '7':
                putsConsole("\r\nChange SYS Wake time\r\n");
                waitingResponse = TRUE;
                consoleState = 0x07;
                break;
            case '8':
                putsConsole("\r\nEnter RN1723 Dev Board Base-URI\r\n");
                waitingResponse = TRUE;
                consoleState = 0x08;
                break;

            case '9':
                putsConsole("\r\nEnter Date in mm/dd/yyyy format\r\n");
                waitingResponse = TRUE;
                consoleState = 0x09;
                break;


            default:
                putsConsole("\r\n\r\nChoose from the below options:\r\n");
                putsConsole("------------------------------------------");
                putsConsole("\r\na. Configure PIC32 to RN-UART BAUDRATE");
                putsConsole("\r\nb. Pass Terminal to RN-UART");
                putsConsole("\r\nc. Factory Reset System\r\n");
                putsConsole("\r\n1. Restore RN1723 Dev Board Default Settings");
                putsConsole("\r\n2. Scan for networks to join");
                putsConsole("\r\n3. Configure SSID, Passphrase");
                putsConsole("\r\n4. Change DNS Name");
                putsConsole("\r\n5. Change Host IP Address");
                putsConsole("\r\n6. Change Destination port");
                putsConsole("\r\n7. Change RN SYS Wake time");
                putsConsole("\r\n8. Change RN1723 Dev Board Base-URI");
                putsConsole("\r\n9. Enter Date and Time (Used for SSL Peer Validation)");

                putsConsole("\r\n\r\nPress 'ESC' to exit PIC32 Console Mode");
                putsConsole("\r\n>>");
                break;
        }

    }
    else
    {
        waitingResponse = FALSE;
        switch(consoleState)
        {
            case 'a':
                {
                    
                    RN_UartCmdMode = TRUE;
                    while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                    while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                    sprintf(OutString,"set uart baud %s\r\n", consoleMsg);
                    while(PutCMD_WiFlyUART(OutString,">",SEC(1))==Result_BUSY);
                    while(PutCMD_WiFlyUART("save\r\n", ">", SEC(1)) == Result_BUSY);
                    while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                    RN_UartCmdMode = FALSE;
                    consoleState = 0;
                }
                break;

            case 0x03:
                putsConsole("\r\nProgramming SSID...\r\n");
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                sprintf(OutString,"set wlan ssid %s\r\n",consoleMsg);
                while(PutCMD_WiFlyUART(OutString,"OK",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("save\r\n", "OK", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                
                putsConsole("\r\nEnter Passphrase\r\n");
                waitingResponse = TRUE;
                consoleState = 0x32;
                break;

            case 0x04:
                putsConsole("\r\nProgramming DNS...\r\n");
                if(strlen(consoleMsg) > 2)
                {
                    memset(hostName, '\0', 40);
                    strcpy(hostName, consoleMsg);
                    XEEWriteCompleteArray(USER_HOST_NAME, (char*) &hostName, 40);
                }
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                sprintf(OutString,"set dns name %s\r\n",consoleMsg);
                while(PutCMD_WiFlyUART(OutString,">",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("set ip host 0\r\n", ">", SEC(1))== Result_BUSY);
                while(PutCMD_WiFlyUART("save\r\n", ">", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                consoleState = 0;
               
                break;

            case 0x05:
                putsConsole("\r\nProgramming host IP...\r\n");
                if(strlen(consoleMsg) > 2)
                {
                    memset(hostName, '\0', 40);
                    strcpy(hostName, consoleMsg);
                    XEEWriteCompleteArray(USER_HOST_NAME, (char*) &hostName, 40);
                }
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                sprintf(OutString,"set ip host %s\r\n",consoleMsg);
                while(PutCMD_WiFlyUART(OutString,">",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("set dns name 0\r\n",">",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("save\r\n", ">", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                consoleState = 0;
               
                break;

            case 0x06:
                putsConsole("\r\nProgramming Destination port...\r\n");
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                sprintf(OutString,"set ip remote %s\r\n",consoleMsg);
                while(PutCMD_WiFlyUART(OutString,">",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("save\r\n", ">", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                consoleState = 0;
               
                break;
            
            case 0x07:
                putsConsole("\r\nProgramming sys wake time...\r\n");
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                sprintf(OutString,"set sys wake %s\r\n",consoleMsg);
                while(PutCMD_WiFlyUART(OutString,">",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("save\r\n", ">", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                consoleState = 0;
                break;

            case 0x08:
                putsConsole("\r\nProgramming Base-URI...\r\n");
                memset(moduleBaseURI, '\0', 40);
                strcpy(moduleBaseURI, consoleMsg);
                XEEWriteCompleteArray(USER_BASE_URI_ADDRESS, (char*) &moduleBaseURI, 40);
                consoleState = 0;
                break;

            case 0x09:
            {
                char *indexPtr;

                if(indexPtr = strchr(consoleMsg, '/'))
                {
                    unsigned char temp[4] = {"\0"};
                    
                    temp[0] = *(indexPtr-2);
                    temp[1] = *(indexPtr-1);
                   
                    timeNow.tm_mon = atoi(temp);
                    sprintf(OutString,"Month %d\r\n", timeNow.tm_mon);
                    putsConsole(OutString);

                    indexPtr = strchr(indexPtr+1, '/');

                    temp[0] = *(indexPtr-2);
                    temp[1] = *(indexPtr-1);

                    timeNow.tm_mday = atoi(temp);
                    sprintf(OutString,"Day %d\r\n", timeNow.tm_mday);
                    putsConsole(OutString);

                    temp[0] = *(indexPtr+1);
                    temp[1] = *(indexPtr+2);
                    temp[2] = *(indexPtr+3);
                    temp[3] = *(indexPtr+4);

                    timeNow.tm_year = (atoi(temp)-1900);
                    sprintf(OutString,"Year %d\r\n", atoi(temp));
                    putsConsole(OutString);

                    putsConsole("\r\nEnter Time in HH:MM (Hours:Minutes) format\r\n");
                    waitingResponse = TRUE;
                    consoleState = 0x92;

                }
                break;
            }

            case 0x32:
                putsConsole("\r\nProgramming Passphrase...\r\n");
                RN_UartCmdMode = TRUE;
                while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
                sprintf(OutString,"set wlan pass %s\r\n",consoleMsg);
                while(PutCMD_WiFlyUART(OutString,"OK",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("set wlan join 1\r\n",">",SEC(1))==Result_BUSY);
                while(PutCMD_WiFlyUART("save\r\n", "OK", SEC(1)) == Result_BUSY);
                while(PutCMD_WiFlyUART("exit\r\n", "", SEC(1)) == Result_BUSY);
                RN_UartCmdMode = FALSE;
                EEPROM_Config_Defaults();
                putsConsole("\r\nSetting EEPROM provisioned flag...");
                {
                unsigned char xeeVal = DEVICE_PROVISION_VAL;
                XEEWriteCompleteArray(EE_PROVISION_ADDR, (char *) &xeeVal, sizeof(xeeVal));
                }
                consoleState = 0;
                //resets the board after new configurations are programmed
                SoftReset();                
                
            case 0x92:
                {
                char *indexPtr;

                if(indexPtr = strchr(consoleMsg, ':'))
                {
                    unsigned char temp[2] = {"\0"};

                    temp[0] = *(indexPtr-2);
                    temp[1] = *(indexPtr-1);

                    timeNow.tm_hour = atoi(temp);
                    sprintf(OutString,"Hours %d\r\n", timeNow.tm_hour);
                    putsConsole(OutString);
        
                    temp[0] = *(indexPtr+1);
                    temp[1] = *(indexPtr+2);
                    

                    timeNow.tm_min = atoi(temp);
                    sprintf(OutString,"Min %d\r\n", timeNow.tm_min);
                    putsConsole(OutString);

                }
                }
                timeNow.tm_isdst = 0;
                sprintf(OutString, "\r\nOld Time in secs - %lu\r\n", PIC32_time);
                putsConsole(OutString);
                PIC32_time = mktime(&timeNow);
                sprintf(OutString, "\r\nNew Time in secs - %lu\r\n", PIC32_time);
                putsConsole(OutString);
                XEEWriteCompleteArray(CURRENT_TIME_INFO, (char *) &PIC32_time, 4);

                consoleState = 0;
                break;

            default:
                consoleState = 0;
                
                break;
                //Resets console selection
                //Unexpected state
        }

        putsConsole("\r\n>>");
    }

    memset(consoleMsg, '\0', sizeof(consoleMsg));
}

/*******************************************************************************
  Function:
    void _Factory_Reset_RN(void)

  Remarks: Reset the RN1723 module to factory defaults

*/
void _Factory_Reset_RN(void)
{
    unsigned long int timeout;
    UINT32 baud = WIFLY_FACTORY_RATE;


    /* Hold RN in Reset and drive GPIO9 High */
    RN_RESET_LOW();
    RN_GPIO9_HIGH();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    /* Release Reset and toggle GPIO9 5 times */
    RN_RESET_HIGH();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_LOW();    /* 1*/
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_HIGH();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_LOW();     /* 2 */
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_HIGH();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_LOW();     /* 3 */
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_HIGH();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_LOW();     /* 4 */
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);;

    RN_GPIO9_HIGH();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_GPIO9_LOW();   /* 5 */
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    /* reset the device */
    RN_RESET_LOW();
    timeout = TickGet() + TICK_SECOND * 1;
    while ((UINT32)TickGet() < timeout);

    RN_RESET_HIGH();

    // Write Factory Default UART Setting to EEProm
    //XEEWriteCompleteArray(EE_RN_UARTSETTITNGS_ADDR,(BYTE *)&baud, 4);
    timeout = TickGet() + TICK_SECOND * 2;
    while ((UINT32)TickGet() < timeout);

    //Reset();
}
