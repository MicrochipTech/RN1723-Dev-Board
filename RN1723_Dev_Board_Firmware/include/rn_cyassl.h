/*******************************************************************************
  The wifly module-level command/response API functions prototypes

  Company:
    Microchip Technology Inc.

  File Name:
    wifly_util.h

  Summary:
     API prototyes for the wifly module

  Description:
      The prototypes in this header file  are used to send commands and receive
      responses from the Wifly module.
******************************************************************************/

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

#ifndef WIFLY_UTIL_H
#define	WIFLY_UTIL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus  // Provide C++ Compatability

    extern "C" {

#endif

// Required WiFly module pre-configuration (v2.45 firmware)
//
// Step 1. Perform Factory Reset (manual)
// factory R
// save
// reboot
//
// Step 2. Execute the following (send "wifly_settings.txt" from tera term)
// $$$
// set comm close 0         // suppress all output when connection opened/closed
// set comm open 0
// set comm remote 0
// set sys print 0          // suppress all print level output messages in data mode
// set sys iofunc 0x70      // enable ALT GPIO functions
// set wlan join 1          // auto-associate with saved SSID on reboot or w/u from sleep
// set uart flow 1          // enable UART flow control
// set uart mode 1          // suppress ECHO of rx'd char's in command mode
// save                     // save these settings
// reboot                   // reboot the module
//
// (remaining parameters are left at defaults)

        // *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/*  These function prototypes are for the number of bytes in the fifo, the fifo
 *  write, read, and initialization functions respectively.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
extern char  OutString[128];               // console/lcd display output buffer
extern char  WiflyInBuffer[1024];           // input buffer used to receive strings from WiFly
extern char  WiflyOutBuffer[128];          // output buffer used to send strings to WiFly

extern UINT8      accessPoints[20][25];

// Structure to contain a MAC address
typedef struct
{
    char raw[6][3];
    int v[6];
    
} MAC_ADDR;

// Structure to contain an IPv4 address
typedef struct
{
    int v[4];
    // for IP Addr=192.168.1.113 : v[0]=192, v[1]=168, v[2]=1, v[3]=113
} IP_ADDR;

// Address structure for a node
typedef struct
{
    IP_ADDR       IPAddr;
    MAC_ADDR      MACAddr;
} NODE_INFO;


// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************
/******************************************************************************/
// WiFly command hierarchy
//      "<Type> <Category> <Option> <Value>"
// ex.  "set wlan ssid Cisco40671"

// Command Type Keywords (only those used are listed)
// (refer to the WiFly UM)
#define SET             "set"
#define GET             "get"
#define STATUS_SHOW     "show"
#define ACTION_$$$      "$$$"       // enter command mode
#define ACTION_EXIT     "exit"      // exit command mode
#define ACTION_JOIN     "join"      // join a WiFi network
#define ACTION_LEAVE    "leave"     // leave a WiFi network
#define ACTION_RUN      "run"       // run an application
#define FILEIO_SAVE     "save"      // save configuration settings to the default config file

// Command Category Keywords
// (refer to the WiFly UM)
#define IP              "ip"        // get/set the remote host IP/port settings
#define MAC             "mac"       // get the module's MAC address
#define WLAN            "wlan"      // get/set the APs SSID, passphrase, channel
#define DNS             "dns"       // get/sets the remote host domain name
#define COMM            "comm"
#define UART            "uart"
#define SYS             "sys"

// "ip" option keywords
// (refer to the WiFly UM)
#define HOST            "host"      // specify the remote host's IP Address
                                    // use "set dns name <URL>" if IP addr unknown
#define REMOTE          "remote"    // specifies the remote port#
#define ADDRESS         "address"   // get IP address of station
#define DHCP            "dhcp"
#define ADDRESS         "address"
#define NETMASK         "netmask"
#define GATEWAY         "gateway"

// "wlan" option keywords
// (refer the the WiFly UM)
#define SSID            "ssid"      // specifies the SSID
#define PHRASE          "phrase"    // specify the WPA/WPA2 passphrase
#define CHANNEL         "channel"   // specifies the WLAN channel
#define JOIN            "join"      // set WLAN join mode

// "dns" option keywords
// (refer to the WiFly UM)
#define NAME            "name"

// "comm" option keywords
// (refer the the WiFly UM)
#define CLOSE           "close"
#define OPEN            "open"
#define REMOTE          "remote"

// "uart" option keywords
#define FLOW            "flow"
#define MODE            "mode"

// "sys" option keywords
#define PRINT           "print"
#define IOFUNC          "iofunc"


// Command "Response" Strings
#define STD_RESPONSE                "4.04"      // all replies must contain WiFly fw version
#define GENERIC_RESPONSE            "AOK"
#define FILE_SAVE_RESPONSE          "Storing in config"
#define ACTION_EXIT_RESPONSE        "EXIT"

/******************************************************************************/
// Editable TCP/WiFi Module Parameters (defaults listed)

// "comm" option values
#define CLOSE_VALUE         "0"
#define OPEN_VALUE          "0"
#define COMM_REMOTE_VALUE   "0"

// "sys" option values
#define PRINT_VALUE         "0"
#define IOFUNC_VALUE        "0x70"

// "wlan" option values
#define JOIN_VALUE          "1"                 // duplicate setting as "JOIN_MODE_TRY_ASSOC" below

// "ip" option values
#define IP_REMOTE_VALUE        "80"                // port number of mtt.mchpcloud.com

// "dns" option values
#define NAME_VALUE          "www.rovingnetworks.com" // DNS name of host

// "uart" option values
#define FLOW_VALUE          "1"                 // flow control enabled
#define MODE_VALUE          "1"                 // RX echo disabled


// Generic module option values
// Used by EZConfig code...
#define JOIN_MODE_MANUAL        "0"
#define JOIN_MODE_TRY_ASSOC     "1"
#define JOIN_MODE_ASSOC_TO_ANY  "2"
#define JOIN_MODE_ADHOC         "4"
#define JOIN_MODE_SOFT_AP       "7"

#define DHCP_MODE_OFF           "0"
#define DHCP_MODE_ON            "1"
#define DHCP_MODE_AUTO_IP       "2"
#define DHCP_MODE_CACHE         "3"
#define DHCP_MODE_SOFT_AP       "4"

#define GPIO_ALT_0X70           "0x70"
#define GPIO_ALT_0X50           "0x50"

#define UART_ENABLE_FLOW_CONTROL    "1"

// EZConfig SoftAP mode-specific option values
#define SAP_NETWORK_SSID            "thermostat"
#define SAP_NETWORK_IP_ADDRESS      "wifly_config.html"
#define SAP_NETWORK_PASS            "pass"
#define SAP_NETWORK_AP_MODE         "7"
#define SAP_NETWORK_CHANNEL         "1"
#define SAP_NETWORK_DHCP_MODE       "4"
#define SAP_NETWORK_SUBNET_MASK     "255.255.255.0"
#define SAP_NETWORK_GW_ADDRESS      SAP_NETWORK_IP_ADDRESS

#define TCPPORT_OPEN_STRING     "^o^"
#define TCPPORT_CLOSE_STRING    "^c^"
#define TCPPORT_HELLO_STRING    "0"

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines Group
// *****************************************************************************
// *****************************************************************************
/******************************************************************************/

// Module-level APIs
void Wifly_Default_Config(void);
void Wifly_Factory_Reset(void);
void Wifly_Reboot(void);


BOOL wifly_enter_command_mode(void);
void wifly_exit_command_mode(void);
BOOL wifly_send_command(
        const char *command_type,
        const char *command_category,
        const char *command_option,
        const char *command_value,
        const char *command_response);
void wifly_run_webApp();
void wifly_exit_webApp(void);
void wifly_active_scan();

void flushWiflyRxUart(INT8);

#endif	/* WIFLY_UTIL_H */

