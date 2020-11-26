#!/bin/bash
#To create the server certificate:

openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout server.key -out server.crt

#To create the client certificate:

openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout client.key -out client.crt
