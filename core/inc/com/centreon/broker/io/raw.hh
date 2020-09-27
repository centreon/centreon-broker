/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_IO_RAW_HH
#define CCB_IO_RAW_HH

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class raw raw.hh "io/raw.hh"
 *  @brief Raw byte array.
 *
 *  Raw byte array.
 */
class raw : public data {
 public:
  raw();
  raw(raw const& r);
  raw(std::vector<char>&& b);
  ~raw();
  raw& operator=(raw const& r);
  constexpr static uint32_t static_type() {
    return events::data_type<events::internal, events::de_raw>::value;
  }
  void resize(size_t s);
  char* data();
  char const* const_data() const;
  size_t size() const;
  std::vector<char>& get_buffer();
  bool empty() const;

  template <typename T>
  void append(T const& msg) {
    _buffer.insert(_buffer.end(), msg.begin(), msg.end());
  }

 public:
  std::vector<char> _buffer;
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_RAW_HH
