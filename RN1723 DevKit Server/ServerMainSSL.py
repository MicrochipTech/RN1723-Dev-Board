#!/usr/bin/env python
#
#
#

import bottle
import logging
from json import dumps, loads
import datetime
from cherrypy import wsgiserver
from cherrypy.wsgiserver import ssl_builtin
import threading, time


#******************************************************************************
# Application Configuration Definitions
# - Change to match your specific application requirements
#******************************************************************************
APP_VERSION = '4.00'

# Server Settings
HOST_IP_ADDRESS = '0.0.0.0'
PORT = 443
BASE_URI = "/RN1723/App1/v1"


#******************************************************************************
# Globals
#******************************************************************************
eclient = {}                                # Global empty dict of clients
numClients = 0                              # Number of Active Clients
today = datetime.date.today().strftime("%w") # Global current date


#******************************************************************************
# Setup logging
#******************************************************************************
logging.basicConfig(format='localhost - - [%(asctime)s] %(message)s', level=logging.DEBUG)
log = logging.getLogger(__name__)
bottle.debug(True)


#******************************************************************************
# Declaration of new class that inherits from ServerAdapter
# Generate the .pem file using openssl.
# Ex: openssl req -new -x509 -keyout server.pem -out server.pem -days 365 -nodes
#******************************************************************************
class MySSLCherryPy(bottle.ServerAdapter):
    def run(self, handler):
        server = wsgiserver.CherryPyWSGIServer((self.host, self.port), handler)

        server.ssl_adapter = ssl_builtin.BuiltinSSLAdapter("certs/localhost.cert",
                                                    "certs/localhost.key")

        try:
            server.start()
        finally:
            server.stop()


#******************************************************************************
# Add new MySSLCherryPy class to the supported servers under the key 'mysslcherrypy'
#******************************************************************************
bottle.server_names['mysslcherrypy'] = MySSLCherryPy

app = bottle.app()


#******************************************************************************
# Common class for all Clients
#******************************************************************************
class Clients:

    def __init__(self, uuid):
        self.totalURICount = 0              # Total Number of times connected to Server
        self.URICount = 0                   # Number of times connected to Server
        self.uuid = uuid                    # Device Unique Identifier(mac address)

        self._links = 'GET_SYSTEM'          # Next link State

        # Status
        self.status = {'potvalue' : 120, 'switches' : 1, 'alarms' : int(0), 'temperature' : 60}
        # Settings
        self.system = {'tempScale' : 'fahrenheit', 'tiltVibe' : bool(True), 'publishPeriod' : int(3), 'temperatureThreshold' : int(90), 'potMaxThreshold' : int(920)}


    def incTotalURICount(self):
        self.totalURICount += 1
        self.URICount += 1


#******************************************************************************
# Check Alarm routine
#******************************************************************************
def checkAlarm(uuid):
    global eclient

    if (int(eclient[uuid].status['potvalue']) >= int(eclient[uuid].system['potMaxThreshold'])):
        eclient[uuid].status['alarms'] = int(2)

    elif (int(eclient[uuid].status['temperature']) >= int(eclient[uuid].system['temperatureThreshold'])):
        eclient[uuid].status['alarms'] = int(1)

    elif (int(eclient[uuid].status['temperature']) >= int(eclient[uuid].system['temperatureThreshold']) and int(eclient[uuid].status['potvalue']) >= int(eclient[uuid].system['potMaxThreshold'])):
        eclient[uuid].status['alarms'] = int(3)

    else:
        eclient[uuid].status['alarms'] = int(0)


#******************************************************************************
# Response Packet Builder Routine
#******************************************************************************
def buildResponsePkt(uuid, respCode = 200, pktData = '', nextLnk=''):
    global eclient
    global headersContentType


    # Set HTTP Header content type
    bottle.response.content_type = 'application/json'

    # Set HTTP Header Response Code
    bottle.response.status = respCode

    # Set Packet _links Information
    if nextLnk is not '':
        msgHead = {"msgHeader":{"uuid":uuid}, "_links":{"self":{"href": '/' + uuid},"next":{"href":'/' + uuid + '/' + nextLnk}}}
    else:
        msgHead = {"msgHeader":{"uuid":uuid}, "_links":{"self":{"href": '/' + uuid}}}
    
    # Add the message data to the packet
    if pktData is not '':
        msgData = pktData
        packet = dict(msgHead, **msgData)
    else:
        packet = msgHead
    
    # Format as JSON
    packet = dumps(packet)

    print "\r\nServer RESPONSE %s:" % respCode
    print packet
    print "\r\n\r\n"

    return packet


#******************************************************************************
# Request Packet Verification Routine
#******************************************************************************
def verifyRequestPkt(uuid, packet):
    global eclient
    global jsonData


    if not packet:
        print "\r\nError:  REQUEST - Data Packet Empty!" 
        return False

    jsonData = loads(packet)  
    print "\r\nREQUEST Packet:\r\n%s\r\n" % jsonData
    
    # Verify msgheader UUID matches URI UUID
    if jsonData['msgHeader']['uuid'] != uuid:
        print "\r\nError: REQUEST - UUID Mismatch.", jsonData['msgHeader']['uuid'], " != ", uuid
        return False

    return True


#******************************************************************************
# Static Routes (used for handling HTTP
# .css, .js, and image file requests
#******************************************************************************
@bottle.get('/static/<filename:re:.*\.js>')
@bottle.route(BASE_URI + '/static/<filename:re:.*\.js>', method='GET')
@bottle.route(BASE_URI + '/<uuid>/static/<filename:re:.*\.js>', method='GET')
def javascripts(uuid='', filename=''):
    return bottle.static_file(filename, root='static/_scripts')


@bottle.get('/static/<filename:re:.*\.css>')
@bottle.route(BASE_URI + '/static/<filename:re:.*\.css>', method='GET')
@bottle.route(BASE_URI + '/<uuid>/static/<filename:re:.*\.css>', method='GET')
def stylesheets(uuid='', filename=''):
    return bottle.static_file(filename, root='static/_styles')


@bottle.get('/static/<filename:re:.*\.php>')
@bottle.route(BASE_URI + '/static/<filename:re:.*\.php>', method='GET')
@bottle.route(BASE_URI + '/<uuid>/static/<filename:re:.*\.php>', method='GET')
def phpscripts(uuid='', filename=''):
    return bottle.static_file(filename, root='static/_php')


@bottle.get('/static/<filename:re:.*\.(jpg|png|gif|ico)>')
@bottle.route(BASE_URI + '/static/<filename:re:.*\.(jpg|png|gif|ico)>', method='GET')
@bottle.route(BASE_URI + '/<uuid>/static/<filename:re:.*\.(jpg|png|gif|ico)>', method='GET')
def images(uuid='', filename=''):
    return bottle.static_file(filename, root='static/_images')


#******************************************************************************
# Server URI's for Browser Clients
#******************************************************************************
@bottle.route('/hello')
@bottle.route(BASE_URI + '/hello', method='GET')
def helloURI():
    return '''<p>Hello from RN1723 Secure Python Server.</p>'''

#------------------------------------------------------------------------------
@bottle.route('/notes', method='GET')
@bottle.view('notes_page')
def notes_page():
    """Release Notes Page"""
    return {}

#------------------------------------------------------------------------------
@bottle.route('/', method='GET')
@bottle.route(BASE_URI + '/dashboard', method='GET')
@bottle.view('dashboard_page')
def dashboard_page():
    global eclient

    return dict(eclient = eclient)

#------------------------------------------------------------------------------
@bottle.route(BASE_URI + '/<uuid>/control', method='GET')
@bottle.view('control_page')
def control_page(uuid=''):
    global eclient

    print "Control Pannel for device: %s" %(uuid)
    return dict(eclient = eclient[uuid])


#------------------------------------------------------------------------------
@bottle.route(BASE_URI + '/<uuid>/updatestatus.json', method='GET')
def updateStatusJSON(uuid=''):
    global eclient

    # Build Packet Data
    getSystem = {"status": eclient[uuid].status, "system": eclient[uuid].system}

    return getSystem

#------------------------------------------------------------------------------
@bottle.route(BASE_URI + '/<uuid>/resetURICounter', method='POST')
def resetURICounter(uuid=''):
    global eclient

    print "Reset URI Counter: %s\r\n" % (bottle.request.forms.get('Reset'))
    if(bottle.request.forms.get('Reset') == 'Reset'):
        eclient[uuid].URICount = 0
    
     # Return HTML page to Client Browser
    bottle.redirect(BASE_URI + '/dashboard')

#------------------------------------------------------------------------------
@bottle.route(BASE_URI + '/<uuid>/change_settings', method='POST')
def change_Settings(uuid=''):
    global eclient

    # Check if tiltVibe in POST packet
    if 'tiltVibe' not in bottle.request.POST:
        eclient[uuid].system['tiltVibe'] = False
    else:
        eclient[uuid].system['tiltVibe'] = True


    # Update the Client's settings with submitted form data
    eclient[uuid].system['tempScale'] = bottle.request.forms.get('tempScale')
    maxTemp = int(bottle.request.forms.get('max_temp'))

    if(eclient[uuid].system['tempScale'] == 'celsius'):
        print "\r\nTemp is in Celsius:\r\n"
        maxTemp = 9.0/5.0 * maxTemp + 32
        print "Converted temp = ", round(maxTemp, 0)
        
    eclient[uuid].system['temperatureThreshold'] = maxTemp
    eclient[uuid].system['publishPeriod'] = bottle.request.forms.get('postInterval')
    eclient[uuid].system['potMaxThreshold'] = bottle.request.forms.get('max_pot')

    # Direct Client to GET New System Settings on next connection
    eclient[uuid]._links = 'GET_SYSTEM'

    # Print the Settings
    print "================\r\nTemp Scale = %s\r\nTilt/Vibe = %s\r\nPOST Interval = %s\r\nMax Temp = %s\r\nMax POT = %s\r\n================\r\n" % (eclient[uuid].system['tempScale'],eclient[uuid].system['tiltVibe'], eclient[uuid].system['publishPeriod'], eclient[uuid].system['temperatureThreshold'], eclient[uuid].system['potMaxThreshold'])
    
    # Return HTML page to Client Browser
    bottle.redirect(BASE_URI + '/%s/control'%(uuid))


#******************************************************************************
# Server URI's for Embedded Clients
#******************************************************************************
@bottle.route(BASE_URI + '/<uuid>/status', method='PUT')
def putStatusURI(uuid=''):
    global eclient
    global jsonData
    global numClients

      
    print "\r\n\r\n"
    print "---------------------------------------------------"
    print "  %s " % str(datetime.datetime.now())
    print " Begin Embedded Client Session: %s" % uuid
    print "---------------------------------------------------"
    print "\r\n"
    
    print bottle.request

    # Check if valid client
    if uuid not in eclient:
        # Add new client
        eclient[uuid] = Clients(uuid)
        numClients += 1
        print "Alert:  New Client %s added!" % uuid

    # Verify Request Packet
    if not verifyRequestPkt(uuid, bottle.request.body.read()):
        bottle.abort(400, 'The request cannot be fulfilled due to bad syntax\r\n')
    else:
        # Read the Packet data
        eclient[uuid].status['potvalue'] = jsonData['status']['potvalue']
        eclient[uuid].status['temperature'] = jsonData['status']['temperature']
        eclient[uuid].status['switches'] = jsonData['status']['switches']

        # Print the Packet Settings
        print "================\r\nPot = %s\r\nSwitches = %s\r\nTemp = %s\r\n================\r\nAlarms = %s\r\n" % (eclient[uuid].status['potvalue'], eclient[uuid].status['switches'], eclient[uuid].status['temperature'], eclient[uuid].status['alarms'])

	# Check Alarm Condition
	checkAlarm(uuid)

    # Increment the device URI counter  
    eclient[uuid].incTotalURICount()

    # Determine Response to send & Build Response Packet
    if eclient[uuid]._links == 'SELF_SLEEP':
        respPacket = buildResponsePkt(uuid)

    if eclient[uuid]._links == 'PUT_SYSTEM':
        respPacket = buildResponsePkt(uuid, nextLnk='system')

    if eclient[uuid]._links == 'GET_SYSTEM':
        respPacket = buildResponsePkt(uuid, respCode=303, nextLnk='system')

    # Send response packet
    return respPacket


#------------------------------------------------------------------------------
@bottle.route(BASE_URI + '/<uuid>/system', method='PUT')
def putSystemURI(uuid=''):
    global eclient
    global jsonData


    # Decrypt and Verify Request Packet
    if not verifyRequestPkt(uuid, bottle.request.body.read()):
        bottle.abort(400, 'The request cannot be fulfilled due to bad syntax')
    else:
        # Read the Packet data
        eclient[uuid].system = jsonData['system']
        # Print Packet Settings
        print "================\r\nTemperature Scale: %s\r\nTilt/Vibe = %s\r\nPOST Interval = %s\r\nMax Temp = %s\r\nMax POT = %s\r\n================\r\n" % (eclient[uuid].system['tempScale'], eclient[uuid].system['tiltVibe'], eclient[uuid].system['publishPeriod'], eclient[uuid].system['temperatureThreshold'], eclient[uuid].system['potMaxThreshold'])

    # Set Next Link
    eclient[uuid]._links = 'SELF_SLEEP'

    # Build response packet
    respPacket = buildResponsePkt(uuid)

    # Send response packet
    return respPacket


#------------------------------------------------------------------------------
@bottle.route(BASE_URI + '/<uuid>/system', method='GET')
def getSystemURI(uuid=''):
    global eclient

    # Check if valid client
    if uuid not in eclient:
        bottle.abort(400, 'Client not registered.\r\n')

    # Build Packet Data
    getSystem = {"system":{"publishPeriod":int(eclient[uuid].system['publishPeriod']),"tempScale":(eclient[uuid].system['tempScale']), "tiltVibe":bool(eclient[uuid].system['tiltVibe']), "temperatureThreshold":int(eclient[uuid].system['temperatureThreshold']), "potMaxThreshold":int(eclient[uuid].system['potMaxThreshold'])}}

    # Set Next Link
    eclient[uuid]._links = 'SELF_SLEEP'

    # Build response packet
    respPacket = buildResponsePkt(uuid, pktData = getSystem)

    # Send response packet
    return respPacket


#------------------------------------------------------------------------------
#******************************************************************************
# Application Main Routine
#******************************************************************************
def main():
    print "======================================================="
    print " RN1723 Secure Test Server Application Version: %s   \r\n" % APP_VERSION
    print "======================================================="

    bottle.debug(True)
    bottle.run(app=app, host=HOST_IP_ADDRESS, port=PORT, server='mysslcherrypy', quiet=False, reloader=True)


if __name__ == "__main__":
    main()
