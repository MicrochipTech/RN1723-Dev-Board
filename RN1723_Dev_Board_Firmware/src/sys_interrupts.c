#include <p32xxxx.h>
#include <plib.h>
#include "../include/HardwareProfile.h"
#include "../include/wifly_UART_drv.h"
#include "../include/sys_tasks.h"
#include "../include/Tick.h"
#include "../include/app.h"



void _general_exception_handler (DWORD cause, DWORD status)
{
    while(1);                                                                                                                                                                                               
    return;
}

void __ISR(_TIMER_1_VECTOR, ipl4) _T1Interrupt ( void )
{
    IFS0CLR = _IFS0_T1IF_MASK;
}

void __ISR(_TIMER_2_VECTOR, ipl4) _T2Interrupt ( void )
{
    TXTimerIntHandler();
    IFS0CLR = _IFS0_T2IF_MASK;
}

void __ISR(_TIMER_5_VECTOR, ipl4) _T5Interrupt(void)
{
    TickIntHandler();
}

void __ISR(UART_WIFLY_INT_VECTOR, WIFLY_IPL) RNUartInterrupt(void)
{
    IntHandler_WiFlyUART();
}

void __ISR(UART_CONSOLE_INT_VECTOR, UART_CONSOLE_IPL) DebugUartInterrupt(void)
{
    ConsoleUartIntHandler();
}

void __ISR(INPUT_CAPTURE3_INT_VECTOR, IC3_IPL) GPIO9Interrupt(void)
{
    GPIO9IntHandler();
   
}
