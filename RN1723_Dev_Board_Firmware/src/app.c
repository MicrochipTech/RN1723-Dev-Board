/*******************************************************************************
  Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the LPCM Demo Application.

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

#include "../include/app.h"
#include "../include/json_format.h"
#include "../include/json_parser.h"
#include "../include/HardwareProfile.h"
#include "../include/console.h"
#include "../include/Tick.h"
#include "../include/wifly_UART_drv.h"
#include "../include/https_tasks.h"
#include "../include/spi_eeprom.h"
#include <GenericTypedefs.h>


char OutString[128];
const char Version[] = "1.00";
const char CompileDate[] = __DATE__;
const char CompileTime[] = __TIME__;


const WiFlyCmdSet_t App_WiFly_DefaultConfig[] = {
    // Command                              Response    Timeout     WaitAfterCommand
    {"$$$", "", SEC(1), MS(900)},
    {"\r\n", "> ", SEC(1), MS(50)},
    {"set ip  host 192.168.1.10\r\n", "> ", SEC(1), MS(50)},
    {"set dns name 0\r\n", "> ", SEC(1), MS(50)},
    {"set ip  remote 443\r\n", "> ", SEC(1), MS(50)},
    {"set ip proto 2\r\n", "> ", SEC(1), MS(50)}, // Make TCP but not HTTP Client on RN
    {"set sys trigger 0x20\r\n", "> ", SEC(1), MS(50)},
    {"set sys wake 10\r\n", "> ", SEC(1), MS(50)},
    {"set sys sleep 0\r\n", "> ", SEC(1), MS(50)},
    {"set ip dhcp 3\r\n", "> ", SEC(1), MS(50)},
    {"set comm open 0\r\n", "> ", SEC(1), MS(50)}, // Make sure 0 to prevent echo from RN
    {"set comm close 0\r\n", "> ", SEC(1), MS(50)}, // Make sure 0 to prevent echo from RN
    {"set comm remote 0\r\n", "> ", SEC(1), MS(50)},
    {"set wlan join 0\r\n", "> ", SEC(1), MS(50)},
    {"set sys iofunc 0x70\r\n", "> ", SEC(1), MS(50)},
    {"set uart baud 115200\r\n", "> ", SEC(1), MS(50)},
    {"set uart cmdgpio 1\r\n", "> ", SEC(1), MS(50)},
    {"save\r\n", "> ", SEC(1), MS(50)},
    {"exit\r\n", "", SEC(1), MS(50)},
    {"", "", 0, 0}, //Finished
};

const WiFlyCmdSet_t WiFly_Auto_Join[] = {
    // Command                              Response    Timget eout     WaitAfterCommand
    {"$$$", "", SEC(1), MS(900)},
    {"\r\n", "> ", SEC(1), MS(50)},
    {"set wlan join 1\r\n", ">", SEC(1), MS(50)},
    {"save\r\n", "> ", SEC(1), MS(50)},
    {"exit\r\n", "", SEC(1), MS(50)},
    {"", "", 0, 0}, //Finished
};

const WiFlyCmdSet_t WiFly_FACTORY_Reset[] = {
    // Command                              Response    Timeout     WaitAfterCommand
    //{"$$$"                                  ,"CMD"      ,SEC(1)    ,MS(900) },
    //{"\r\n"                                 ,"> "       ,SEC(1)    ,MS(50) },
    {"factory R\r\n", "> ", SEC(1), MS(50)},
    {"reboot\r\n", "*Reboot*", SEC(2), MS(50)},
    {"", "", 0, 0}, //Finished
};

const WiFlyCmdSet_t WiFly_Enable_TiltVibe[] = {
    //Command                                               Response    Timeout     WaitAfterCommand
    //{"$$$"                                                  ,"CMD"         ,SEC(1)    ,MS(900) },
    //{"\r\n"                                                 ,"> "       ,SEC(1)    ,MS(50) },
    {"set sys trigger 0x28\r\n", "> ", SEC(1), MS(50)},
    {"set ip dhcp 3\r\n", "> ", SEC(2), MS(50)},
    {"save\r\n", "> ", SEC(1), MS(50)},
    {"exit\r\n", "", SEC(1), MS(50)},
    {"", "", 0, 0}, //Finished
};

const WiFlyCmdSet_t WiFly_Disable_TiltVibe[] = {
    //Command                                               Response    Timeout     WaitAfterCommand
    //{"$$$"                                                  ,"CMD"         ,SEC(1)    ,MS(900) },
    //{"\r\n"                                                 ,"> "       ,SEC(1)    ,MS(50) },
    {"set sys trigger 0x20\r\n", "> ", SEC(2), MS(50)},
    {"set ip dhcp 3\r\n", "> ", SEC(2), MS(50)},
    {"save\r\n", "> ", SEC(1), MS(50)},
    {"exit\r\n", "", SEC(1), MS(50)},
    {"", "", 0, 0}, //Finished
};

typedef enum _LPC_COMMANDS
{
    LPC_SWITCHES_STATUS = 0x1000,
    LPC_POTVALUE_STATUS,
    LPC_LEDS_STATUS,
    LPC_SETTING_PUBLISH_PERIOD,
    LPC_SETTING_SECOND_SCALAR,
    LPC_TILT_VIBE,
    LPC_ALARMS_TEMP_THRESHOLD,
    LPC_ALARMS_POTMAX_THRESHOLD,
    LPC_ALARMS_POTMIN_THRESHOLD,


    LPC_STATUS_EVENT_ID, /* scheduled events IDs */
    DIAG_EVENT_ID,
    INTERNAL_ONLY_DIAG_COMMAND, /* doesn't require Thermo cmds */


    UPDATE_URI_ID,

    LOG_URI_ID,
    ALARMS_URI_ID,

    CLOUD_URI_ID,
    SYSTEM_URI_ID,

} LPC_COMMANDS;


RESOURCE_URI uriResourceTable[] = {

    {SYSTEM_URI_ID, "/system",
        {LPC_SETTING_PUBLISH_PERIOD, LPC_TILT_VIBE, LPC_ALARMS_POTMAX_THRESHOLD, LPC_ALARMS_TEMP_THRESHOLD, 0x0000}, SETTINGS_DEFAULT_URL, NULL},
};


char moduleMACAddress[14] = {'\0'};
char moduleBaseURI[40] = "/RN1723/App1/v1";
char hostName[40] = "192.168.1.10";

uint32_t PIC32_time = 1432238104;
BOOL module_time_set = FALSE;

extern APP_DATA app;
extern BOOL RN_UartCmdMode;
extern BOOL consoleKeyPressed;


//TICK        epochTime;
//uint32_t    timeDiffMS;

/*******************************************************************************
  Function:
    void AppTasks(APP_DATA* app)

  Remarks:
    This routine runs the application-specific state machine
 */
void AppTasks(APP_DATA* app)
{
    unsigned char xeeVal;
    static int16_t bytes_pending;
    static unsigned short int previousUriID;
    static BOOL waitingResponse = FALSE;
    static char previoushttpObject[40];

    //Check if socket "Unexpected Socket Error" flag is set
    //APP_CLOSE_CONNECTION state clears all the socket flags and puts the 
    //PIC and RN to sleep
    if (app->socket.unexpectedSocketError == 1)
    {
        app->app_state = APP_CLOSE_CONNECTION;
        app->socket.unexpectedSocketError = 0;
    }

    //Main application state machine
    //Handles all the RESTFUL states along with formating JSON Data
    switch (app->app_state)
    {
            //APP_INIT:
            //Checks if the system is provisioned
        case APP_INIT:
            //LED0 stays ON as along as PIC is powered ON
            LEDS_OFF();
            LED0_ON();
            XEEReadArray(EE_PROVISION_ADDR, &xeeVal, sizeof (xeeVal));

            // Check if device has been provisioned
            //If provisioned reads the HOST information and other settings from EEPROM   
            if (xeeVal == DEVICE_PROVISION_VAL)
            {
                app->app_state = APP_START_CONNECTION;
                {
                    //Read the HOST information from EEPROM necessary to form PUT/POST request
                    XEEReadArray(USER_BASE_URI_ADDRESS, (char *) &moduleBaseURI, sizeof (moduleBaseURI));
                    XEEReadArray(USER_HOST_NAME, (char *) &hostName, sizeof (hostName));
                    XEEReadArray(CURRENT_TIME_INFO, (char *) &PIC32_time, 4);
                    XEEReadArray(USER_MODULE_MAC_ADDRESS, (char *) moduleMACAddress, 14);
                }
            } else if (xeeVal == DEVICE_INTERMEDIATE_VAL)
            {
                //Wait until RN1723 is *ready*
                {
                    uint32_t timeout = TickGet();
                    while (TickGet() < (timeout + (1 * TICK_SECOND)));
                }
                //Program the device to connect to the Access Point automatically
                //RN Command - "set wlan join 1"
                while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) WiFly_Auto_Join) == Result_BUSY);

                xeeVal = DEVICE_PROVISION_VAL;
                XEEWriteCompleteArray(EE_PROVISION_ADDR, (char *) &xeeVal, sizeof (xeeVal));
            } else
            {
                app->app_state = APP_PROVISION;
            }
            break;

        case APP_PROVISION:
            //APP_PROVISION state can be triggered using GPIO9 
            //and also based on the EEPROM provision flag
        {
            uint8_t cycles = 0;
            BOOL provisionMode = FALSE;
            uint32_t timeout = TickGet();


            putsConsole("\r\n**RN1723 DEV BOARD not provisioned**");
            putsConsole("\r\nPress SW0 to provision the device");
            putsConsole("\r\n[Note: Place jumper on J4 during provision mode]");
            LED0_OFF();
            while (1)
            {
                uint32_t timeout2 = TickGet();
                if ((timeout2 - timeout) > TICK_SECOND)
                {
                    LED0_INV();
                    LED1_INV();
                    LED2_INV();
                    LED3_INV();

                }

                if (Read_SW0() == 0)
                {
                    provisionMode = TRUE;
                    break;
                }

            }

            //              _Factory_Reset_RN();    
            putsConsole("\r\nProvisioning Module...");

            //Resets the RN Module
            //Module comes out of RN WEB_APP

            RN_RESET_LOW();

            {
                //pulse of atleast 160 us
                uint32_t timeout = TickGet();
                while (TickGet() < (timeout + (1 * TICK_MILLISECOND)));
            }

            RN_RESET_HIGH();


            //Set the GPIO13 flag as input to PIC, monitor when the module comes out of sleep
            RN_CTS_Init();

            while (PIC_CTS == 1);

            {
                //Wait atleast 160us before sending commands over UART
                uint32_t timeout = TickGet();
                while (TickGet() < (timeout + (1000 * TICK_MILLISECOND)));
            }

            //Program Default Configuration Settings for RN1723
            Wifly_Default_Config(TRUE);

            //Program EEPROM Default Settings
            EEPROM_Config_Defaults();

            putsConsole("\r\nPress any key to indicate provisioning is complete");

        }

            while (!consoleKeyPressed);
            //putsConsole("Configuring EEPROM Default Settings");
            xeeVal = DEVICE_INTERMEDIATE_VAL;
            XEEWriteCompleteArray(EE_PROVISION_ADDR, (char *) &xeeVal, sizeof (xeeVal));
            break;

            //APP_START_CONNECTION:
            //Trying to establish connection (secure)
        case APP_START_CONNECTION:

            /* Initialize CyaSSL */
            if (!InitializeCyaSSL(&app->socket))
                app->app_state = APP_ERROR;
            else
            {
                //Once CyaSSL is Initialize trigger WiFi Connection
                app->socket.triggerConnection = 1;
                app->app_state = APP_PREPARE_REQUEST;
            }
            break;


            //APP_PREPARE_REQUEST:
            //JSON Formats PUT data
        case APP_PREPARE_REQUEST:

            //Read Sensors and Format Data
            JSON_FormatPUTData(0, postBuffer, NULL);


            app->app_state = APP_SEND_REQUEST;
            break;

            //APP_SEND_REQUEST:
            //Copies the HTTP/JSON data to the buffers
        case APP_SEND_REQUEST:


            strncpy(app->socket.ucTxBuf, postBuffer, strlen(postBuffer));
            app->socket.ucTxBuf[strlen(postBuffer)] = '\0';
            app->socket.triggerRequestReady = 1;
            bytes_pending = 0;
            app->app_state = APP_WAIT_RESPONSE;
            break;

            //APP_WAIT_RESPONSE
            //Waiting for response from the server
        case APP_WAIT_RESPONSE:

            if (app->socket.triggerResponseReady == 1)
            {

                if ((char *) strstr(app->socket.ucRxBuf, (char *) "Content-Length"))
                {
                    //Print Sent Packet
                    putsConsole("\r\nPost to server-\r\n");
                    putsConsole(postBuffer);
                    putsConsole("\r\n\r\n");
                    memset(rxBuffer, '\0', sizeof (rxBuffer));
                    strncpy(rxBuffer, app->socket.ucRxBuf, strlen(app->socket.ucRxBuf));
                    app->socket.triggerResponseReady = 0;
                    bytes_pending = get_HTTP_ContentLength();
                    if (bytes_pending == 0)
                        app->app_state = APP_HANDLE_RESPONSE;
                    break;
                } else
                {
                    strncat(rxBuffer, app->socket.ucRxBuf, strlen(app->socket.ucRxBuf));
                    app->socket.triggerResponseReady = 0;

                    bytes_pending = bytes_pending - strlen(app->socket.ucRxBuf);

                    if (bytes_pending <= 0)
                    {
                        putsConsole("\r\nResponse from server-\r\n");
                        putsConsole(rxBuffer);
                        putsConsole("\r\n\r\n");
                        app->app_state = APP_HANDLE_RESPONSE;
                        app->socket.http_state = SM_HTTP_SOCKET_READY;
                        break;
                    }

                }

            }
            break;

            //APP_HANDLE_RESPONSE:
            //Handles the RESTFUL state messages
        case APP_HANDLE_RESPONSE:

            if ((char *) strstr(rxBuffer, (char *) OK_200_RESPONSE))
            {
                //Response 200: Self Link 
                putsConsole("\r\nSuccessfully communicated with host\r\n");
                app->app_state = APP_200_RESPONSE;
                break;
            } else if ((char *) strstr(rxBuffer, (char *) OK_303_RESPONSE))
            {
                //Response 303: Next Link
                app->app_state = APP_303_RESPONSE;
                break;
            }                //Un-recognized header format
            else
            {
                //Go to sleep
                //Close connection
                app->app_state = APP_CLOSE_CONNECTION;
                break;
            }

            break;

            //APP_200_RESPONSE:
            //Handle 200 RESPONSE        
        case APP_200_RESPONSE:

        {
            char *indexPtr1, *indexPtr2;
            JSON_Value *root_value;
            JSON_Object *object;
            char *httpNextLink = NULL;
            uint8_t root[800];


            indexPtr1 = (char *) strstr(rxBuffer, HTTP_BODY_SEPERATION_SEQUENCE);
            indexPtr1 = indexPtr1 + 4;

            memset(root, '\0', sizeof (root));
            indexPtr2 = (char *) strrchr(rxBuffer, '}');

            strncpy(root, indexPtr1, (indexPtr2 - indexPtr1 + 1));

            indexPtr1 = root;


            //JSON Parsing
            root_value = json_parse_string(indexPtr1);
            if (!(json_value_get_type(root_value) == JSONObject))
            {
                app->app_state = APP_CLOSE_CONNECTION;
                break;
            }
            object = json_value_get_object(root_value);

            httpNextLink = (char *) NULL;
            httpNextLink = (char *) json_object_dotget_string(object, "_links.next.href");

            if ((httpNextLink != NULL) && (waitingResponse == FALSE))
            {
                putsConsole("\r\n200 Next Link - Not handled");
                app->app_state = APP_CLOSE_CONNECTION;
            } else if ((httpNextLink == NULL) && (waitingResponse == TRUE))
            {

                int objectCount, index;
                char objectNameArray[40];
                char *objectName;

                waitingResponse = FALSE;
                objectCount = json_object_get_count(object);

                /* get the string names of objects and compare them to GET URI name previously sent */
                memset(objectNameArray, '\0', sizeof (objectNameArray));
                for (index = 0; index < objectCount; index++)
                {
                    objectName = (char *) json_object_get_name(object, index);
                    if (objectName)
                    {
                        strcpy(objectNameArray, objectName);
                        if (!(strcmp(objectName, previoushttpObject + 1))) /* skip the leading  slash */
                        {
                            updateDeviceInfo(previousUriID, objectName, object);
                        }
                    }
                }
                app->app_state = APP_CLOSE_CONNECTION;

            } else if (httpNextLink == NULL)
            {
                app->app_state = APP_CLOSE_CONNECTION;

            } else
            {
                app->app_state = APP_CLOSE_CONNECTION;

            }
            break;

        }


        //APP_303_RESPONSE:
        //Handle 303 RESPONSE message
        case APP_303_RESPONSE:

        {
            char *indexPtr1, *indexPtr2;
            JSON_Value *root_value;
            JSON_Object *object;
            char *httpNextLink = NULL;
            uint8_t root[800];


            indexPtr1 = (char *) strstr(rxBuffer, HTTP_BODY_SEPERATION_SEQUENCE);
            indexPtr1 = indexPtr1 + 4;

            memset(root, '\0', sizeof (root));
            indexPtr2 = (char *) strrchr(rxBuffer, '}');

            strncpy(root, indexPtr1, (indexPtr2 - indexPtr1 + 1));


            indexPtr1 = root;


            //JSON Parsing
            root_value = json_parse_string(indexPtr1);

            if (!(json_value_get_type(root_value) == JSONObject))
            {
                app->app_state = APP_CLOSE_CONNECTION;
                break;
            }

            object = json_value_get_object(root_value);

            httpNextLink = (char *) NULL;
            httpNextLink = (char *) json_object_dotget_string(object, "_links.next.href");

            if ((httpNextLink != NULL) && (waitingResponse == FALSE))
            {
                static char *httpObject;
                unsigned char index;
                unsigned short int uriID;
                /* find the last ending  "/xxxx" for example inside the link */
                if (httpObject = strrchr(httpNextLink, '/'))
                {
                    /* find object in uri resource table and  process it*/
                    for (index = 0; index < sizeof (uriResourceTable) / sizeof (RESOURCE_URI); index++)
                    {
                        if (!(strcmp(uriResourceTable[index].uriObject, httpObject)))
                        {
                            uriID = uriResourceTable[index].uriObjectID;
                            previousUriID = uriID;
                            waitingResponse = TRUE;
                            /* the current next link object */
                            memset(previoushttpObject, '\0', sizeof (previoushttpObject));
                            strcat(previoushttpObject, httpObject);


                            /* gather up the data for GET to the nexTlink request  */
                            {
                                char httpHdrLine[200];
                                memset(httpHdrLine, '\0', sizeof (httpHdrLine));



                                strcat(httpHdrLine, "GET ");
                                strncat(httpHdrLine, moduleBaseURI, 40);
                                strcat(httpHdrLine, httpNextLink);
                                strcat(httpHdrLine, " HTTP/1.1\r\n");

                                JSON_FormatPUTData(2, postBuffer, httpHdrLine);
                            }

                            app->app_state = APP_SEND_REQUEST;

                            break;


                        }
                        break;
                    }
                }
            } else if ((httpNextLink != NULL) && (waitingResponse == TRUE))
            {
                putsConsole("\r\n303 - Link Chasing Not Handled");
                app->app_state = APP_CLOSE_CONNECTION;

            } else if (httpNextLink == NULL)
            {
                app->app_state = APP_CLOSE_CONNECTION;

            }
            break;

        }
            break;

            //APP_CLOSE_CONNECTION:
            //close the connection (triggers close socket)  
        case APP_CLOSE_CONNECTION:

            app->socket.triggerShutdown = 1;
            app->socket.triggerConnection = 0;
            app->socket.triggerRequestReady = 0;
            app->socket.triggerResponseReady = 0;
            app->app_state = APP_DEVICE_SLEEP;
            break;

            //APP_DEVICE_SLEEP:
            //Put RN1723 in sleep mode. Jumper ON in position J4 turns off PIC32

        case APP_DEVICE_SLEEP:

            if (app->socket.triggerShutdown == 0)
            {
                /* sleep the RN and PIC get powered down */
                putsConsole("\r\nRN1723 going to sleep\r\n");
                TRIS_RN_GPIO8 = 0;
                RN_GPIO8 = 1;
                {
                    uint32_t timeout = TickGet();
                    while (TickGet() < (timeout + (10 * TICK_MILLISECOND)));
                }
                TRIS_RN_GPIO8 = 1;
                //Clear resources
                app->app_state = APP_CLEAR_RESOURCES;
            }
            break;

        //APP_CLEAR_RESOURCES
        //Clears all the flags/buffers/memory
        case APP_CLEAR_RESOURCES:

            app->socket.triggerConnection = 0;
            app->socket.triggerRequestReady = 0;
            app->socket.triggerResponseReady = 0;
            app->socket.triggerShutdown = 0;
            app->socket.unexpectedSocketError = 0;

            if (app->socket.memfree > 1)
            {
                SoftReset();
            }
            app->app_state = APP_WAIT_ASSOC;
            break;
        
        //APP_WAIT_ASSOC
        //Waiting for RN1723 association to AP (access point) to restart state machine
        case APP_WAIT_ASSOC:

            if (RN_ASSOCIATED())
            {
                app->app_state = APP_START_CONNECTION;
                app->socket.http_state = SM_HTTP_IDLE;
            }
            break;

        //APP_ERROR
        //Prints ERROR message on the console if APP_ERROR
        case APP_ERROR:
            putsConsole("\r\nError: Application fault...");
            while (1);
            break;

        default:
            break;
    }





}

/*******************************************************************************
  Function:
    void App_Initialize(void)

  Remarks:
    This routine Initiallizes the application-specific settings
 */
void App_Initialize(void)
{
    // Print Build Version Info
    sprintf(OutString, "\r\nRN with CyaSSL Demo Version: %s\r\n", Version);
    putsConsole(OutString);
    sprintf(OutString, "Build Date: %s %s\r\n", CompileDate, CompileTime);
    putsConsole(OutString);

}

/*******************************************************************************
  Function:
    void Wifly_Default_Config(void)

  Remarks:
    This routine programs the factory default settings required for this demo
 */
void Wifly_Default_Config(BOOL run_webApp)
{
    WIFLY_Result_t result;

    //putsConsole("\r\nProgramming default configuration settings....");
    // Try to communicate @ WIFLY_BAUD_RATE_FAST setting
    while ((result = PutCMD_WiFlyUART("$$$", "CMD", SEC(1))) == Result_BUSY);

    //try again @115200 baud rate
    if (result != Result_SUCCESS)
        while ((result = PutCMD_WiFlyUART("$$$", "CMD", SEC(1))) == Result_BUSY);

    if (result != Result_SUCCESS)
    {
        // Baud rate is not WIFLY_BAUD_RATE_FAST setting, so trying 9600 now
        putsConsole("\r\nModule not at 115200 baud so changing it to 9600");

        // Set baud rate to 9600
        UARTSetDataRate(UART_WIFLY, GetPeripheralClock(), WIFLY_FACTORY_RATE);

        // Try entering cmd mode again
        while ((result = PutCMD_WiFlyUART("$$$", "CMD", SEC(1))) == Result_BUSY);

        //try again @9600 baud rate
        if (result != Result_SUCCESS)
            while ((result = PutCMD_WiFlyUART("$$$", "CMD", SEC(1))) == Result_BUSY);

        if (result != Result_SUCCESS)
        {
            putsConsole("\r\nCannot find proper baud rate at 115200 nor 9600");
        } else
        {

            putsConsole("\r\nFactory Resetting RN Module");
            {
                uint32_t timeout = TickGet();
                while (TickGet() < (timeout + (1 * TICK_MILLISECOND)));
            }
            while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) WiFly_FACTORY_Reset) == Result_BUSY);
            {
                uint32_t timeout = TickGet();
                while (TickGet() < (timeout + (2 * TICK_SECOND)));
            }
            putsConsole("\r\nProgramming Application default Settings");
            while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) App_WiFly_DefaultConfig) == Result_BUSY);
            Wifly_getMAC(moduleMACAddress);

            if (run_webApp)
            {
                //run web app
                while (PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while (PutCMD_WiFlyUART("run web_app\r\n", "", SEC(1)) == Result_BUSY);
                putsConsole("\r\nRunning Web App...");
                putsConsole("\r\n\r\nConnect to Web App and choose the network to join...");
            }

        }

        // Set baud rate to WIFLY_BAUD_RATE_FAST setting
        UARTSetDataRate(UART_WIFLY, GetPeripheralClock(), WIFLY_BAUD_RATE_FAST);
    } else
    {
        putsConsole("\r\nFactory Resetting RN Module");
        {
            uint32_t timeout = TickGet();

            while (TickGet() < (timeout + (1 * TICK_MILLISECOND)));
        }
        while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) WiFly_FACTORY_Reset) == Result_BUSY);
        {
            uint32_t timeout = TickGet();
            while (TickGet() < (timeout + (2 * TICK_SECOND)));
        }

        // Set baud rate to 9600
        UARTSetDataRate(UART_WIFLY, GetPeripheralClock(), WIFLY_FACTORY_RATE);

        putsConsole("\r\nProgramming Application default Settings");
        while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) App_WiFly_DefaultConfig) == Result_BUSY);
        Wifly_getMAC(moduleMACAddress);

        if (run_webApp)
        {
            //run web app
            //putsConsole("\r\nRunning Web App...");
            while (PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
            while (PutCMD_WiFlyUART("run web_app\r\n", "", SEC(1)) == Result_BUSY);
            putsConsole("\r\nRunning Web App...");
            putsConsole("\r\n\r\nConnect to Web App and choose the network to join...");
        }
    }

}

/*******************************************************************************
  Function:
    void ReadAnalogTemp(void)

  Remarks:
    This routine reads the temp sensor value and returns it
 */
WORD ReadAnalogTemp(void)
{
    WORD ADCValue;
    uint32_t timeout;

    ANALOG_TEMP_TRIS = 1;
    mPORTBSetPinsAnalogIn(BIT_0);
    //Configure ADC

    AD1PCFG = 0xFFFE; /* AN0 RB0 */
    AD1CON1 = 0x0000;

    //AD1CHS = 0x000d0000;
    AD1CHS = 0x00000000; /* RB0 <19:16> */

    AD1CSSL = 0; /* 0x2000*/
    AD1CON3 = 0x1F05; /* Perhi clock, Autosample time, x5Tpb*/
    AD1CON2 = 0; /* do not scan inputs, interupt every sampel/conv seq*/


    AD1CON1SET = 0x8000; /* arm ADC */

    AD1CON1SET = 0x0002; /* start sampling */

    timeout = TickGet();
    while (TickGet() < (timeout + (10 * TICK_MILLISECOND)));

    AD1CON1CLR = 0x0002; /* stop sampling */
    while (!(AD1CON1 & 0x0001)); /* DONE status set */

    ADCValue = ADC1BUF0;

    //Voltage to Centigrade conversion
    {
        float temp = ADCValue;
        temp = (temp * 3.2) / 1024;
        temp = temp - 0.5;
        temp = temp * 100;
        temp = (temp * 9) / 5;
        temp = temp + 32;
        ADCValue = (unsigned int) temp;
    }

    return ADCValue;
}

/*******************************************************************************
  Function:
    void ReadAnalogPOT(void)

  Remarks:
    This routine reads the POT value and returns it
 */
WORD ReadAnalogPOT(void)
{
    WORD ADCValue;
    uint32_t timeout;

    ANALOG_POT_TRIS = 1;
    mPORTBSetPinsAnalogIn(BIT_2);
    //Configure ADC
    //AD1PCFG = 0xCFFF;     //All port B digital; RB13 analog  -> AN13
    AD1PCFG = 0xFFFB;
    AD1CON1 = 0x0000;

    //AD1CHS = 0x000d0000;
    AD1CHS = 0x00020000;

    AD1CSSL = 0; /* 0x2000*/
    AD1CON3 = 0x1F05; /* Perhi clock, Autosample time, x5Tpb*/
    AD1CON2 = 0; /* do not scan inputs, interupt every sampel/conv seq*/


    AD1CON1SET = 0x8000; /* arm ADC */

    AD1CON1SET = 0x0002; /* start sampling */

    timeout = TickGet();
    while (TickGet() < (timeout + (10 * TICK_MILLISECOND)));

    AD1CON1CLR = 0x0002; /* stop sampling */
    while (!(AD1CON1 & 0x0001)); /* DONE status set */

    ADCValue = ADC1BUF0;

    return ADCValue;
}

/*******************************************************************************
  Function:
    void Wifly_getMAC(void)

  Remarks:
    This routine reads the MAC address from the Wifly module
 */
void Wifly_getMAC(unsigned char* address)
{

    unsigned char count = 0;
    char *DataBuffPtr = NULL;
    char moduleMACAddressClean[24] = {'\0'};
    WIFLY_Result_t result;

    /* write mac address to EEPROM */
    while (PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);

    FlushWiFlyRxBuffer();
    while ((result = PutCMD_WiFlyUART("get mac\r\n", "Mac Addr=", SEC(1))) == Result_BUSY);

    DataBuffPtr = WiFly_WaitString("\r", SEC(2));

    while (PutCMD_WiFlyUART("exit\r\n ", "", SEC(1)) == Result_BUSY);

    if (DataBuffPtr != NULL)
    {
        strcpy(address, DataBuffPtr);

        /* remove the \r at the end */
        count = strlen(address);
        address[count - 1] = '\0';

        DataBuffPtr = address;
        count = 0;

        /* remove the colons */
        while (*DataBuffPtr != '\0')
        {
            /* skip the : */
            if (*DataBuffPtr == ':')
            {
                DataBuffPtr++;
            } else
            {
                moduleMACAddressClean[count++] = *DataBuffPtr++;
            }
        }
        moduleMACAddressClean[count] = '\0';

        XEEWriteCompleteArray(USER_MODULE_MAC_ADDRESS, (char *) &moduleMACAddressClean, 14);

    }
}

/*******************************************************************************
  Function:
    void get_HTTP_ContentLength(void)

  Remarks:
    This routine reads the value of Content Length from the HTTP message header
 */
uint16_t get_HTTP_ContentLength(void)
{
    uint16_t bodyLen = 0;
    unsigned char index = 0;
    char lenOfHTTPBody[5];
    char *indexPtr;

    memset(lenOfHTTPBody, '\0', sizeof (lenOfHTTPBody));

    /* extract number of bytes that are in body of packet */
    if (indexPtr = (char *) strstr(rxBuffer, "Content-Length:"))
    {
        indexPtr = strchr(indexPtr, ' ');
        indexPtr++;
        index = 0;

        while ((*indexPtr != '\r'))
        {
            lenOfHTTPBody[index] = *indexPtr;
            index++;
            indexPtr++;
        }
        bodyLen = atoi(lenOfHTTPBody);
    }
    return bodyLen;
}

/*******************************************************************************
  Function:
    void updateDeviceInfo(void)

  Remarks:
    This routine updates the EEPROM settings based on the 303 message
 */
void updateDeviceInfo(int uriId, char *objectName, JSON_Object *object)
{


    uint16_t pubPeriod16Bit;
    uint8_t index = 0;
    char objectNameArray[40];
    double currentNumberValue;
    int currentIntValue;

    switch (uriId)
    {

        case SYSTEM_URI_ID:

            /* get the publishPeriod number  */
            strcpy(objectNameArray, objectName);
            strcat(objectNameArray, ".publishPeriod");
            currentNumberValue = 0;
            if ((currentNumberValue = (double) json_object_dotget_number(object, objectNameArray)))
            {
                pubPeriod16Bit = (uint16_t) currentNumberValue;
                XEEWriteCompleteArray(USER_PUBLISH_PERIOD, (char *) &pubPeriod16Bit, 2);


                while (PutCMD_WiFlyUART("$$$", "CMD", SEC(1)) == Result_BUSY);
                while (PutCMD_WiFlyUART("\r\n", ">", SEC(1)) == Result_BUSY);

                sprintf(OutString, "set sys wake %d\r\n", pubPeriod16Bit);
                while (PutCMD_WiFlyUART(OutString, "> ", SEC(1)) == Result_BUSY);
                //while( (result = putsWiFly_ReceiveResponse("save\r\n", "> ",SEC(1)))==Result_BUSY);
                //while( (result = putsWiFly_ReceiveResponse("exit\r\n", "",  SEC(1)))==Result_BUSY);
            }


            /* temp Threshold */
            strcpy(objectNameArray, objectName);
            strcat(objectNameArray, ".temperatureThreshold");
            currentNumberValue = 0;
            if ((currentNumberValue = (double) json_object_dotget_number(object, objectNameArray)))
            {
                index = (unsigned char) currentNumberValue;
                XEEWriteCompleteArray(USER_TEMPTHRESHOLD_VALUE, (char *) &index, 1);
            }

            /* pot threshold */
            strcpy(objectNameArray, objectName);
            strcat(objectNameArray, ".potMaxThreshold");
            currentNumberValue = 0;
            if ((currentNumberValue = (double) json_object_dotget_number(object, objectNameArray)))
            {
                pubPeriod16Bit = (uint16_t) currentNumberValue;
                XEEWriteCompleteArray(USER_POTMAXTHRESHOLD_VALUE, (char *) &pubPeriod16Bit, 2);
            }

            /* tilt vibe */
            strcpy(objectNameArray, objectName);
            strcat(objectNameArray, ".tiltVibe");
            currentIntValue = 99;
            currentIntValue = json_object_dotget_boolean(object, objectNameArray);
            if ((currentIntValue == 1))
            {
                index = (unsigned char) 1;
                XEEWriteCompleteArray(USER_TILTVIBE_VALUE, (char *) &index, 1);

                while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) WiFly_Enable_TiltVibe) == Result_BUSY);

            } else if ((currentIntValue == 0))
            {
                index = (unsigned char) 0;
                XEEWriteCompleteArray(USER_TILTVIBE_VALUE, (char *) &index, 1);

                while (PutCMDSet_WiFlyUART((WiFlyCmdSet_t*) WiFly_Disable_TiltVibe) == Result_BUSY);
            } else
            {

            }

            break;


        default:
            /* send sleep command */
            break;

    } /* end of switch statement*/

}

/*******************************************************************************
  Function:
    void EEPROM_Config_Defaults(void)

  Remarks:
    This routine programs the EEPROM with default settings during provisioning of the module.
 */
void EEPROM_Config_Defaults(void)
{
    uint8_t defaultV;
    uint16_t defaultV_16;

    //Sys wake is set to 15 during provisioning
    defaultV_16 = 15;
    XEEWriteCompleteArray(USER_PUBLISH_PERIOD, (char *) &defaultV_16, 2);
    //Default setting for Temp threshold is set at 80F
    defaultV = 80;
    XEEWriteCompleteArray(USER_TEMPTHRESHOLD_VALUE, (char *) &defaultV, 1);
    //Default setting for POT threshold is set at 600
    defaultV_16 = 600;
    XEEWriteCompleteArray(USER_POTMAXTHRESHOLD_VALUE, (char *) &defaultV_16, 2);
    //Tilt Vibe is disabled during provisioning
    defaultV = 0;
    XEEWriteCompleteArray(USER_TILTVIBE_VALUE, (char *) &defaultV, 1);
    //Module Base URI
    XEEWriteCompleteArray(USER_BASE_URI_ADDRESS, (char *) &moduleBaseURI, 40);
    //Current time info for Certificate Validation
    XEEWriteCompleteArray(CURRENT_TIME_INFO, (char *) &PIC32_time, 4);
    //Host Name
    XEEWriteCompleteArray(USER_HOST_NAME, (char *) &hostName, 40);
}

/*******************************************************************************
  Function:
    void GPIO9IntHandler(void)

  Remarks:
 Captures GPIO9 as Interrupt and puts the module in provisioning mode
 */

void GPIO9IntHandler(void)
{
    unsigned char xeeVal = 0x00;
    IEC0bits.IC3IE = 0;
    IFS0bits.IC3IF = 0;
    app.app_state = APP_INIT;
    XEEWriteCompleteArray(EE_PROVISION_ADDR, (char *) &xeeVal, sizeof (xeeVal));

}

/*******************************************************************************
  Function:
    void PIC32_Time(void)

  Remarks:
 Module return the time information during certification validation process.
 */
long PIC32_Time(void)
{
    return PIC32_time;
}

