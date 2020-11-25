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


host_addr = '127.0.0.1'
host_port = 5758
server_sni_hostname = 'localhost'
server_cert = 'server.crt'
client_cert = 'client.crt'
client_key = 'client.key'

context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=server_cert)
context.load_cert_chain(certfile=client_cert, keyfile=client_key)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host_addr, host_port))

w = welcome()
s.send(w)
header = s.recv(16)
chksum, size, typ, src, dst = get_header(header)
content = s.recv(size)
while len(content) < size:
    print("packet not full...")
    time.sleep(0.2)
    l = size - len(content)
    content += s.recv(l)
print("bbdo message of type {} and length {} received".format(typ,size))

print("TLS establishment")

conn = context.wrap_socket(s, server_side=False, server_hostname=server_sni_hostname)
print("SSL established. Peer: {}".format(conn.getpeercert()))
try:
    while True:
        header = conn.recv(16)
        chksum, size, typ, src, dst = get_header(header)
        content = conn.recv(size)
        while len(content) < size:
            print("packet not full...")
            time.sleep(0.2)
            l = size - len(content)
            content += conn.recv(l)
        print("bbdo message of type {} and length {} received".format(typ,size))
finally:
    print("Closing connection")
    conn.shutdown(socket.SHUT_RDWR)
    conn.close()
