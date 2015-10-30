# RN1723 Development Kit Out of Box Setup Guide

## Introduction
The RN1723 Development Board provides a platform to evaluate Microchip’s
RN1723 module for low-power IoT client applications.

### Current version is 4.0

#### Notes About this version
This is the initial public release version.  Please check back often for updates.

## SETTING UP THE PYTHON SERVER
To set up the Python server, users will need to install python and the necessary
packages that support SSL before launching the server script. After the server is set up
and running properly, the RN1723 Development Board will then be able to connect
to the server as a client and post the status updates to the server.
The following sections describe the necessary steps to set up a computer as a server
for Windows ® , Mac OS ® , and Linux.

#### Setting Up the Python Server for Windows 7
Do the following:

Install python , version 2.7.x.
```
a) Download and run the installer from:
https://www.python.org/downloads/release/python-2710/
b) Add the Python directory to your system PATH.
```
> Note: The pip package will be installed with Python 2.7 or later.

Install pybottle by opening a command shell (click Start (
in the search box) and entering the following command:
) and enter cmd
```
python –m pip install bottle
```
Install CherryPy , version 3.2.3 by entering the following command:
```
python –m pip install cherrypy==3.2.3
```
Launch the server script from the installed directory by entering the following
command:
```
python ServerMain.py
```

#### Setting Up a Python Server for Mac OS
The instructions provided in this section assume that a previous version of openssl has
not been installed. If that is not the case, all previous versions must first be uninstalled
before proceeding.
Only openssl version 1.0.0 or later supports the RN1723 Secure Server Script using
TLS1.2. To install openssl do the following:

Use Homebrew (http://brew.sh) to install the latest version of openssl using the
following commands in a terminal window, while forcing the links so that Python
will use the right version of openssl.
```
brew update
brew install openssl
brew link --force openssl
```

Reinstall Python to use the latest openssl:?
```
brew install python --with-brewed-openssl
```

To verify that python was updated, run the following command and verify that the
version number of the operating system is greater than 1.0.2d?
```
python -c import ssl; print ssl.OPENSSL_VERSION
```

Install pip by opening a terminal application (Applications > Utilities > Terminal)
and entering the following command:
```
sudo easy_install pip
```

Install pybottle by entering the following command:
```
sudo pip install bottle
```

Install Cherrypy version 3.2.3 by entering the following command:
```
sudo pip install cherrypy==3.2.3
```

Launch the server script by entering the following command:
```
sudo python ServerMain.py
```

#### Setting Up a Python server for Linux
Install Python 2.7.x by opening a terminal application and entering the following
command:
```
sudo apt-get install python
```

Install pip by entering the following command:
```
sudo apt-get install python-pip
```

Install pybottle by entering the following command:
```
sudo pip install bottle
```

Install Cherrypy version 3.2.3 by entering the following command:
```
sudo pip install cherrypy==3.2.3
```

Launch the server script by entering the following command:
```
sudo python ServerMain.py
```

## Opening the Server Web Page
Open a Web browser and enter the following URL on the same machine running the python server script:
```
https://localhost
```

## SETTING UP RN1723 EMBEDDED CLIENT
#### Provisioning the RN1723 Development Board Using a Serial Port Terminal Application
```
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
```

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
