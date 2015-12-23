/*******************************************************************************
  Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER+++++++++++++++++++++++++++++++++++++++++++++
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "https_tasks.h"
#include "json_parser.h"
#include "GenericTypeDefs.h"


// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros/Constants
// *****************************************************************************
// *****************************************************************************
#define EE_PROVISION_ADDR               0x000
#define DEVICE_INTERMEDIATE_VAL         0xA0
#define DEVICE_PROVISION_VAL            0xA5

#define MAX_POST_BUFFER_LEN             1024
#define MAX_RX_BUFFER_LEN               1024


#define OK_200_RESPONSE            "HTTP/1.1 200 OK"
#define OK_303_RESPONSE            "HTTP/1.1 303 See Other"
#define HTTP_BODY_SEPERATION_SEQUENCE    "\r\n\r\n"
 

#define SETTINGS_DEFAULT_URL "PUT /RN1723/App1/v1/000666672f67f/settings HTTP/1.1\r\n"

//User EEPROM total 0x37f bytes
#define USER_REGISTRATION_ADDRESS           0x000   
#define USER_SERVER_IP_ADDRESS              USER_REGISTRATION_ADDRESS + 2
#define USER_MODULE_IP_ADDRESS              USER_REGISTRATION_ADDRESS + 26      /* 20 bytes*/
#define USER_MODULE_MAC_ADDRESS             USER_REGISTRATION_ADDRESS + 50      /* 24 */
#define USER_BASE_URI_ADDRESS               USER_REGISTRATION_ADDRESS + 68    /* 40 bytes */
#define CURRENT_TIME_INFO                   USER_REGISTRATION_ADDRESS + 108   /* 4 bytes*/
#define USER_HOST_NAME                      USER_REGISTRATION_ADDRESS + 112   /*40 bytes*/
#define USER_PUBLISH_PERIOD                 USER_REGISTRATION_ADDRESS + 148   /* 2 bytes long */
#define USER_HOST_SERVER                    USER_REGISTRATION_ADDRESS + 152   /* 30 byte slot */
#define USER_API_VERSION                    USER_REGISTRATION_ADDRESS + 184   /* 6 BYTE slot*/
#define USER_WIFLY_VERSION                  USER_REGISTRATION_ADDRESS + 192   /* 3 bytes slot */
#define USER_TILTVIBE_VALUE                 USER_REGISTRATION_ADDRESS + 200   /* 1 byte 8-bits */
#define USER_TEMPTHRESHOLD_VALUE            USER_REGISTRATION_ADDRESS + 202   /* 1 bytes */
#define USER_POTMAXTHRESHOLD_VALUE          USER_REGISTRATION_ADDRESS + 204   /* 2 byte 16-bits */
#define USER_SERVER_DNS_NAME                USER_REGISTRATION_ADDRESS + 208   /* 34 bytes */

char postBuffer[MAX_POST_BUFFER_LEN];
char rxBuffer[MAX_RX_BUFFER_LEN];


//APP_STATES
typedef enum
{
    APP_INIT=0,
    APP_PROVISION,
    APP_START_CONNECTION,
    APP_PREPARE_REQUEST,
    APP_SEND_REQUEST,
    APP_WAIT_RESPONSE,
    APP_HANDLE_RESPONSE,
    APP_200_RESPONSE,
    APP_303_RESPONSE,
    APP_CLOSE_CONNECTION,
    APP_DEVICE_SLEEP,
    APP_CLEAR_RESOURCES,
    APP_WAIT_ASSOC,
    APP_ERROR
} APP_STATES;



// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
//Definition of APP_DATA
typedef struct
{
    APP_STATES app_state;
    HTTP_SOCKET socket;
} APP_DATA;

typedef struct __attribute__((packed,aligned(2))) _RESOURCE_URI
{
    unsigned short int  uriObjectID;
    char                *uriObject;
    unsigned short int  groupOfCommandIDs[10];
    unsigned char       *httpHeaderURL;
    char                *host;
} RESOURCE_URI;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
void App_Initialize (void);
void AppTasks(APP_DATA* data);
void Wifly_getMAC(unsigned char* address);
uint16_t get_HTTP_ContentLength(void);
void updateDeviceInfo(int uriId, char *httpObject, JSON_Object *object);
long PIC32_Time(void);
void Wifly_Default_Config(BOOL run_webApp);
void EEPROM_Config_Defaults();




#endif /* _APP_H */
/*******************************************************************************
 End of File
 */
