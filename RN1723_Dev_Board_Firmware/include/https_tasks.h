/*******************************************************************************
  The main application tasks state machines definitions

  Company:
    Microchip Technology Inc.

  File Name:
    http_tasks.h

  Summary:
    Function prototypes for  http application tasks

  Description:
     The function prototypes that make up the http application tasks, as well
     as the state machine states for the tasks are defined in this file.
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
#ifndef __HTTPSTASKS_H__
#define __HTTPSTASKS_H__

#include <stdint.h>
#include <cyassl/ssl.h>


#define CONNECTION_TIMEOUT_SECS     15

#define MAX_TXLINE                  1024
#define MAX_RXLINE                  1024

//#define ENABLE_SSL_SESSIONS


/* HTTPS State Machine States */
typedef enum
{
    SM_HTTP_IDLE = 0,
    SM_ESTABLISH_CONNECTION,
    SM_HTTP_PREP_RSA,
    SM_HTTP_SEND_RSA,
    SM_HTTP_SOCKET_READY,
    SM_HTTP_SEND_REQUEST,
    SM_HTTP_GET_RESPONSE,
    SM_HTTP_RESPONSE_COMPLETE,
    SM_HTTP_CLOSE_SOCKET,
    SM_HTTP_ERROR,
}HTTP_STATES;


typedef struct
{
    HTTP_STATES http_state;
    uint32_t state_timeout;
    void* ctx;
    void* ssl;
    CYASSL_SESSION* session;
    uint32_t host;
    uint32_t port;
    uint32_t triggerConnection;
    uint32_t triggerShutdown;
    uint32_t triggerRequestReady;
    uint32_t triggerResponseReady;
    uint32_t unexpectedSocketError;
    uint32_t memfree;
    char ucTxBuf[MAX_TXLINE];
    char ucRxBuf[MAX_RXLINE];
    uint32_t ucTxCount;
    uint32_t ucRxCount;
} HTTP_SOCKET;


#define HTTP_RESPONSE_200               "HTTP/1.1 200 OK"
#define HTTP_RESPONSE_303               "HTTP/1.1 303 See Other"
#define HTTP_HEADER_END                 "\r\n\r\n"


int InitializeCyaSSL(HTTP_SOCKET* socket);
void HTTPSTasks(HTTP_SOCKET* client);


#endif
