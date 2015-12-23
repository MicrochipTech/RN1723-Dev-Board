/*******************************************************************************
 The main program that launches all the tasks to configure the wifly module and
 and then run the http client.

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
 Performs the RN module configuration

  Description:
     main program to run initialize the Uarts used as a terminal and to
     communicate with the RN WiFi module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <year> released Microchip Technology Inc.  All rights reserved.

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

#include <xc.h>
#include "include/HardwareProfile.h"
#include "include/console.h"
#include "include/Tick.h"
#include "include/wifly_api.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GenericTypeDefs.h>
#include <plib.h>
#include "include/sys_tasks.h"
#include <cyassl/ssl.h>
#include <cyassl/ctaocrypt/logging.h>
#include "ca-cert.h"
#include "include/XEEPROM.h"


/*** CONFIGURATION ************************************************************/
// *****************************************************************************
// *****************************************************************************
// Section: Configuration bits
// *****************************************************************************
// *****************************************************************************
// DEVCFG3
// USERID = No Setting
#pragma config UPLLEN   = ON        // USB PLL Enabled
#pragma config FPLLMUL  = MUL_15        // PLL Multiplier
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
#pragma config FPLLIDIV = DIV_3         // PLL Input Divider
#pragma config FPLLODIV = DIV_1         // PLL Output Divider
#pragma config FPBDIV   = DIV_2         // Peripheral Clock divisor
#pragma config FWDTEN   = OFF           // Watchdog Timer
#pragma config WDTPS    = PS1           // Watchdog Timer Postscale
#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
#pragma config OSCIOFNC = OFF           // CLKO Enable
#pragma config POSCMOD  = HS            // Primary Oscillator
#pragma config IESO     = OFF           // Internal/External Switch-over
#pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
#pragma config FNOSC    = PRIPLL        // Oscillator Selection
//#pragma config FNOSC    = FRCPLL        // Oscillator Selection
#pragma config CP       = OFF           // Code Protect
#pragma config BWP      = OFF           // Boot Flash Write Protect
#pragma config PWP      = OFF           // Program Flash Write Protect
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select

//#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
//#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#define NO_SHA256

/*****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// ****************************************************************************
* VARIABLES *****************************************************************/

char  OutString[128];
const char Version[]        = "M180.49";
const char CompileDate[]    =__DATE__;
const char CompileTime[]    =__TIME__;


/** LOCAL PROTOTYPES ********************************************************/
void InitializeBoard(void);
void InitializeWifly(void);
void InitializeDataResourses(void);

void StatusLedTasks(void);      // Toggle LED_D3 every second - indicates blocking code
void AppTasks(void);            // Application Tasks

int RN_CyaSSL_Send_cb(void *sslin, char *buf, int sz, void *ctx);
int RN_CyaSSL_Receive_cb(void *sslin, char *buf, int sz, void *ctx);
void RN_CyaSSL_Logging_cb(const int logLevel, const char *const logMessage);


char  OutString[128];               // console/lcd display output buffer
char  WiflyInBuffer[1024];           // input buffer used to receive data from WiFly
int   WiflyInBufferIndex;           // index to wifly input buffer array


typedef struct
{
    /* The application's current state */
//    APP_STATES state;

    /* TODO: Define any additional data used by the application. */
    uint16_t port;
    void* ctx;
    void* ssl;
    char urlBuffer[256];
    char * host;
    char * path;
    uint8_t ipMode;
    uint8_t queryState;
    int8_t cyasslConnectionState;
    bool cyaSSLLogEnabled;

    uint64_t testStart;
    uint64_t connectionOpened;
    uint64_t sslNegComplete;
    uint64_t firstRxDataPacket;
    uint64_t lastRxDataPacket;
    uint32_t ip_address;

    uint32_t rawBytesReceived;
    uint32_t rawBytesSent;
    uint32_t clearBytesReceived;
    uint32_t clearBytesSent;

} APP_DATA;

APP_DATA appData;

char networkBuffer[256];

#define EE_PROVISION_ADDR           0x000


int main(void)
{
    static char xeeVal;
    CYASSL_CTX*     ctx;
    CYASSL*         ssl;
    int16_t socket;
    int ret;
    int  error;
    char buffer[64];

    mJTAGPortEnable(0);

    /* initialize the PIC and prepare the RN module to operate in infrastructure mode */
    InitializeBoard();

    LED0_ON();

    // Check if Board put into Reset Factory Default Mode
    if (Read_SW0() == 0 && Read_SW1() == 0)
    {
        putsConsole("\r\nRestoring System to Default Settings...");
        xeeVal = 0xFF;
        XEEWriteCompleteArray(EE_PROVISION_ADDR, &xeeVal, sizeof(xeeVal));
    }
    
    // Check if System has been Provisioned
    XEEReadArray(EE_PROVISION_ADDR, &xeeVal, sizeof(xeeVal));

    char correctVal = 0xA5;

    if (xeeVal != correctVal)
    {
        putsConsole("\r\nSytem NOT Initiallized...");

        /* Comfigure the RN module  with Default settings*/
        Wifly_Default_Config();

        xeeVal = 0xA5;
        XEEWriteCompleteArray(EE_PROVISION_ADDR, &xeeVal, sizeof(xeeVal));

        Wifly_Reboot();
    }

    LED1_ON();

    /*************************************************************************/
    /*************************************************************************/
    putsConsole("\r\nInitiallize SSL \r\n");

    CyaSSL_SetLoggingCb(&RN_CyaSSL_Logging_cb);
    CyaSSL_Debugging_ON();
    // Initiallize SSL
    CyaSSL_Init();

    // Create a new context
    ctx = CyaSSL_CTX_new(CyaSSLv23_client_method());

    if (ctx == NULL)
        putsConsole("unable to get ctx");

    // Hook up the send/receive callbacks
    CyaSSL_SetIORecv(ctx, (CallbackIORecv)&RN_CyaSSL_Receive_cb);
    CyaSSL_SetIOSend(ctx, (CallbackIOSend)&RN_CyaSSL_Send_cb);

    // Load the certificates
    if (CyaSSL_CTX_load_verify_buffer(ctx, caCert, caCert_len, SSL_FILETYPE_PEM) != SSL_SUCCESS)
    {
        // Couldn't load the certificates
        putsConsole("Error loading CA-Cert, please check the file.\n");
        //exit(EXIT_FAILURE);
    }

    // Turn off verification, because SNTP is usually blocked by a firewall
    CyaSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);

    // Wait for AP Association
    while (!RN_ASSOCIATED());

    // OPEN Socket
    RN_OPEN_SOCKET();

    // Wait for Socket to OPEN
    while (RN_CONNECTION_STATUS() == SOCKET_CLOSED);
    
    // After connecting to socket
    //Create a new SSL structure
    ssl = CyaSSL_new(ctx);

    if (ssl == 0)
    {
        putsConsole("CyaSSL_new error.\n");
    }

    socket = 1;

    // Set the file descriptors
    if (CyaSSL_set_fd(ssl, socket) != SSL_SUCCESS)
    {
        // Could not set the file descriptor
        putsConsole("CyaSSL set the file descriptor error.\n");
    }

    flushWiFlyRxBuffer();
    
    do
    {
        ret = CyaSSL_connect(ssl);

        if (ret != SSL_SUCCESS)
        {
            error = CyaSSL_get_error(ssl, ret);
        }
    } while (ret != SSL_SUCCESS && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE));

    if (ret == SSL_SUCCESS)
    {
        putsConsole("SSL_connect Passed!");
    }
    else
    {
        sprintf(OutString,"\r\n\err = %d, %s\n", error,
                                CyaSSL_ERR_error_string(error, buffer));
        putsConsole(OutString);

        putsConsole("\r\nSSL_connect failed\r\n");
        
        while(1);
    }

    // Send GET Request
    int msgSz;
    char msg[32] = "hello cyassl!";   /* GET may make bigger */

    putsConsole("\r\nSending GET...\n");
    msgSz = 28;
    strncpy(msg, "GET /index.html HTTP/1.0\r\n\r\n", msgSz);
    msg[msgSz] = '\0';

    if (CyaSSL_write(ssl, msg, msgSz) != msgSz)
        putsConsole("SSL_write failed");

    // Get Response
    while(1)
    {
        ret = CyaSSL_read(ssl, networkBuffer, sizeof(networkBuffer)-1);

        if (ret < 0)
        {
            error = CyaSSL_get_error(ssl, ret);

            if ((error == SSL_ERROR_WANT_READ) || (error == SSL_ERROR_WANT_WRITE))
            {
                ;
            }
            else
            {
                putsConsole("\r\nSSL_Read failed\r\n");
            }
        }
        else
        {
            networkBuffer[ret] = 0;
            putsConsole(networkBuffer);
        }

    }

    while(1);
    // Close Socket
    RN_CLOSE_SOCKET();
    
    // Cleanup SSL
    CyaSSL_CTX_free(ctx);
    CyaSSL_Cleanup();
    
    /*************************************************************************/
    /*************************************************************************/

    while(1)
    {
        sys_Tasks();
    }
    
}

/******************************************************************************
 * Function:        void sys_Initialize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine takes care of all of the system
 *                  initialization that is required.
 *
 * Note:
 *
 *****************************************************************************/
void InitializeBoard(void)
{
    // Enable multi-vectored interrupts
    INTEnableSystemMultiVectoredInt();

    // Enable optimal performance
    SYSTEMConfigPerformance(GetSystemClock());
    // Use 1:1 CPU Core:Peripheral clocks
    mOSCSetPBDIV(OSC_PB_DIV_1);

    // Initiallize the UART Console
    initConsole(WIFLY_BAUD_RATE_FAST);
    clrscrConsole();
    homeConsole();

    // Print Build Version Info
    sprintf(OutString,"\r\nRN UART Driver Ver: %s\r\n", Version);
    putsConsole(OutString);
    sprintf(OutString,"Build Date: %s %s\r\n", CompileDate, CompileTime);
    putsConsole(OutString);

    // Initiallize the RN-WiFly interface
    initWiFly(WIFLY_BAUD_RATE_FAST);

    Init_Switches();

    // Initiallize LED's
    LEDS_OFF();
    Init_LEDs();

    // Initialize the Timers
    TickInit();

    // Initialize the SPI EEPROM
    XEEInit();
    
}


int RN_CyaSSL_Receive_cb(void *sslin, char *buf, int sz, void *ctx)
{
    uint16_t cnt = 0;

    // See how many bytes avail to read
    cnt = WiFlyDataAvailable();

    // If no Data in Buffer return ERROR
    if (cnt == 0)
    {
        return CYASSL_CBIO_ERR_WANT_READ;
    }

    cnt = getsWiFly_noWait((char *)buf, sz);

    // Return the number of bytes Read
    return cnt;
}


int RN_CyaSSL_Send_cb(void *sslin, char *buf, int sz, void *ctx)
{
    // Check to see how much data can be sent
//
//    // If none can be sent return ERROR
//    if (bufferSize == 0)
//    {
//        return CYASSL_CBIO_ERR_WANT_WRITE;
//    }

    // Send the data to the RN171
    putbWiFly (buf, sz);

    // Return the ammount of data Sent
    return sz;
}


void RN_CyaSSL_Logging_cb(const int logLevel, const char *const logMessage)
{
    if (!appData.cyaSSLLogEnabled)
    {
        return;
    }
    sprintf(OutString,"CyaSSLog (%d): %s\r\n", logLevel, logMessage);
    putsConsole(OutString);
}