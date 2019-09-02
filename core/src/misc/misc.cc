/*
** Copyright 2012-2013,2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker;

/**
 *  Get a temporary file path.
 *
 *  @return Temporary file path.
 */
std::string misc::temp_path() {
  char path[] = "/tmp/brokerXXXXXX";
  int retval(mkstemp(path));
  if (retval < 0) {
    char const* err_msg(strerror(errno));
    throw (exceptions::msg() << "cannot create temporary file: "
           << err_msg);
  }
  ::close(retval);
  ::remove(path);
  return (path);
}

static const uint16_t crc_tbl[16] = {
    0x0000, 0x1081, 0x2102, 0x3183,
    0x4204, 0x5285, 0x6306, 0x7387,
    0x8408, 0x9489, 0xa50a, 0xb58b,
    0xc60c, 0xd68d, 0xe70e, 0xf78f
};


uint16_t misc::crc16_ccitt(char const* data, unsigned int data_len) {
  uint16_t crc = 0xffff;
  uint8_t c;
  const uint8_t *p = reinterpret_cast<const uint8_t *>(data);
  while (data_len--) {
    c = *p++;
    crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
    c >>= 4;
    crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
  }
  return ~crc & 0xffff;
}
