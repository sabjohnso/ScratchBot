#!/usr/bin/env python
"""A module for receiving commands over a very small http`
server.  This module contains one class: "HTTPReceiver", 
which will listen for commands on the specified port."""

import socket
import serial
import re


        
HOST, PORT = 'localhost', 8888


OPEN_RESPONSE = """\
HTTP/1.1 200 OK

"""

CROSS_DOMAIN_RESPONSE = """\
<cross-domain-policy>
    <allow-access-from domain="*" to-ports="8888"/>
</cross-domain-policy>"""

CONTINUE_RESPONSE = """\
<html>
    <body>
        <h1>ScratchBot</h1>
        <a href="http://localhost:8888/halt">HALT</a>
     </body>
</html>"""

DEFAULT_RESPONSE = """\
<html>
    <body>
        <h1>ScratchBot</h1>
        <p>Like, whaaaaat?</p>
        <p><a href="http://localhost:8888/halt">HALT</a></p>
     </body>
</html>"""


HALT_RESPONSE = """\
<html>
    <body>
        <h1>ScratchBot</h1>
        <p>Goodbye!</p>
    </body>
</html>"""


MOVE_COMMAND_PATTERN = ".*/moveBy/([+-]?[0-9]+)/([+-]?[0-9]+).*"

CROSS_DOMAIN_PATTERN = ".*GET\s+/crossdomain.xml.*"

HALT_COMMAND_PATTERN = ".*GET\s+/halt\s.*"


def loop():

    ser = serial.Serial( 3 )

    listen_socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
    listen_socket.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
    listen_socket.bind(( HOST, PORT ))
    listen_socket.listen( 1 )

    print 'Serving HTTP on port %s ... ' % PORT
    keep_running = True
    while keep_running:
        client_connection, client_address = listen_socket.accept()
        request = client_connection.recv( 1024 )

        if re.match( MOVE_COMMAND_PATTERN, request ) is not None:
            command = re.match( MOVE_COMMAND_PATTERN, request )
            assert len( command.groups()) == 2
            print( command.groups()[0] + ":" + command.groups()[1] )
            ser.write( command.groups()[0] + ":" + command.groups()[1] + "\n" )
            response = CONTINUE_RESPONSE
        elif re.match( HALT_COMMAND_PATTERN, request ) is not None:
            response = HALT_RESPONSE
            keep_running = False
        elif re.match( CROSS_DOMAIN_PATTERN, request ) is not None:
            response = CROSS_DOMAIN_RESPONSE
        else:
            response = DEFAULT_RESPONSE


        http_response = OPEN_RESPONSE+response
        


        

        client_connection.sendall( http_response )
        client_connection.close()

        if not keep_running:
            if listen_socket is not None: listen_socket.close()
            if ser is not None: ser.close()
            
    

if __name__ == "__main__":
    loop()
