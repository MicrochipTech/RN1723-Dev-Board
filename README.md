# RN1723 Development Kit Out of Box Setup Guide

## Introduction
The RN1723 Development Board provides a platform to evaluate Microchip’s
RN1723 module for low-power IoT client applications.

### Current version is 0.1.0

#### Notes About this version
This version of the software is being released as a __beta__ version.  Please check back often as there is going to be many updates to this starter kit.

### Required Tools and Applications
You will need the following Microchip development tools to compile and program the RN1723 Development Kit Firmware

- Download and install Microchip MPLAB<sup>&reg;</sup>X IDE for your OS from here:
[Microchip MPLAB<sup>&reg;</sup>X](http://www.microchip.com/mplabx).

- Download and install the latest XC32 compiler for your OS from here:  [Microchip XC32](http://www.microchip.com/xc32)

	>NOTE: MPLAB® XC Compiler licenses are also available as Free editions and have the option of a 60-day PRO evaluation trial.  This will be required as the source code requires optimizations to be turned on.

- [PICKit&trade; 3 In-Circuit Debugger/Programmer](http://www.microchip.com/pickit3).  This programmer uses in-circuit debugging logic incorporated into each chip with Flash memory to provide a low-cost hardware debugger and programmer. You can use this programmer to load firmware into the WCM board.


## SETTING UP THE PYTHON SERVER
To set up the PythonTM server, users will need to install python and the necessary
packages that support SSL before launching the server script. After the server is set up
and is running properly, the RN1723 Development Board will then be able to connect
to the server as a client and post the system updates to the server.
The following sections describe the necessary steps to set up a computer as a server
for Windows ® , Mac OS ® , and Linux.

#### Setting Up the Python Server for Windows 7
Do the following
1. Install python , version 2.7.x.
a) Download and run the installer from:
https://www.python.org/downloads/release/python-2710/
b) Add the Python directory to your system PATH.
Note:
The pip package will be installed with Python 2.7 or later.
2. Install pybottle by opening a command shell (click Start (
in the search box) and entering the following command:
) and enter cmd
python –m pip install bottle
3. Install CherryPy , version 3.2.3 by entering the following command:
python –m pip install cherrypy==3.2.3
4. Launch the server script from the installed directory by entering the following
command:
python ServerMain.py

#### Setting Up a Python Server for Mac OS
The instructions provided in this section assume that a previous version of openssl has
not been installed. If that is not the case, all previous versions must first be uninstalled
before proceeding.
Only openssl version 1.0.0 or later supports the RN1723 Secure Server Script using
TLS1.2. To install openssl do the following:
1. Use Homebrew (http://brew.sh) to install the latest version of openssl using the
following commands in a terminal window, while forcing the links so that Python
will use the right version of openssl .
brew update
brew install openssl
brew link --force openssl
2. Reinstall Python to use the latest openssl:?
brew install python --with-brewed-openssl
3. To verify that python was updated, run the following command and verify that the
version number of the operating system is greater than 1.0.2d?
python -c 'import ssl; print ssl.OPENSSL_VERSION
4. Install pip by opening a terminal application (Applications > Utilities > Terminal)
and entering the following command:.
$ sudo easy_install pip
5. Install pybottle by entering the following command:
$ sudo pip install bottle
6. Install Cherrypy version 3.2.3 by entering the following command:
$ sudo pip install cherrypy==3.2.3
7. Launch the server script by entering the following command:
$ sudo python ServerMain.py

#### Setting Up a Python server for Linux
1. Install Python 2.7.x by opening a terminal application and entering the following
command:
$ sudo apt-get install python
2. Install pip by entering the following command:
$ sudo apt-get install python-pip
3. Install pybottle by entering the following command:
$ sudo pip install bottle
4. Install Cherrypy version 3.2.3 by entering the following command:
$ sudo pip install cherrypy==3.2.3
5. Launch the server script by entering the following command:
$ sudo python ServerMain.py

## Reading the Server IP Address
After the server is set up on a host PC/laptop, please read the IP address of the
laptop/PC using the command ipconfig (Windows) or ifconfig (Mac/Linux).
Figure 3-2 shows a sample screen.
While setting up the RN1723 Client, the host address is required to be set according to
the output of ifconfig / ipconfig (e.g., 192.168.1.225). It is important to ensure that
both the client and server are in the same subnet.

## Opening the Server Web Page
Open a Web browser and enter the following URL, where <local host IP address>
is the actual IP address of your local host:
https://< local host IP address> /RN1723/App1/v1

## SETTING UP RN1723 EMBEDDED CLIENT
#### Provisioning the RN1723 Development Board Using a Serial
Port Terminal Application
1. Connect a mini-USB to Type A USB cable between the RN1723 Development
Board and the laptop/PC.
2. Use a Serial Port Terminal Application software and configure it to the following
settings:
- Baud Rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow Control: None
3. Ensure that the jumper is installed on J4.
4. Press any key to display the Menu in the terminal application
5. Select option 5 and configure the host server address. For example, if the Host
server IP Address is 192.168.1.225, enter 192.168.1.225 when prompted for an
IP Address).
6. Select option 3, enter the SSID, and then the Passphrase to the new AP. Check
for the response “OK” from the RN1723 module, as shown in Figure 3-6.

After provisioning the RN1723 Development Board onto a network, the device will com-
municate periodically to the host server (Cloud) with status information. Once the data
exchange is completed the RN module will return to Sleep mode.
For configuring the application system settings (wake-up interval, alarm thresholds),
the user can modify the settings on the server web page.

```

## License
© 2015 Microchip Technology Inc. and its subsidiaries.  You may use this
software and any derivatives exclusively with Microchip products.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS,
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES
OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR
ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR
USE IN ANY APPLICATION.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER
RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF
THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED
BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS
SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
TERMS.
