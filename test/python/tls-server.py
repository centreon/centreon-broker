#!/usr/bin/python3
#
# Copyright 2020 Centreon
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# For more information : contact@centreon.com
#
# This script is a little tcp server working on port 5669. It can simulate
# a cbd instance. It is useful to test the validity of BBDO packets sent by
# centengine.

import socket, sys, ssl, time
from datetime import datetime


def welcome():
    s = "\x65\xfe\x00\x0e\x00\x02\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x02\x00\x00\x00\x00\x54\x4c\x53\x00\x00\x00\x00\x00"
    retval = bytearray()
    retval.extend(map(ord, s))
    return retval


def get_header(header):
    h = int.from_bytes(header[0:2], byteorder='big')
    s = int.from_bytes(header[2:4], byteorder='big')
    t = int.from_bytes(header[4:8], byteorder='big')
    src = int.from_bytes(header[8:12], byteorder='big')
    dst = int.from_bytes(header[12:16], byteorder='big')
    print("chksum: ", hex(h))
    print("size: ", s)
    print("type: ", t)
    print("src: ", src)
    print("dst: ", dst)
    assert (0 <= dst <= 3)
    assert (0 <= src <= 3)
    return h, s, t, src, dst


import socket
from socket import AF_INET, SOCK_STREAM, SO_REUSEADDR, SOL_SOCKET, SHUT_RDWR
import ssl

listen_addr = '127.0.0.1'
listen_port = 5671
server_cert = 'server.crt'
server_key = 'server.key'
client_certs = 'client.crt'

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.verify_mode = ssl.CERT_REQUIRED
context.load_cert_chain(certfile=server_cert, keyfile=server_key)
context.load_verify_locations(cafile=client_certs)

bindsocket = socket.socket()
bindsocket.bind((listen_addr, listen_port))
bindsocket.listen(5)

while True:
    print("Waiting for client")
    newsocket, fromaddr = bindsocket.accept()
    print("Client connected: {}:{}".format(fromaddr[0], fromaddr[1]))
    conn = context.wrap_socket(newsocket, server_side=True)
    print("SSL established. Peer: {}".format(conn.getpeercert()))
    buf = b''  # Buffer to hold received client data
    try:
        while True:
            data = conn.recv(4096)
            if data:
                # Client sent us data. Append to buffer
                buf += data
            else:
                # No more data from client. Show buffer and close connection.
                print("Received:", buf)
                break
    finally:
        print("Closing connection")
        conn.shutdown(socket.SHUT_RDWR)
        conn.close()
