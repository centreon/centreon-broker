#!/usr/bin/python3
#
# Copyright 2020-2021 Centreon
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


host_addr = '127.0.0.1'
host_port = 4141
#server_sni_hostname = 'dev-dbr'
server_cert = '/tmp/server.crt'
client_cert = '/tmp/client.crt'
client_key = '/tmp/client.key'

context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH) # , cafile=server_cert)
context.check_hostname = False
context.verify_mode = ssl.CERT_NONE
context.load_cert_chain(certfile=client_cert, keyfile=client_key)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host_addr, host_port))

conn = context.wrap_socket(s, server_side=False)  #, server_hostname=server_sni_hostname)
try:
    conn.send(b"Hello cbd")
    time.sleep(2)
finally:
    conn.shutdown(socket.SHUT_RDWR)
    conn.close()
