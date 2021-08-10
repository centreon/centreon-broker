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

import socket
from socket import AF_INET, SOCK_STREAM, SO_REUSEADDR, SOL_SOCKET, SHUT_RDWR
from datetime import datetime
import ssl, sys, time

listen_addr = '127.0.0.1'
listen_port = 4141
server_cert = '/tmp/server.crt'
server_key = '/tmp/server.key'
client_cert = '/tmp/client.crt'

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
# Just the server cert/key
context.load_cert_chain(certfile=server_cert, keyfile=server_key)

# With CERT_NONE => we don't need the client certificate
# With CERT_OPTIONAL => we verify peer, so we need a certificate authority
# With CERT_REQUIRED => We verify peer with its certificate, so we also need a certificate authority
context.verify_mode = ssl.CERT_OPTIONAL
# certificate authority needed except when CERT_NONE
context.load_verify_locations(cafile=client_cert)

bindsocket = socket.socket()
bindsocket.bind((listen_addr, listen_port))
bindsocket.listen(5)

newsocket, fromaddr = bindsocket.accept()
conn = context.wrap_socket(newsocket, server_side=True)
print("SSL established. Peer: {}".format(conn.getpeercert()))
try:
    content = conn.recv(16)
    print(content)
    time.sleep(1)
finally:
    print("Closing connection")
    conn.shutdown(socket.SHUT_RDWR)
    conn.close()
