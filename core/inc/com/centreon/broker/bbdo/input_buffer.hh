/*
** Copyright 2017 Centreon
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

#ifndef CCB_BBDO_INPUT_BUFFER_HH
#define CCB_BBDO_INPUT_BUFFER_HH

#include <list>
#include <memory>
#include <string>
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bbdo {
/**
 *  @class input_buffer input_buffer.hh
 * "com/centreon/broker/bbdo/input_buffer.hh"
 *  @brief Bufferize BBDO input data.
 *
 *  This class bufferizes BBDO input data in a performance-optimal
 *  container. It also provides an optimized data-fetching interface
 *  for the BBDO decoder.
 */
class input_buffer {
 public:
  input_buffer();
  input_buffer(input_buffer const& other);
  ~input_buffer();
  input_buffer& operator=(input_buffer const& other);
  void append(std::shared_ptr<io::raw> const& d);
  void erase(int bytes);
  void extract(std::string& output, int offset, int size);
  int size() const;

 private:
  void _internal_copy(input_buffer const& other);

  std::list<std::shared_ptr<io::raw> > _data;
  int _first_offset;
  int _size;
};
}  // namespace bbdo

CCB_END()

#endif  // !CCB_BBDO_INPUT_BUFFER_HH
