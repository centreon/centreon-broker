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

#include <random>
#include <cstring>
#include <com/centreon/broker/misc/uuid.hh>

using namespace com::centreon::broker;

misc::Uuid::Uuid() {
  std::random_device rd;

  std::uniform_int_distribution<uint32_t> dist32(0, UINT32_MAX);
  std::uniform_int_distribution<uint16_t> dist16(0, UINT16_MAX);
  std::uniform_int_distribution<uint8_t> dist8(0, UINT8_MAX);


  _time_low = dist32(rd);
  _time_mid = dist16(rd);
  _time_hi_and_version = dist16(rd);
  _clock_seq_hi_and_reserved = dist8(rd);
  _clock_seq_low = dist8(rd);
  for (int i = 0; i < 6; ++i)
    _node[i] = dist8(rd);

  _clock_seq_hi_and_reserved &= ~(1 << 6);
  _clock_seq_hi_and_reserved |= (1 << 7);
  _time_hi_and_version &= ~(1 << 12);
  _time_hi_and_version &= ~(1 << 13);
  _time_hi_and_version |= (1 << 14);
}

misc::Uuid::Uuid(Uuid const& uuid) {
  operator=(uuid);
}

misc::Uuid const& misc::Uuid::operator=(Uuid const& uuid) {
  if (this != &uuid) {
    _time_low = uuid._time_low;
    _time_mid = uuid._time_mid;
    _time_hi_and_version = uuid._time_hi_and_version;
    _clock_seq_hi_and_reserved = uuid._clock_seq_hi_and_reserved;
    _clock_seq_low = uuid._clock_seq_low;

    memcpy(&_node, uuid._node, sizeof(_node));
  }
  return *this;
}

std::string misc::Uuid::to_string() const {
  std::string uuid("", 37);
  int c;

  c = snprintf(&uuid[0], 37, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
               _time_low, _time_mid, _time_hi_and_version,
               _clock_seq_hi_and_reserved, _clock_seq_low, _node[0],
               _node[1], _node[2], _node[3], _node[4], _node[5]);

  if (c < 0)
    return std::string();

  return uuid;
}