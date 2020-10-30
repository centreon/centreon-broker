##
## Copyright 2020 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## For more information : contact@centreon.com
##
import struct
import zlib
import bbdo


class Compression:
    def __init__(self, name):
        self.data = open(name, "rb").read()
        self.offset = 8


    def unserialize(self):
        while self.offset < len(self.data):
            (size,) = struct.unpack_from("!I", self.data, self.offset)
            out = bytearray(zlib.decompress(self.data[self.offset+8:]))
            self.offset += size + 4

            elem = bbdo.Bbdo(out)
            print(elem.unserialize())
