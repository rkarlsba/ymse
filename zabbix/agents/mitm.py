#!/usr/bin/python3

import socket
import sys

listen_port = 10060
listen_addr = '0.0.0.0'
connect_port = 10050
bufsize = 1500

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
server_address = (listen_addr, listen_port)
print('starting up on %s port %s' % server_address, file=sys.stderr)
sock.bind(server_address)

try:
    print >>sys.stderr, 'connection from', client_address

    # Receive the data in small chunks and retransmit it
    while True:
        data = connection.recv(bufsize)
        print('received "%s"' % data, file=sys.stderr)
        if data:
            print('sending data back to the client', file=sys.stderr) 
            connection.sendall(data)
        else:
            print('no more data from', client_address, file=sys.stderr) 
            break

finally:
    # Clean up the connection
    connection.close()
