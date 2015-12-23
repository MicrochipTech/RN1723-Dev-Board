/*******************************************************************************
   Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    self_test.c

  Summary:
    This file contains the source code for the LPCM Demo application.

  Description:
    This file contains the source code for the LPCM Demo application.  It
    implements the logic of the application's state machine and it may call
    API routines of other modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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


#include "../include/HardwareProfile.h"
#include "../include/console.h"
#include "../include/Tick.h"
#include "../include/wifly_UART_drv.h"
#include "../include/spi_eeprom.h"
#include "../include/app.h"
#include "../include/self_test.h"

#include <GenericTypedefs.h>
#define EEPROM_TEST_ADDRESS                  250 /*1 byte */
BOOL self_TESTMode = FALSE;
BOOL consoleKeyPressed = FALSE;
extern BOOL RN_UartCmdMode;

#define SPI_READ_ID 0x90

/*******************************************************************************
  Function:
    BOOL self_test(void)

  Remarks:
    This routine can be used to self test the RN1723 DEV BOARD
*/
void self_test()
{
    BOOL status = TRUE;
    self_TESTMode = TRUE;
    
    putsConsole("\r\nSelf Test Mode - RN1723 Dev Board\r\n");
    putsConsole("Press SW0 on RN1723 Dev Board to continue...\r\n");
    
    while(1)
    {
        if(Read_SW0() == 0)
        {
            while(!Read_SW0());
            break;    
        }
    }
    putsConsole("\r\nTesting Switches... OK");
    
    putsConsole("\r\nTesting EEPROM...");
    status = (status & EEPROM_selfTest());
    
    putsConsole("\r\nTesting Flash Memory...");
    status = (status & Flash_selfTest());
    
    putsConsole("\r\nTesting LEDs....");
    status = (status & LEDs_selfTest());
    
    putsConsole("\r\nTesting Potentiometer....");
    status = (status & Potentiometer_selfTest());
    
    putsConsole("\r\nTesting Temperature Sensor...");
    status = (status & TempSensor_selfTest());
    
    putsConsole("\r\nTesting RN1723 Module...");
    status = (status & Configure_RN1723());
    
    self_TESTMode = FALSE;
    if(status)
    {
        putsConsole("\r\nSelf Test Pass\r\n");
        while(1);
    }
    else
    {
        putsConsole("\r\n***Self Test Failed***\r\n");
        while(1);
    }
   
}

/*******************************************************************************
  Function:
    BOOL Configure_RN1723(void)

  Remarks:
    This routine can be used to configure out of factory RN1723 module
*/
BOOL Configure_RN1723()
{
    unsigned int retry = 0;
    WIFLY_Result_t result;
    BOOL status = FALSE;
    
    
    Wifly_Default_Config(FALSE);        //web app mode turned off
    while(PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
    while(PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);
    while(retry < 8)
    {
        while(result = PutCMD_WiFlyUART("join roving1\r\n",">",SEC(1))==Result_BUSY);
        {
            uint32_t timeout = TickGet();
            while(TickGet() < (timeout + (10 * TICK_SECOND)));
        }
        if(RN_ASSOCIATED())
        {
            status = TRUE;
            break;
        }
        retry++;
        putsConsole("\r\nConnection to roving1 FAIL");
    }
    
    while(PutCMD_WiFlyUART("reboot\r\n", "*Reboot*", SEC(1)) == Result_BUSY);
   
    return status;
}

/*******************************************************************************
  Function:
    BOOL EEPROM_selfTest(void)

  Remarks:
    This routine can be used to test the EEPROM
*/
BOOL EEPROM_selfTest(void)
{
    unsigned char xeeVal, temp;
    //Read current value
    XEEReadArray(EEPROM_TEST_ADDRESS, &xeeVal, sizeof(xeeVal));
    //Write 0x11 to the address
    temp  = 0x11;
    XEEWriteCompleteArray(EEPROM_TEST_ADDRESS, (char *) &temp, sizeof(temp));
    //Read back to verify
    XEEReadArray(EEPROM_TEST_ADDRESS, &temp, sizeof(temp));
    if(temp == 0x11)
    {
        putsConsole("OK");
        return TRUE;
    }
    else
    {
        putsConsole("**FAIL**");
        return FALSE;
    }
    //restore the original value
    XEEWriteCompleteArray(EEPROM_TEST_ADDRESS, (char *) &xeeVal, sizeof(xeeVal));
    
}

/*******************************************************************************
  Function:
    BOOL Flash_selfTest(void)

  Remarks:
    This routine can be used to test the 8K Flash
*/
BOOL Flash_selfTest(void)
{
    BYTE readV, deviceId;
    char  OutString[128] = {'\0'};
    
    SPIFlash_GetID(&readV, &deviceId);
     if ((readV == 0xBF) && (deviceId == 0x8D))
     {
        putsConsole("OK [4Mbit SPI Serial Flash]");
         
        return TRUE;
     }
    
     else if ((readV == 0xBF) && (deviceId == 0x8E))
     {
        putsConsole("OK [8Mbit SPI Serial Flash]");
         
        return TRUE;
     }
    else
    {
        putsConsole("**FAIL**");
        return FALSE;
    }
}

/*******************************************************************************
  Function:
    BOOL LEDs_selfTest(void)

  Remarks:
    This routine can be used to test the LEDs
*/
BOOL LEDs_selfTest(void)
{
    LED0_ON();
    LED1_ON();
    LED2_ON();
    LED3_ON();
    
    putsConsole("LEDS(0-3) ON");
    return TRUE;
}

/*******************************************************************************
  Function:
    BOOL Potentiometer_selfTest(void)

  Remarks:
    This routine can be used to test the POT
*/
BOOL Potentiometer_selfTest(void)
{
    unsigned short int potValue, temp;
    uint32_t timeout;
    char  OutString[128];
    
    potValue = ReadAnalogPOT();
    putsConsole("\r\nTurn the analog potentiometer to verify the readings -");
    putsConsole("\r\n [Press SW1 key to exit]");
    sprintf(OutString,"\r    Potentiometer Value %d", potValue);
    putsConsole(OutString);
    while(!consoleKeyPressed)
    {
        if(Read_SW1() == 0)
        {
            while(!Read_SW1());
            break;    
        }
        
        timeout = TickGet();
        while(TickGet() < (timeout + (20 * TICK_MILLISECOND)));
        temp = ReadAnalogPOT();
        if((unsigned short int) (potValue - temp) > 2)
        {
            potValue = temp;
            sprintf(OutString,"\r    Potentiometer Value %d", potValue);
            putsConsole(OutString);
            
        }
        
        
    }
    consoleKeyPressed = FALSE;
    return TRUE;
}


/*******************************************************************************
  Function:
    BOOL TempSensor_selfTest(void)

  Remarks:
    This routine can be used to test the Temperature sensor
*/
BOOL TempSensor_selfTest(void)
{
    short int tempValue;
    char  OutString[128];
    
    tempValue = ReadAnalogTemp();

    if((tempValue > 60) && (tempValue < 90))
    {
        sprintf(OutString,"Temperature Reading = %dF OK", tempValue);
        putsConsole(OutString);
        return TRUE;
    }
    else
    {
        sprintf(OutString,"tempValue %d **FAIL**", tempValue);
        putsConsole(OutString);
        return FALSE;
    }
}

/*******************************************************************************
  Function:
    BOOL TiltVibe_selfTest(void)

  Remarks:
    This routine can be used to test the Tilt vibe sensor
*/
//Current version - Tilt Vibe not tested
BOOL TiltVibe_selfTest(void)
{  
    return TRUE;
}

/*******************************************************************************
  Function:
    void ConsoleMsgCallback_selfTest(void)

  Remarks:
    This routine is sets the consoleKeyPressed flag state
*/
void ConsoleMsgCallback_selfTest(void)
{
    consoleKeyPressed = TRUE;
}


