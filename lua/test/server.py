import socket

# Create a socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Ensure that you can restart your server quickly when it terminates
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Set the client socket's TCP "well-known port" number
well_known_port = 9200
sock.bind(('', well_known_port))

# Set the number of clients waiting for connection that can be queued
sock.listen(5)

# loop waiting for connections (terminate with Ctrl-C)
try:
    while 1:
        newSocket, address = sock.accept()
        print "Connected from", address
        # loop serving the new client
        while 1:
            receivedData = newSocket.recv(1024)
            if not receivedData: break
            # Echo back the same data you just received
            newSocket.send(receivedData)
        newSocket.close(  )
        print "Disconnected from", address
finally:
    sock.close()
