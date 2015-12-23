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



#ifndef __SYMBOL_TIME_H_
#define __SYMBOL_TIME_H_

/************************ HEADERS **********************************/

#include <p32xxxx.h>
#include <plib.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"

/************************ DEFINITIONS ******************************/
#if defined(__PIC32MX__)
    /* this section is based on the Timer 2/3 module of the PIC32MX family */
        #if(GetInstructionClock() <= 125000)
            #define CLOCK_DIVIDER 1
            #define CLOCK_DIVIDER_SETTING 0x0000 /* no prescalar */
            #define SYMBOL_TO_TICK_RATE 125000
        #elif(GetInstructionClock() <= 1000000)
            #define CLOCK_DIVIDER 8
            #define CLOCK_DIVIDER_SETTING 0x0030
            #define SYMBOL_TO_TICK_RATE 1000000
        #elif(GetInstructionClock() <= 8000000)
            #define CLOCK_DIVIDER 64
            #define CLOCK_DIVIDER_SETTING 0x0060
            #define SYMBOL_TO_TICK_RATE 8000000
        #elif(GetInstructionClock() <= 16000000)
            #define CLOCK_DIVIDER 256
            #define CLOCK_DIVIDER_SETTING 0x70
            #define SYMBOL_TO_TICK_RATE GetInstructionClock()
        #else
            #define CLOCK_DIVIDER 256
            #define CLOCK_DIVIDER_SETTING 0x70
            #define SYMBOL_TO_TICK_RATE GetInstructionClock()
        #endif

        #define ONE_SECOND (GetInstructionClock()/CLOCK_DIVIDER) //((GetInstructionClock()/1000 * 62500) / (SYMBOL_TO_TICK_RATE / 1000))
        /* SYMBOLS_TO_TICKS to only be used with input (a) as a constant, otherwise you will blow up the code */
        #define SYMBOLS_TO_TICKS(a) (((DWORD)(GetInstructionClock()/1000) * a) / (SYMBOL_TO_TICK_RATE / 1000))
#else
    #error "Unsupported processor.  New timing definitions required for proper operation"
#endif

#define SEC(a)          (ONE_SECOND*a)
#define MS(a)           ((ONE_SECOND*a)/1000)


#define TickGetDiff(a,b) (a - b)

typedef UINT64 TICK;

void TickInit(void);
TICK TickGet(void);
TICK TickGetNoInt(void);
void TickIntHandler(void);

#define TICK_SECOND     SEC(1)
#define TICK_MILLISECOND MS(1)


/************************ VARIABLES ********************************/
extern volatile BYTE timerExtension1,timerExtension2;
#endif
