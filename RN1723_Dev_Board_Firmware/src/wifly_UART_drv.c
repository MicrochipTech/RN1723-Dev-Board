#include <p32xxxx.h>
#include <GenericTypeDefs.h>
#include <plib.h>
#include "../include/HardwareProfile.h"
#include "../include/Tick.h"
#include "../include/wifly_UART_drv.h"
#include "../include/console.h"
#include "../include/app.h"
#include <stdint.h>




typedef enum u2State_t_def
{
    SM_WIFLY_SEND,
    SM_WIFLY_TX_WAIT,
    SM_WIFLY_WAIT_DONE
}WIFLY_UART_STATE;

RNState_t sRNState = sRNInit;


WIFLY_UART_STATE WiflyState = SM_WIFLY_SEND;
WIFLY_UART_STATE WiflyRxDataState = SM_WIFLY_SEND;


#define TX_BUFF_SIZE            600
#define RX_BUFF_SIZE            4072//1024


WORD Tx2Count=0;
WORD Tx2Len=0;
BYTE Tx2Buffer[TX_BUFF_SIZE];
BOOL Tx2Active = FALSE;

WORD Rx2Count=0;
BYTE Rx2Buffer[RX_BUFF_SIZE];

BYTE RxData[RX_BUFF_SIZE];

//WORD InterByteDelay2=0;

BOOL TxTimerTriggered=FALSE;

WORD pStartIndex=0;
//WORD pEndIndex=0;
UINT16 TailCount=0;

BOOL _MatchStringLenInBuffer(UINT16 Count);
void _Set2RxIndex(WORD *Index,WORD Value);
BYTE _Rx2Buff(WORD Index);
//void ProcessBGMRecord(BYTE *Data);
void _StartTXTimer(void);
WORD _WiFlyGetRxCount(WORD sIndex,WORD pEndIndex);

extern char  OutString[128];
extern BOOL RN_UartCmdMode;


/*******************************************************************************
  Function:
    void Initialize_WiFlyUART( UINT32 baud_rate)

  Remarks:
    This routine Initiallizes the RN UART Interface Settings
*/
void Initialize_WiFlyUART( UINT32 baud_rate)
{
    /* Initiallize RN GPIO's */
    Init_RN_GPIO();

    TRIS_RN_GPIO8 = 1;
    /* Initiallize RN UART Flow Control Pins */
    Init_RN_FlowCtrl();

    /* Set up the UART that will be used to communicate with the RN module */
    UARTConfigure(UART_WIFLY, UART_ENABLE_PINS_TX_RX_ONLY | UART_ENABLE_HIGH_SPEED);
    UARTSetFifoMode(UART_WIFLY, UART_INTERRUPT_ON_TX_BUFFER_EMPTY | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_WIFLY, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_WIFLY, GetPeripheralClock(), baud_rate);
    UARTEnable(UART_WIFLY, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    /* Configure WIFLY UART TX/RX Interrupts */
    INTSetVectorPriority(INT_VECTOR_UART(UART_WIFLY), INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_WIFLY), INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_ENABLED);
    INTEnable(INT_SOURCE_UART_TX(UART_WIFLY), INT_ENABLED);
}

// *****************************************************************************
// send a null terminated string to the Wifly Serial Port
// *****************************************************************************
//void putsWiFly(char *s)
//{
//    puts_WiFlyUART(s);
//    while (WiFlyTXActive()==TRUE);  //Block till done
//}


// *****************************************************************************
// send a null terminated string to the Wifly Serial Port
// *****************************************************************************
//void putbWiFly(UINT8 *s, UINT16 size)
//{
//    putb_WiFlyUART(s,size);
//    while (WiFlyTXActive()==TRUE);  //Block till done
//}

WIFLY_Result_t PutCMDSet_WiFlyUART(WiFlyCmdSet_t *WiFlyCommands)
{
    WIFLY_Result_t status=Result_BUSY;
    WIFLY_Result_t result;
    static UINT16 index=0;
    static TICK tStart;
    static TICK delayTime;
    TICK delayTicks;

    switch(sRNState)
    {
        case sRNInit:
            index = 0;
            sRNState = sRNSend;
            //break;

        case sRNSend:
            result = PutCMD_WiFlyUART(WiFlyCommands[index].Command,WiFlyCommands[index].Response,WiFlyCommands[index].CommandTimeout);
            if((result == Result_TIMEOUT) || (result == Result_ERROR))
            {
                // Failed
                status = result;
                sRNState = sRNInit;// Restart
            }
            else if(result == Result_SUCCESS)
            {
                delayTicks = WiFlyCommands[index].DelayAfterCommand;
                index++;
                if(WiFlyCommands[index].Command[0] == 0)
                {
                    //Last Command!
                    if(delayTicks != 0)
                    {
                        delayTime = delayTicks;
                        tStart = TickGet();
                        sRNState = sRNDelayAfterSend;
                    }
                    else
                    {
                        status = Result_SUCCESS;
                        sRNState = sRNInit;
                    }
                }
                else
                {
                    if(delayTicks != 0)
                    {
                        delayTime = delayTicks;
                        tStart = TickGet();
                        sRNState = sRNDelayAfterSend;
                    }
                }
            }
            break;

        case sRNDelayAfterSend:
            if(TickGetDiff(TickGet(),tStart) > delayTime)
            {
                if(WiFlyCommands[index].Command[0] == 0)
                {
                    // Last command!
                    status = Result_SUCCESS;
                    sRNState = sRNInit;
                }else
                {
                    sRNState = sRNSend;
                }
            }
            break;
    }
    return(status);
}

/* Send a Wifly Command to RN and waits for expected Response */
WIFLY_Result_t PutCMD_WiFlyUART(char* Command, char* Response, DWORD WaitTime)
{
    WIFLY_Result_t ReturnVal = Result_BUSY;
    static TICK tStart;

    switch(WiflyState)
    {
        case SM_WIFLY_SEND:
            //memset(Rx2Buffer,0x00,sizeof(Rx2Buffer));//Debug!
            puts_WiFlyUART(Command);
            FlushWiFlyRxBuffer();
            WiflyState = SM_WIFLY_TX_WAIT;
        break;

        case SM_WIFLY_TX_WAIT:
            if(Tx2Active == FALSE)
            {
                tStart = TickGet();
                WiflyState = SM_WIFLY_WAIT_DONE;
            }
        break;

        case SM_WIFLY_WAIT_DONE:
            if(strlen(Response) == 0)
            {
                ReturnVal = Result_SUCCESS;
                WiflyState = SM_WIFLY_SEND;
            }
            else if(TickGetDiff(TickGet(),tStart) >= WaitTime)
            {
                ReturnVal = Result_TIMEOUT;
                WiflyState = SM_WIFLY_SEND;
            }
            else if(_MatchStringLenInBuffer(strlen(Response)) == TRUE)
            {
                if(strstr((char*)RxData,Response) != NULL)
                {
                    _Set2RxIndex(&pStartIndex, pStartIndex + strlen(Response)-1);
                    WiflyState = SM_WIFLY_SEND;
                    ReturnVal = Result_SUCCESS;
                }
                /*else if(strcmp((char*)RxData,"ERROR\r\n")==0)
                {
                    ReturnVal=Result_ERROR;
                }else
                {
                    ReturnVal=Result_TIMEOUT;
                }
                u2State=u2_Send;*/
            }
        break;
    }

    return(ReturnVal);
}

void IntHandler_WiFlyUART(void) {
    UINT8 bData;

    /* Check if UART Recieve Interrupt */
    if (INTGetFlag(INT_SOURCE_UART_RX(UART_WIFLY))) 
    {
        /* While the receiver has data, put it in the Recieve Buffer */
        while (UARTReceivedDataIsAvailable(UART_WIFLY) == TRUE)
        {
            bData = UARTGetDataByte(UART_WIFLY);

            if (Rx2Count < RX_BUFF_SIZE)
            {
                Rx2Buffer[Rx2Count++] = bData;

                if (Rx2Count >= RX_BUFF_SIZE)
                {
                    Rx2Count -= RX_BUFF_SIZE;
                }
            }
            if(RN_UartCmdMode == 1)
            {
                putConsole(bData);
            }
        }

        /* Clear the RX Interrupt Flag */
        INTClearFlag(INT_SOURCE_UART_RX(UART_WIFLY));
    }

    /* Check if UART TX Interrupt */
    if ((INTGetFlag(INT_SOURCE_UART_TX(UART_WIFLY))) && (INTGetEnable(INT_SOURCE_UART_TX(UART_WIFLY))))
    {
        while (UARTTransmitterIsReady(UART_WIFLY) == TRUE) 
        {
            // Disable TX Interrupt for interbyte delay
            INTEnable(INT_SOURCE_UART_TX(UART_WIFLY), INT_DISABLED);

            if (((Tx2Len == 0) && (Tx2Buffer[Tx2Count] != 0)) || ((Tx2Len > 0) && (Tx2Count < Tx2Len)))
            {
                bData = Tx2Buffer[Tx2Count];

                TxTimerTriggered = TRUE; /* dpl changed */

                if (PIC_CTS == 0)
                {
                    // OK To send immediately!
                    TxTimerTriggered = TRUE;
                }
                if (TxTimerTriggered == TRUE)
                {
                    UARTSendDataByte(UART_WIFLY, Tx2Buffer[Tx2Count++]);

                    INTEnable(INT_SOURCE_UART_TX(UART_WIFLY), INT_ENABLED);
                } 
                else
                {
                    // Wait for the timer!!!
                    _StartTXTimer();
                }
            } 
            else
            {
                Tx2Len = 0;
                Tx2Active = FALSE;
            }

            break; //For now always send 1 byte only!!!
        }

        TxTimerTriggered = FALSE;
        
        /* Clear the TX Interrupt Flag */
        INTClearFlag(INT_SOURCE_UART_TX(UART_WIFLY));
    }
}

void _StartTXTimer(void)
{
    CloseTimer2();
    WriteTimer2(0x00);
    OpenTimer2((T2_ON|T2_32BIT_MODE_OFF | T2_PS_1_256),GET_PERIOD_PRE(RN_MICROSECONDS_BETWEEN_BYTES,256));

    mT2SetIntPriority(4);
    mT2ClearIntFlag();
    mT2IntEnable (1);
    TxTimerTriggered=FALSE;
}


void TXTimerIntHandler(void)
{
    TxTimerTriggered = TRUE;
    INTEnable(INT_SOURCE_UART_TX(UART_WIFLY), INT_ENABLED);
    mT2IntEnable (0);
}

/* Send a String of characters to RN */
void puts_WiFlyUART(char* data)
{
    WORD i = strlen(data);
    strcpy((char*)Tx2Buffer,(char*)data);

    Tx2Buffer[i++] = 0;

    Tx2Len = 0;
    Tx2Count = 0;
    Tx2Active = TRUE;
    
    INTEnable(INT_SOURCE_UART_TX(UART_WIFLY), INT_ENABLED);
}


void putb_WiFlyUART(UINT8* buffer,UINT16 size)
{
    memcpy(Tx2Buffer,buffer,size);
    Tx2Len = size;
    Tx2Count = 0;
    Tx2Active = TRUE;
    INTEnable(INT_SOURCE_UART_TX(UART_WIFLY), INT_ENABLED);
    //IFS1bits.U2TXIF=1;
    //IEC1bits.U2TXIE = 1;
}



WIFLY_Result_t SendReqRxData(char* Attribute, char*Response, DWORD WaitTime)
{
    WIFLY_Result_t ReturnVal=Result_BUSY;
    static TICK tStart;

    switch(WiflyRxDataState)
    {
        case SM_WIFLY_SEND:
            puts_WiFlyUART(Attribute);
            FlushWiFlyRxBuffer();
            WiflyRxDataState=SM_WIFLY_TX_WAIT;
            break;

        case SM_WIFLY_TX_WAIT:
            if(Tx2Active==FALSE)
            {
                tStart=TickGet();
                WiflyRxDataState=SM_WIFLY_WAIT_DONE;
            }
            break;

        case SM_WIFLY_WAIT_DONE:
            if(TickGetDiff(TickGet(),tStart)>=WaitTime)
            {
                ReturnVal=Result_TIMEOUT;
                WiflyRxDataState=SM_WIFLY_SEND;
            }
            else if(_MatchStringLenInBuffer(strlen(Response))==TRUE)
            {
                tStart=TickGet();//Got a line!
                if(strcmp((char*)RxData,Response)==0)
                {
                    ReturnVal=Result_SUCCESS;
                    WiflyRxDataState=SM_WIFLY_SEND;
                }
                else if(strcmp((char*)RxData,"ERROR\r\n")==0)
                {
                    ReturnVal=Result_ERROR;
                    WiflyRxDataState=SM_WIFLY_SEND;
                }else
                {
                    //Process Record Here
                    //ProcessRecord(RxData);
                    //TODO:  Add Stream handling for file download!!!
                }
            }
            break;
    }

    return(ReturnVal);
}

/* Flushes the RN Recieve Buffer */
void FlushWiFlyRxBuffer(void)
{
    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_DISABLED);
    pStartIndex = Rx2Count;
    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_ENABLED);
}

// Returns NULL if no data is available
// Otherwise returns all bytes in the buffer upto len in count
UINT16 getsWiFly_noWait(char *s, UINT16 len)
{
    uint32_t i;
    uint32_t dtCount;

    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_DISABLED);
    dtCount=_WiFlyGetRxCount(pStartIndex,Rx2Count);
    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_ENABLED);
    if(dtCount>len)
    {
        dtCount=len;
    }
    if(dtCount>0)
    {
        for(i=0;i<dtCount;i++)
        {
            s[i]=_Rx2Buff(pStartIndex+i);
        }
        _Set2RxIndex(&pStartIndex,pStartIndex+dtCount);
    }
    return(dtCount);
}


BOOL _MatchStringLenInBuffer(UINT16 Count)
{
    BOOL retVal=FALSE;
    WORD dtCnt;
    WORD i;


    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_DISABLED);
    dtCnt=_WiFlyGetRxCount(pStartIndex,Rx2Count);
    INTEnable(INT_SOURCE_UART_RX(UART_WIFLY), INT_ENABLED);
    if(dtCnt>=RX_BUFF_SIZE-1)
    {
        FlushWiFlyRxBuffer();
        dtCnt=0;
    }

    if(dtCnt>0)
    {
        for(i=0;i<dtCnt;i++)
        {
            RxData[i]=_Rx2Buff(pStartIndex+i);
        }//For
TailCount=i;
        if(dtCnt>=Count)
        {
            RxData[Count]=0;
            _Set2RxIndex(&pStartIndex,pStartIndex+1);//Shift by Count (ignore all found characters)
            retVal=TRUE;
        }
    }
    return(retVal);
}


BYTE _Rx2Buff(WORD Index)
{
    if(Index>=RX_BUFF_SIZE)
    {
        Index-=RX_BUFF_SIZE;
    }
    return(Rx2Buffer[Index]);
}


WORD WiFlyDataAvailable(void)
{
    return(_WiFlyGetRxCount(pStartIndex,Rx2Count));
}


char* WiFly_WaitString(char *String, TICK Timeout)
{
    TICK tStart;
    BOOL ContinueWait=TRUE;
    char *ptr=NULL;
    WORD i=0;

    memset(RxData,0x00,sizeof(RxData));
    tStart=TickGet();

    do
    {
        if(TickGetDiff(TickGet(),tStart)>Timeout)
        {
         ContinueWait=FALSE;
        }
        else if(WiFlyDataAvailable()>0)
        {
            getsWiFly_noWait(&RxData[i++],1);
            RxData[i]=0;
            if(strstr((char*)RxData,String)!=NULL)
            {
                //Found string!
                ContinueWait=FALSE;
                ptr=RxData;
            }
            else
            {
                if(i>=(sizeof(RxData)-1))
                {
                    //Too much data, move on
                    i=0;
                    memset(RxData,0x00,sizeof(RxData));
                }
            }
        }
    } while (ContinueWait==TRUE);
    return(ptr);
}

WORD _WiFlyGetRxCount(WORD sIndex,WORD pEndIndex)
{
    WORD retVal=0;

    if(sIndex<=pEndIndex)
    {
        retVal=(pEndIndex-sIndex);
    }
    else
    {
        retVal=RX_BUFF_SIZE-sIndex+pEndIndex;
    }
    return(retVal);
}

void _Set2RxIndex(WORD *Index,WORD Value)
{
    if(Value>=RX_BUFF_SIZE)
    {
        Value-=RX_BUFF_SIZE;
    }
    *Index=Value;
}

BOOL WiFlyTXActive(void)
{
    return(Tx2Active);
}
