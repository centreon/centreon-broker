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

#include "com/centreon/broker/bbdo/input_buffer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**
 *  Default constructor.
 */
input_buffer::input_buffer() : _first_offset(0), _size(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
input_buffer::input_buffer(input_buffer const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
input_buffer::~input_buffer() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
input_buffer& input_buffer::operator=(input_buffer const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Append data to buffer.
 *
 *  @param[in] d  Data.
 */
void input_buffer::append(std::shared_ptr<io::raw> const& d) {
  if (d && d->size()) {
    if (_data.empty())
      _first_offset = 0;
    _data.push_back(d);
    _size += d->size();
  }
}

/**
 *  Erase bytes from beginning of buffer.
 *
 *  @param[in] bytes  Number of bytes to discard.
 */
void input_buffer::erase(int bytes) {
  // Adjust size.
  _size -= bytes;
  if (_size < 0)
    _size = 0;

  while (bytes && !_data.empty()) {
    // If the number of bytes available in first object is less than
    // the number of bytes to erase, pop first object.
    int available(_data.front()->size() - _first_offset);
    if (bytes >= available) {
      _data.pop_front();
      _first_offset = 0;
      bytes -= available;
    }
    // Otherwise current object still has available data.
    else {
      _first_offset += bytes;
      bytes = 0;
    }
  }
  return ;
}

/**
 *  Extract data from buffer.
 *
 *  @param[out] output  Output buffer.
 *  @param[in]  offset  Start position.
 *  @param[in]  size    Number of bytes to extract.
 */
void input_buffer::extract(std::string& output, int offset, int size) {
  // Find initial position.
  std::list<std::shared_ptr<io::raw> >::const_iterator
    it(_data.begin()), end(_data.end());
  int it_offset(_first_offset);
  for (int remaining(offset); it != end; ++it, it_offset = 0) {
    int available((*it)->size() - it_offset);
    if (remaining < available) {
      it_offset += remaining;
      break ;
    }
    else
      remaining -= available;
  }

  // Extract data.
  int remaining(size);
  for (; remaining && (it != end); ++it, it_offset = 0) {
    int to_extract((*it)->size() - it_offset);
    if (to_extract > remaining)
      to_extract = remaining;
    output.append((*it)->const_data() + it_offset, to_extract);
    remaining -= to_extract;
  }

  // Check that extraction was successful.
  if (remaining)
    throw exceptions::msg()
        << "BBDO: cannot extract " << size << " bytes at offset " << offset
        << " from input buffer, only " << _size
        << " bytes available: this is likely a software bug"
        << " that you should report to Centreon Broker developers";
}

/**
 *  Get buffer size.
 *
 *  @return Buffer size in bytes.
 */
int input_buffer::size() const {
  return _size;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void input_buffer::_internal_copy(input_buffer const& other) {
  _data = other._data;
  _first_offset = other._first_offset;
  _size = other._size;
}
