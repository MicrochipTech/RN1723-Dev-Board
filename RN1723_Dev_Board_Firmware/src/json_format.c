/*******************************************************************************
 Json format source file

  Company:
    Microchip Technology Inc.

  File Name:
    json_format.c

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

#include "json_format.h"
#include "app.h"
#include "spi_eeprom.h"



unsigned char   *ptrOpenBracket     = "{";
unsigned char   *ptrCloseBracket    = "}";
unsigned char   *ptrColon           = ":";
unsigned char   *ptrComma           = ",";

extern char moduleBaseURI[];
extern char moduleMACAddress[];
extern char hostName[];


const struct _tstatKeys
{
    char *tstatusKey;
    char *systemKey;
    char *msgHeaderObjectKey;

    char *uuidKey;

    char *publishPeriodKey;
    char *potvalueKey;
    char *switchesKey;
    char *ledsKey;
    char *tiltVibeKey;
    char *temperatureThresholdKey;
    char *potMaxThresholdKey;
    char *potMinThresholdKey;
    char *analogTempKey;

}tstatKeys = {  "\"status\": {",
                "\"system\": {",
                "\"msgHeader\": {",

                "\"uuid\": ",
                
                "\"publishPeriod\": ",
                "\"potvalue\": ",
                "\"switches\": ",
                "\"leds\": ",
                "\"tiltVibe\": ",
                "\"temperatureThreshold\": ",
                "\"potMaxThreshold\": ",
                "\"potMinThreshold\": ",
                "\"temperature\": ",
            };



/*******************************************************************************
  Function:
    void JSON_FormatHeader(unsigned char *buffer, unsigned char *httphddrline)

  Remarks:
    This routine formats the http header for PUT/GET request
*/
void JSON_FormatHeader(unsigned char *buffer, unsigned char *httphddrline)
{
/* Sample output format generated 
        PUT /RN1723/App1/v1/0006666dea2d/status HTTP/1.1
        Host: lpc1.cloud.microchip.com
        Accept: application/json
        Connection: keep-alive
        Content-Type: application/json
        Content-Length: 
 */
    if(httphddrline == NULL)
    {
        strcat(buffer, "PUT ");
        strncat(buffer, moduleBaseURI, 40);
        strcat(buffer,  "/");
        strncat(buffer, moduleMACAddress, 14);
        strcat(buffer, "/status HTTP/1.1\r\n");
    }
    else
        strcat(buffer, httphddrline);
    
    //Header Host Line
    strcat(buffer, "Host: ");
    strcat(buffer, hostName);
    strcat(buffer, "\r\n");
    //Header Accept Line
    strcat(buffer, "Accept: application/json\r\n");
    //Header Connection Line
    strcat(buffer, "Connection: keep-alive\r\n");
    //Header Content Type
    strcat(buffer, "Content-Type: application/json\r\n");
    //Header Content Length
    strcat(buffer, "Content-Length: ");
}

/*******************************************************************************
  Function:
    void JSON_FormatPUTData(uint16_t type, unsigned char *buffer, unsigned char *httphddrline)

  Remarks:
    This routine formats the data in http format
*/
void JSON_FormatPUTData(uint16_t type, unsigned char *buffer, unsigned char *httphddrline)
{
    
    unsigned char msgBody[MAX_POST_BUFFER_LEN];
    unsigned char msgBodyLen[10];


    //Prepare http header
    memset(buffer, '\0', sizeof(buffer));
    JSON_FormatHeader(buffer, httphddrline);

    //JSON formatted data content
    //Clear buffers
    memset(msgBody, '\0', sizeof(msgBody));
    
    switch(type)
    {
        //STATUS URI
        case 0x00:
            strcat(msgBody, ptrOpenBracket);  /* single opening top { bracket */
            JSON_createMSGHEADEREntry(msgBody);
            JSON_createSTATUSUriEntry(msgBody);
            strcat(msgBody, ptrCloseBracket); /* single closing bottom } bracket */
            break;
        //SYSTEM URI
        default:
            break;
    }

    sprintf((char *)msgBodyLen, "%d\r\n\r\n", strlen(msgBody));
    strncat(buffer, msgBodyLen, strlen(msgBodyLen));
    strncat(buffer, msgBody, strlen(msgBody));
    

}

/*******************************************************************************
  Function:
    void insertKeyValuePairStringValue(char *theKey, char *theValue, unsigned char *body)

  Remarks:
    This routine formats the data in key value pairs (String Value)
*/
void insertKeyValuePairStringValue(char *theKey, char *theValue, unsigned char *body)
{
    strcat(body, theKey);
    strcat(body, theValue);

}

/*******************************************************************************
  Function:
    void insertKeyValuePairIntegerValue(char *theKey, unsigned int theValue, unsigned char *body)

  Remarks:
    This routine formats the data in key value pairs (Integer Value)
*/
void insertKeyValuePairIntegerValue(char *theKey, unsigned int theValue, unsigned char *body)
{
    unsigned char tempIntegerBuffer[32] = {'\0'};  /* needs to hold floating pt number */

    strcat(body, theKey);
    sprintf(tempIntegerBuffer, "%d", theValue);
    strcat(body, tempIntegerBuffer);
}


/*******************************************************************************
  Function:
    void JSON_createMSGHEADEREntry(unsigned char *body)

  Remarks:
    This routine formats the message header
*/
void JSON_createMSGHEADEREntry(unsigned char *body)
{
    char    macstring[20];
  
    strcat(body, tstatKeys.msgHeaderObjectKey);
    
    sprintf(macstring, "\"%s\"", moduleMACAddress);
    insertKeyValuePairStringValue(tstatKeys.uuidKey, macstring, body );
    strcat(body, ptrCloseBracket);
    strcat(body, ptrComma);
}

/*******************************************************************************
  Function:
    void JSON_createSTATUSUriEntry(unsigned char *body)

  Remarks:
    This routine formats the STATUS message
*/
void JSON_createSTATUSUriEntry(unsigned char *body)
{
    unsigned short int rValue;
    uint8_t switchValue = 0x00;
    
    strcat(body, tstatKeys.tstatusKey);

    /* pot */
    rValue = ReadAnalogPOT();
    insertKeyValuePairIntegerValue(tstatKeys.potvalueKey, (unsigned int)rValue, body);
    strcat(body, ptrComma);

    /*temp*/
    rValue = 0;
    rValue = ReadAnalogTemp();
    insertKeyValuePairIntegerValue(tstatKeys.analogTempKey, (unsigned int)rValue, body);
    strcat(body, ptrComma);

    /* switches */
   
    switchValue = PORTDbits.RD4;
    switchValue = switchValue | ((PORTDbits.RD13<<1)&0x02);
    insertKeyValuePairIntegerValue(tstatKeys.switchesKey, (unsigned int)(switchValue), body  );
    strcat(body, ptrCloseBracket);
 
    
}



