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

import socket, sys, time
from datetime import datetime

host_addr = '10.0.2.15'
host_port = 5669

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host_addr, host_port))

# Just not to close the socket immediatly
time.sleep(30)
