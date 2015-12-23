/*  *************************************************************/
/*  MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:  You may use this */
/*  software, and any derivatives created by any person or      */
/*  entity by or on your behalf, exclusively with Microchip’s   */
/*  products.  Microchip and its licensors retain all ownership */
/*  and intellectual property rights in the accompanying        */
/*  software and in all derivatives hereto.                     */
/*                                                              */
/*  This software and any accompanying information is for       */
/*  suggestion only.  It does not modify Microchip’s standard   */
/*  warranty for its products.  You agree that you are solely   */
/*  responsible for testing the software and determining its    */
/*  suitability.  Microchip has no obligation to modify, test,  */
/*  certify, or support the software.                           */
/*                                                              */
/*  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".             */
/*  NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,       */
/*  INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF        */
/*  NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A        */
/*  PARTICULAR PURPOSE APPLY TO THIS SOFTWARE, ITS              */
/*  INTERACTION WITH MICROCHIP’S PRODUCTS, COMBINATION          */
/*  WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.         */
/*                                                              */
/*  IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN           */
/*  CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH    */
/*  OF STATUTORY DUTY), STRICT LIABILITY, INDEMNITY,            */
/*  CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL,      */
/*  PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS,      */
/*  DAMAGE, FOR COST OR EXPENSE OF ANY KIND WHATSOEVER          */
/*  RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF          */
/*  MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE        */
/*  DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE   */
/*  BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY    */
/*  WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF  */
/*  FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR  */
/*  THIS SOFTWARE.                                              */
/*                                                              */
/*  MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR    */
/*  ACCEPTANCE OF THESE TERMS.                                  */
/*  *************************************************************/



/************************ HEADERS **********************************/
#include <p32xxxx.h>
#include <plib.h>
#include "../include/tick.h"
#include "GenericTypeDefs.h"

/************************ DEFINITIONS ******************************/

/************************ FUNCTION PROTOTYPES **********************/

/************************ VARIABLES ********************************/

volatile TICK timerExtension=0;

/************************ FUNCTIONS ********************************/

/*********************************************************************
* Function:         void TickInit()
*
* PreCondition:     none
*
* Input:            none
*
* Output:           none
*
* Side Effects:	    None
*
* Overview:         Use timer 4/5 as tick timers to measure and keep track of time.
*
* Note:             The timer interrupt is enabled causing the timer
*                   roll over calculations.  Interrupts are required
*                   to be enabled in order to extend the timer to
*                   4 bytes. 
********************************************************************/
void TickInit()
{
    CloseTimer4();
    CloseTimer5();
    WriteTimer4(0x00);
    OpenTimer4((T4_ON|T4_32BIT_MODE_ON|CLOCK_DIVIDER_SETTING),0xFFFFFFFF);
    IEC0SET =   _IEC0_T5IE_MASK;
}


/*********************************************************************
* Function:         void TickInit()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    TICK - the current symbol time
*
* Side Effects:	    PIC18 only: the timer interrupt is disabled
*                   for several instruction cycles while the
*                   timer value is grabbed.  This is to prevent a
*                   rollover from incrementing the timer extenders
*                   during the read of their values
*
* Overview:		    This function returns the current time
*
* Note:			    PIC18 only:
*                   caution: this function should never be called
*                   when interrupts are disabled if interrupts are
*                   disabled when this is called then the timer
*                   might rollover and the byte extension would not
*                   get updated.
********************************************************************/
TICK currentTime;
TICK TickGet(void)
{
    DWORD IntFlag1=0;
    DWORD IntFlag2=0;
    WORD int_status;

    int_status=INTDisableInterrupts();
    /* disable the timer int to prevent roll over of the lower 16 bits while before/after reading of the extension */
    IEC0CLR=_IEC0_T5IE_MASK;

    do
    {
        IntFlag1=(IFS0 & _IEC0_T5IE_MASK);
        currentTime=(currentTime & 0xFFFFffff00000000ULL) + (TMR4 & 0xffffffff);
        IntFlag2=(IFS0 & _IEC0_T5IE_MASK);

    } while(IntFlag1!=IntFlag2);

    if(IntFlag2>0)
    {
        timerExtension++;
        IFS0CLR=_IEC0_T5IE_MASK;
    }

    currentTime= (timerExtension<<32) | (currentTime & 0xffffffff);

    IEC0SET =_IEC0_T5IE_MASK;
    INTRestoreInterrupts(int_status);

    return currentTime;
}

TICK TickGetNoInt(void)
{
    TICK currentTime;
    /* disable the timer to prevent roll over of the lower 16 bits while before/after reading of the extension */
    //T4CONCLR=T4_ON;
    DWORD IntFlag1=0;
    DWORD IntFlag2=0;

    /* disable the timer int to prevent roll over of the lower 16 bits while before/after reading of the extension */
    IEC0CLR=_IEC0_T5IE_MASK;

    do
    {
        IntFlag1=(IFS0 & _IEC0_T5IE_MASK);
        currentTime=(currentTime & 0xFFFFffff00000000ULL) + (TMR4 & 0xffffffff);
        IntFlag2=(IFS0 & _IEC0_T5IE_MASK);

    } while(IntFlag1!=IntFlag2);

    if(IntFlag2>0)
    {
        timerExtension++;
        IFS0CLR=_IEC0_T5IE_MASK;
    }

    currentTime= (timerExtension<<32) | (currentTime & 0xffffffff);

    IEC0SET =_IEC0_T5IE_MASK;

    return currentTime;
}

void TickIntHandler(void)
{
    IFS0bits.T5IF=0;
    timerExtension++;
}
