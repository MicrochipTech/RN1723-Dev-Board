/*******************************************************************************
  System clock frequency definitions

  Company:
    Microchip Technology Inc.

  File Name:
    sys_config.h

  Summary:
    System clock frequency definitions - valid after reset and initialization.


  Description:
      System clock frequency definitions - valid after reset and initialization.
      Must be supported by a combination of config bit settings as well as run-time
      initialization.
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

#ifndef __SYSCONFIG_H__
#define __SYSCONFIG_H__

#include <plib.h>
#include "GenericTypeDefs.h"
#include "Compiler.h"

#define USE_ASYNC_DRIVER    //Enables Async Driver operation across all modules

#define DEBUG_CONSOLE
#define DEBUG_ALLOW_USER_INPUT //Allows user to type from terminal window
                                //Data will be sent to RN's uart port.


#define  SLEEP_RN_KILL_POWER_TO_PIC

// *****************************************************************************
// *****************************************************************************
// Section: System Macros
// *****************************************************************************
// *****************************************************************************
#define SYS_CLOCK_FREQ                  60000000ul
#define PB_DIVISOR                      1
#define GetSystemClock()                (SYS_CLOCK_FREQ)
#define GetPeripheralClock()            (GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define GetInstructionClock()           (GetSystemClock()/PB_DIVISOR)

// Uart Console hardware config
#define UART_CONSOLE                    UART3
#define CONSOLE_BAUDRATE                115200 //9600    // baud rate setting
#define UART_CONSOLE_INT_VECTOR         _UART_3_VECTOR
#define UART_CONSOLE_IPL                IPL2SOFT


// *****************************************************************************
//  wifly_drv hardware config
// *****************************************************************************
#define UART_WIFLY                      UART1
#define UART_WIFLY_INT_VECTOR           _UART_1_VECTOR
#define WIFLY_FACTORY_RATE              9600
#define WIFLY_BAUD_RATE_FAST            115200
#define WIFLY_IPL                       IPL4

//GPIO 9
#define INPUT_CAPTURE3                  3
#define INPUT_CAPTURE3_INT_VECTOR       _INPUT_CAPTURE_3_VECTOR
#define IC3_IPL                         IPL1


// RN171 RESET input (PIC output)
// module has internal 100kOhm pull-up
#define RN_RESET_LOW()                  {LATAbits.LATA2 = 0; mPORTASetPinsDigitalOut(BIT_2);}
#define RN_RESET_HIGH()                 {LATAbits.LATA2 = 1; mPORTASetPinsDigitalIn(BIT_2);}

// GPIO9 has Internal Pull-down
#define RN_GPIO9_HIGH()                 {LATDbits.LATD10 = 1; mPORTDSetPinsDigitalOut(BIT_10);}
#define RN_GPIO9_LOW()                  {mPORTDSetPinsDigitalIn(BIT_10);}

// GPIO9 has Internal Pull-down
#define RN_WAKE()                       {mPORTDSetPinsDigitalIn(BIT_7);}
#define RN_SLEEP()                      {LATDbits.LATD7 = 1; mPORTDSetPinsDigitalOut(BIT_7);}

// RN171 GPIO4 output (PIC input)
// Used to indicate association status
#define RN_GPIO4_Init()                 {mPORTDSetPinsDigitalIn(BIT_0);}
#define RN_ASSOCIATED()                 (PORTDbits.RD0)

// RN171 GPIO5 input (PIC output)
// Used to trigger remote IP host connection
#define RN_GPIO5_Init()                 RN_CLOSE_SOCKET()
#define RN_OPEN_SOCKET()                {LATCbits.LATC4 = 1; mPORTCSetPinsDigitalOut(BIT_4);}
#define RN_CLOSE_SOCKET()               {LATCbits.LATC4 = 0; mPORTCSetPinsDigitalOut(BIT_4);}

//RN1723 GPIO1 input (PIC output)
#define RN_GPIO1_Init()                 {LATAbits.LATA9 = 0; mPORTASetPinsDigitalOut(BIT_9);}
#define RN_COMMAND_MODE()               {LATAbits.LATA9 = 1;}
#define RN_DATA_MODE()                  {LATAbits.LATA9 = 0;}

// RN171 GPIO6 output (PIC input)
// Used to indicate TCP connection status
#define SOCKET_OPEN        1
#define SOCKET_CLOSED      0
#define RN_GPIO6_Init()                 {mPORTESetPinsDigitalIn(BIT_9);}
#define RN_CONNECTION_STATUS()          (PORTEbits.RE9)

#define RN_GPIO7_Init()                 {mPORTASetPinsDigitalIn(BIT_7);}
#define RN_GPIO7                        LATAbits.LATA5
#define TRIS_RN_GPIO7                   TRISAbits.TRISA5

#define RN_GPIO8                LATDbits.LATD7
#define TRIS_RN_GPIO8           TRISDbits.TRISD7

#define Init_RN_GPIO()  {RN_RESET_HIGH(); RN_GPIO9_LOW(); RN_WAKE(); RN_GPIO4_Init(); RN_GPIO5_Init(); RN_GPIO6_Init(); RN_GPIO7_Init(); RN_GPIO1_Init();}

// RN171 RTS output (PIC input (CTS))
#define PIC_RTS                         LATDbits.LATD14
#define TRIS_PIC_RTS                    TRISDbits.TRISD14
#define RN_RTS_Init()                   {PIC_RTS = 1; mPORTDSetPinsDigitalOut(BIT_14);}

// RN171 CTS input (PIC output (RTS))
#define RN_CTS_Init()                   {mPORTDSetPinsDigitalIn(BIT_15);}
#define PIC_CTS                         PORTDbits.RD15
#define TRIS_PIC_CTS                    TRISDbits.TRISD15
#define Init_RN_FlowCtrl()              {PIC_RTS = 1; mPORTDSetPinsDigitalOut(BIT_14); mPORTDSetPinsDigitalIn(BIT_15);}


#define Init_LEDs()                     {mPORTESetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2 | BIT_3);}
#define LED0                            LATEbits.LATE0
#define LED0_TRIS                       TRISEbits.TRISE0
#define LED1                            LATEbits.LATE1
#define LED1_TRIS                       TRISEbits.TRISE1
#define LED2                            LATEbits.LATE2
#define LED2_TRIS                       TRISEbits.TRISE2
#define LED3                            LATEbits.LATE3
#define LED3_TRIS                       TRISEbits.TRISE3

#define LEDS_OFF()                      {LED0_OFF(); LED1_OFF(); LED2_OFF(); LED3_OFF();}
#define LEDS_ON()                       {LED0_ON(); LED1_ON(); LED2_ON(); LED3_ON();}
#define LED0_OFF()                      {LATECLR = BIT_0;}
#define LED0_ON()                       {LATESET = BIT_0;}
#define LED0_INV()                      {LATEINV = BIT_0;}
#define LED1_OFF()                      {LATECLR = BIT_1;}
#define LED1_ON()                       {LATESET = BIT_1;}
#define LED1_INV()                      {LATEINV = BIT_1;}
#define LED2_OFF()                      {LATECLR = BIT_2;}
#define LED2_ON()                       {LATESET = BIT_2;}
#define LED2_INV()                      {LATEINV = BIT_2;}
#define LED3_OFF()                      {LATECLR = BIT_3;}
#define LED3_ON()                       {LATESET = BIT_3;}
#define LED3_INV()                      {LATEINV = BIT_3;}

#define UNDER_VOLTAGE_ALARM_LED         0x02
#define OVER_VOLTAGE_ALARM_LED          0x04

#define	SW0                             (PORTDbits.RD4)         // Ref SW0 CN13
#define	SW1                             (PORTDbits.RD13)        // Ref SW1 CN19
#define Init_Switches()                 {ConfigCNPullups((CN13_PULLUP_ENABLE | CN19_PULLUP_ENABLE)); mPORTDSetPinsDigitalIn(BIT_4 | BIT_13);}
#define Read_SW0()                      SW0
#define Read_SW1()                      SW1


// 25LC256 I/O pins
#define EEPROM_CS_TRIS                  (TRISBbits.TRISB8)
#define EEPROM_CS_IO                    (LATBbits.LATB8)
#define EEPROM_SCK_TRIS                 (TRISFbits.TRISF13)
#define EEPROM_SDI_TRIS                 (TRISFbits.TRISF4)
#define EEPROM_SDO_TRIS                 (TRISFbits.TRISF5)
#define EEPROM_SPI_IF                   (IFS0bits.SPI4RXIF)
#define EEPROM_SSPBUF                   (SPI4BUF)
#define EEPROM_SPICON1                  (SPI4CON)
#define EEPROM_SPICON1bits              (SPI4CONbits)
#define EEPROM_SPIBRG                   (SPI4BRG)
#define EEPROM_SPISTAT                  (SPI4STAT)
#define EEPROM_SPISTATbits              (SPI4STATbits)

// SPI Flash
#define SPIFLASH_CS_TRIS                (TRISFbits.TRISF12)
#define SPIFLASH_CS_IO                  (LATFbits.LATF12)
#define SPIFLASH_SCK_TRIS               (TRISFbits.TRISF13)
#define SPIFLASH_SDI_TRIS               (TRISFbits.TRISF4)
#define SPIFLASH_SDO_TRIS               (TRISFbits.TRISF5)
#define SPIFLASH_SPI_IF                 (IFS0bits.SPI4RXIF)
#define SPIFLASH_SSPBUF                 (SPI4BUF)
#define SPIFLASH_SPICON1                (SPI4CON)
#define SPIFLASH_SPICON1bits            (SPI4CONbits)
#define SPIFLASH_SPIBRG                 (SPI4BRG)
#define SPIFLASH_SPISTAT                (SPI4STAT)
#define SPIFLASH_SPISTATbits            (SPI4STATbits)
#define FLASH_MAX_SPI_FREQ              (10000000ul)	// Hz


// Analog POTENTIOMETER
#define ANALOG_POT_TRIS                 (TRISBbits.TRISB2)  /*  AN2 */
#define ANALOG_POT_IO                   (LATBbits.LATB2)


#define ANALOG_TEMP_TRIS                (TRISBbits.TRISB0)  /* AN0 */
#define ANALOG_TEMP_IO                  (LATBbits.LATB0)


    //Basic Hardware Defines
    #define GET_PERIOD(us)                  ( (DWORD)us*(GetPeripheralClock()/1000000ul))
    #define GET_PERIOD_PRE(us, prescaler)   (GET_PERIOD(us)/(DWORD)prescaler)

    //Interrupt Priority MACROs
    #define	_PRI_LEVEL(lvl)		INT_PRIORITY_LEVEL_ ## lvl
    #define	PRI_LEVEL(lvl)		_PRI_LEVEL(lvl)

    #define	_SUBPRI_LEVEL(slvl)	INT_SUB_PRIORITY_LEVEL_ ## slvl
    #define	SUBPRI_LEVEL(slvl)	_SUBPRI_LEVEL(slvl)

    #define	_IPL(lvl)               ipl ## lvl
    #define	IPL(lvl)                _IPL(lvl)

    #define     _DMA_VECTOR(v)		_DMA_ ## v ## _VECTOR
    #define	DMA_VECTOR(v)		_DMA_VECTOR(v)


    //#define RN_UART_RX          PORTAbits.RA2
    //#define RN_UART_RX_TRIS     TRISAbits.TRISA2

    //#define RN_UART_TX          LATAbits.LATA0
    //#define RN_UART_TX_TRIS     TRISAbits.TRISA0


    #define RN_UART_TX_TRIS         LATFbits.LATF8
    #define RN_UART_RX_TRIS         LATFbits.LATF2

    #define RN_MICROSECONDS_BETWEEN_BYTES   5000


    #define EEPROM_I2C_BUS      I2C2
    #define I2C_CLOCK_FREQ      400000
#endif
