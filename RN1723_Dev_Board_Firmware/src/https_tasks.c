/*******************************************************************************
  Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    http_tasks.c

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


#include "../include/HardwareProfile.h"
#include "../include/https_tasks.h"
#include "../include/console.h"
#include "../include/Tick.h"
#include "../include/wifly_UART_drv.h"
#include <cyassl/internal.h>
#include "ca-cert.h"
#include <cyassl/ssl.h>


// *****************************************************************************
// *****************************************************************************
// Section: Local Functions
// *****************************************************************************
// *****************************************************************************
int _CyaSSL_Send_cb(void *sslin, char *buf, int sz, void *ctx);
int _CyaSSL_Receive_cb(void *sslin, char *buf, int sz, void *ctx);
void _CyaSSL_Error(HTTP_SOCKET* socket, int ret);

//extern char  OutString[128];
//extern TICK        epochTime;
//extern uint32_t    timeDiffMS;
/*******************************************************************************
  Function:
    int InitializeCyaSSL(HTTP_SOCKET* socket)

  Remarks:
    
 */
int InitializeCyaSSL(HTTP_SOCKET* socket)
{
    socket->triggerConnection = 0;
    socket->triggerRequestReady = 0;
    socket->triggerResponseReady = 0;
    socket->triggerShutdown = 0;

    /* Initialise CyaSSL.  This must be done before any other CyaSSL
     * functions are called. */
    CyaSSL_Init();

    /* Attempt to create a context that uses the TLS V1.2 server protocol. */
    socket->ctx = CyaSSL_CTX_new(CyaSSLv23_client_method());  
    //socket->ctx = CyaSSL_CTX_new(CyaTLSv1_client_method());        
    if (socket->ctx == NULL)
    {
        putsConsole("\r\n\err = Unable to get ctx\n");
        return 0;
    }
    //putsConsole("\r\nCreated Cyassl ctx");

    /* Load the CA certificate. */
    if (CyaSSL_CTX_load_verify_buffer(socket->ctx, caCert, caCert_len, SSL_FILETYPE_PEM) != SSL_SUCCESS)
    {
        putsConsole("\r\n\err = Unable to load CA-Cert, please check the file\n");
        return 0;
    }

    /* Hook up the send/receive callbacks */
    CyaSSL_SetIORecv(socket->ctx, (CallbackIORecv)&_CyaSSL_Receive_cb);
    CyaSSL_SetIOSend(socket->ctx, (CallbackIOSend)&_CyaSSL_Send_cb);

    /* Turn off verification */
    CyaSSL_CTX_set_verify(socket->ctx, SSL_VERIFY_NONE, 0);
    /*TURN ON verification*/
    //
    //CyaSSL_CTX_set_verify(socket->ctx, SSL_VERIFY_PEER, 0);

    socket->http_state = SM_HTTP_IDLE;

    //putsConsole("\r\nInitialize cyassl");
    return 1;
}

/*******************************************************************************
  Function:
    void HTTPSTasks(HTTP_SOCKET* socket)

  Remarks:

 */
void HTTPSTasks(HTTP_SOCKET* socket)
{
    char str[80];
    int ret, i;
    char  OutString[128];
    int  error;


    switch(socket->http_state)
    {
        case SM_HTTP_IDLE:
            //putsConsole("\r\nsm_http_idle");
            /* Wait for application to Trigger a Request */
            if (socket->triggerConnection == 1)
            {
                /* Set next state timeout */
                socket->state_timeout = TickGet() + TICK_SECOND * CONNECTION_TIMEOUT_SECS;

                socket->http_state = SM_ESTABLISH_CONNECTION;
                socket->triggerConnection = 0;
            }

            if (socket->triggerShutdown == 1)
                socket->http_state = SM_HTTP_CLOSE_SOCKET;

            break;

        case SM_ESTABLISH_CONNECTION:
            
            /* Check Establish Connection Timeout */
            if ((uint32_t)TickGet() > socket->state_timeout)
            {
                if (!RN_ASSOCIATED())
                {
                    //putsConsole("\r\n\err = Unable to Associate to AP\n");
                }
                else
                {
                    //sprintf(str,"\r\n\err = Unable to Open Socket: %d, : %s\n", socket->host, socket->port);
                    //putsConsole(str);
                }

                socket->http_state = SM_HTTP_IDLE;
            }
            else
            {
                /* Wait for Association to AP */
                if (RN_ASSOCIATED())
                {
                    /* Wait for Socket to Open */
                    if (RN_CONNECTION_STATUS() == SOCKET_OPEN)
                    {

                        /* Create a CyaSSL object for use with the newly connected socket. */

                        socket->ssl = CyaSSL_new(socket->ctx);
                        if (socket->ssl == 0)
                        {
                            //putsConsole("\r\n\err = Unable to Create new CyaSSL object for socket\n");
                            socket->http_state = SM_HTTP_ERROR;
                            socket->memfree++;
                            break;
                        }
                        socket->memfree = 0;
                        //putsConsole("\r\ncreated new cyassl object");
                        /* Flush out the RN RX Buffer */
                        FlushWiFlyRxBuffer();

                        socket->http_state = SM_HTTP_PREP_RSA;
                        //socket->http_state = SM_HTTP_SEND_RSA;
                    }
                    else
                    {
                        /* If NOT: OPEN the Socket */
                        RN_OPEN_SOCKET();
                    }
                }
            }

            break;

        case SM_HTTP_PREP_RSA:

#if defined(ENABLE_SSL_SESSIONS)
            if(socket->session != NULL)
            {


                putsConsole("\r\nRead SSL Session Info");
                XEEReadArray(SSL_SESSION_SESSIONID, (char *)&(socket->session->sessionID), 32);
                XEEReadArray(SSL_SESSION_MASTERSECRET, (char *)&(socket->session->masterSecret), 48);

                XEEReadArray(SSL_SESSION_SESSIONID, (char *)&socket->session->sessionID, 32 );
                XEEReadArray(SSL_SESSION_MASTERSECRET, (char *)&socket->session->masterSecret, 48 );
                XEEReadArray(SSL_SESSION_BORNON, (char *)&socket->session->bornOn, 4 );
                XEEReadArray(SSL_SESSION_TIMEOUT, (char *)&socket->session->timeout, 4 );
                XEEReadArray(SSL_SESSION_SERVERID, (char *)&socket->session->serverID, 20 );
                XEEReadArray(SSL_SESSION_IDLEN, (char *)&socket->session->idLen, 2 );
                
                putsConsole("\r\nsession ID: ");
                for(i=0; i<32; i++)
                {
                    sprintf(OutString, "%02x", socket->session->sessionID[i]);
                    putsConsole(OutString);
                }
            
                putsConsole("\r\nmaster Secret: ");
                for(i=0; i<48; i++)
                {
                    sprintf(OutString, "%02x", socket->session->masterSecret[i]);
                    putsConsole(OutString);
                }
                putsConsole("\r\nbornOn: ");
                sprintf(OutString, "%x", socket->session->bornOn);
                putsConsole(OutString);

                putsConsole("\r\ntimeout: ");
                sprintf(OutString, "%x", socket->session->timeout);
                putsConsole(OutString);

                putsConsole("\r\nServerID: ");
                for(i=0; i<20; i++)
                {
                    sprintf(OutString, "%02x", socket->session->serverID[i]);
                    putsConsole(OutString);
                }

                putsConsole("\r\nidLen: ");
                sprintf(OutString, "%x", socket->session->idLen);
                putsConsole(OutString);
            }
            
            /* sets session to old session */
            CyaSSL_set_session(socket->ssl, socket->session);
#endif
            
            socket->http_state = SM_HTTP_SEND_RSA;
             
            break;
            
                
        case SM_HTTP_SEND_RSA:
            
            /* Initiate SSL/TLS handshake with server */
            ret = CyaSSL_connect(socket->ssl);
            if (ret == SSL_SUCCESS)
            {
#if defined(ENABLE_SSL_SESSIONS)
                /* checks to see if the new session is the same as the old session */
                if (CyaSSL_session_reused(socket->ssl))
                {
                    putsConsole("\r\nRe-used session ID\n"); 
                    while(1);
                }
                else
                    putsConsole("\r\nDid not re-use session ID\n");
#endif
    
                socket->http_state = SM_HTTP_SOCKET_READY;
            }
            else
                _CyaSSL_Error(socket, ret);

            break;

        case SM_HTTP_SOCKET_READY:
            
            /* Wait for application to trigger Request Ready */
            if (socket->triggerRequestReady == 1)
            {

                socket->http_state = SM_HTTP_SEND_REQUEST;
            }

            if (socket->triggerShutdown == 1)
                socket->http_state = SM_HTTP_CLOSE_SOCKET;

            break;

        case SM_HTTP_SEND_REQUEST:
            
            if (CyaSSL_write(socket->ssl, socket->ucTxBuf, strlen(socket->ucTxBuf)) != strlen(socket->ucTxBuf))
            {
                _CyaSSL_Error(socket, ret);
            }
            else
            {
                socket->triggerRequestReady = 0;
                socket->http_state = SM_HTTP_GET_RESPONSE;
            }

            break;

        case SM_HTTP_GET_RESPONSE:
            if ((uint32_t)TickGet() > socket->state_timeout)
            {
                //putsConsole("\r\nRead Failure\r\n");
                socket->http_state = SM_HTTP_ERROR;
            }

            else
            {
                ret = CyaSSL_read(socket->ssl, socket->ucRxBuf, sizeof(socket->ucRxBuf)-1);
                if (ret < 0)
                {
                    error = CyaSSL_get_error(socket->ssl, ret);
                }
                else
                {
                    socket->ucRxBuf[ret] = 0;
                    socket->triggerResponseReady = 1;
                    //putsConsole(socket->ucRxBuf);
                }
            }

            break;

        case SM_HTTP_ERROR:
            
            socket->unexpectedSocketError = 1;
            socket->http_state = SM_HTTP_IDLE;

            break;

        case SM_HTTP_CLOSE_SOCKET:

#if defined(ENABLE_SSL_SESSIONS)
            /* saves the session */
            putsConsole("\r\nSave SSL Session Info");
            socket->session = CyaSSL_get_session(socket->ssl);
            
            putsConsole("\r\nsession ID: ");
            for(i=0; i<32; i++)
            {
                sprintf(OutString, "%02x", socket->session->sessionID[i]);
                putsConsole(OutString);
            }
            putsConsole("\r\nmaster Secret: ");
            for(i=0; i<48; i++)
            {
                sprintf(OutString, "%02x", socket->session->masterSecret[i]);
                putsConsole(OutString);
            }
            putsConsole("\r\nbornOn: ");
            sprintf(OutString, "%x", socket->session->bornOn);
            putsConsole(OutString);

            putsConsole("\r\ntimeout: ");
            sprintf(OutString, "%x", socket->session->timeout);
            putsConsole(OutString);
                
            putsConsole("\r\nServerID: ");
            for(i=0; i<20; i++)
            {
                sprintf(OutString, "%02x", socket->session->serverID[i]);
                putsConsole(OutString);
            }

            putsConsole("\r\nidLen: ");
            sprintf(OutString, "%x", socket->session->idLen);
            putsConsole(OutString);
            
            XEEWriteCompleteArray(SSL_SESSION_SESSIONID, (char *)&socket->session->sessionID, 32 );
            XEEWriteCompleteArray(SSL_SESSION_MASTERSECRET, (char *)&socket->session->masterSecret, 48 );
            XEEWriteCompleteArray(SSL_SESSION_BORNON, (char *)&socket->session->bornOn, 4 );
            XEEWriteCompleteArray(SSL_SESSION_TIMEOUT, (char *)&socket->session->timeout, 4 );
            XEEWriteCompleteArray(SSL_SESSION_SERVERID, (char *)&socket->session->serverID, 20 );
            XEEWriteCompleteArray(SSL_SESSION_IDLEN, (char *)&socket->session->idLen, 2 );
#endif
          
            //putsConsole("\r\nsm_http_close_socket");
            /* Close Socket */
            RN_CLOSE_SOCKET();


            //putsConsole("\r\nFree cyassl");
            /* CyaSSL objects should be deleted when they are no longer required */
            CyaSSL_free(socket->ssl);

            /* CyaSSL context should be deleted if it is no longer required */
            CyaSSL_CTX_free( socket->ctx );

            /* The library itself should be shut down cleanly if it too is no
             * longer required */
            CyaSSL_Cleanup();
            
            socket->triggerShutdown = 0;
            socket->http_state = SM_HTTP_IDLE;
            break;

        default:
            break;
       
    }
}

/*******************************************************************************
  Function:
    int _CyaSSL_Receive_cb(void *ssl, char *buf, int sz, void *ctx)

  Remarks:

 */
int _CyaSSL_Receive_cb(void *ssl, char *buf, int sz, void *ctx)
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

/*******************************************************************************
  Function:
    int _CyaSSL_Send_cb(void *ssl, char *buf, int sz, void *ctx

  Remarks:

 */
int _CyaSSL_Send_cb(void *ssl, char *buf, int sz, void *ctx)
{
    // Check to see how much data can be sent
//
//    // If none can be sent return ERROR
//    if (bufferSize == 0)
//    {
//        return CYASSL_CBIO_ERR_WANT_WRITE;
//    }

    // Send the data to the RN171
//    putbWiFly (buf, sz);
    putb_WiFlyUART(buf, sz);

    // Return the ammount of data Sent
    return sz;
}

/*******************************************************************************
  Function:
    void _CyaSSL_Error(HTTP_SOCKET* socket, int ret)

  Remarks:

 */
void _CyaSSL_Error(HTTP_SOCKET* socket, int ret)
{
    int  err;
    char str[160];
    char errString[100];

    err = CyaSSL_get_error(socket->ssl, ret);

    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
    ;
    else
    {
        CyaSSL_ERR_error_string(err, errString);
        //sprintf(str,"\r\n\err = %d, %s\n", err, errString);
        //putsConsole(str);
        //sprintf(str,"\r\nCurrent http state -%d\n", socket->http_state);
        //putsConsole(str);
        socket->http_state = SM_HTTP_ERROR;
        socket->triggerConnection = 0;
    }
}
