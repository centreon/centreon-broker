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
import sys
import time
from datetime import datetime

def get_header(header):
  h = int.from_bytes(header[0:2], byteorder = 'big')
  s = int.from_bytes(header[2:4], byteorder = 'big')
  t = int.from_bytes(header[4:8], byteorder = 'big')
  src = int.from_bytes(header[8:12], byteorder = 'big')
  dst = int.from_bytes(header[12:16], byteorder = 'big')
  print("chksum: ", hex(h))
  print("size: ", s)
  print("type: ", t)
  print("src: ", src)
  print("dst: ", dst)
  assert(0 <= dst <= 3)
  assert(0 <= src <= 3)
  return h, s, t, src, dst

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

port = 5669
sock.bind(('', port))

sock.listen(2)

count = 0

try:
  while 1:
    newSocket, address = sock.accept(  )
    print ("Connected from", address)
    # loop serving the new client
    first = True
    version = newSocket.recv(23)
    newSocket.send(version)
    while 1:
      header = newSocket.recv(16)
      if not header: break
      count += 1
      now = datetime.now()
      print("\n{} new message {}".format(now, count))

      chksum, size, typ, src, dst = get_header(header)
      content = newSocket.recv(size)
      while len(content) < size:
        print("packet not full...")
        time.sleep(0.2)
        l = size - len(content)
        content += newSocket.recv(l)

    newSocket.close()
    print ("Disconnected from", address)
finally:
  sock.close(  )

