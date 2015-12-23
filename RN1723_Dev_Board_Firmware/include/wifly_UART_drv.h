#ifndef _RN_UART_H_
#define _RN_UART_H_

#include "Tick.h"

typedef enum
{
    sRNInit,
    sRNSend,
    sRNDelayAfterSend
}RNState_t;

typedef struct
{
    char Command[45];
    char Response[25];
    TICK CommandTimeout;
    TICK DelayAfterCommand;
}WiFlyCmdSet_t;


typedef enum
{
    Result_BUSY,
    Result_SUCCESS,
    Result_ERROR,
    Result_TIMEOUT
}WIFLY_Result_t;

void TXTimerIntHandler(void);
void IntHandler_WiFlyUART(void);


//WiFly compatibility Modes

//Returns 0 if no data is available
//Otherwise returns all bytes in the buffer upto len in count
UINT16 getsWiFly_noWait(char *s, UINT16 len);
WIFLY_Result_t PutCMD_WiFlyUART(char* Attribute, char*Response, DWORD WaitTime);
//    void _putsWiFly(char* str);
//    void _putbWiFly(UINT8 * buffer,UINT16 size);
BOOL WiFlyTXActive(void);
WORD WiFlyDataAvailable(void);
char* WiFly_WaitString(char *String, TICK Timeout);   //Returns Null if fails
void FlushWiFlyRxBuffer(void);

// init the PIC I/O and UART settings
void Initialize_WiFlyUART(UINT32 baud_rate);

// send a null terminated string to the serial port
void puts_WiFlyUART(char* data);

// send binary data to serial port with size
void putbWiFly(UINT8 *s, UINT16 size);


WIFLY_Result_t PutCMDSet_WiFlyUART(WiFlyCmdSet_t *WiFlyCommands);

#endif