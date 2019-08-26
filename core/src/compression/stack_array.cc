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

#include "com/centreon/broker/compression/stack_array.hh"

using namespace com::centreon::broker::compression;

/**
 *  Default constructor.
 */
stack_array::stack_array() : _offset(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
stack_array::stack_array(stack_array const& other)
  : _buffer(other._buffer), _offset(other._offset) {}

/**
 *  Destructor.
 */
stack_array::~stack_array() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
stack_array& stack_array::operator=(stack_array const& other) {
  if (this != &other) {
    _buffer = other._buffer;
    _offset = other._offset;
  }
  return *this;
}

/**
 *  Get internal buffer.
 *
 *  @return Internal buffer.
 */
char const* stack_array::data() const {
  return _buffer.data() + _offset;
}

/**
 *  Pop data from the container.
 *
 *  @param[in] bytes  Number of bytes to pop.
 */
void stack_array::pop(int bytes) {
  _offset += bytes;
}

/**
 *  Push data in container.
 *
 *  @param[in] buffer  Data.
 */
void stack_array::push(std::vector<char> const& buffer) {
  // Remove processed data from underlying container.
  if (_offset) {
    _buffer.erase(0, _offset);
    _offset = 0;
  }

  // Append data.
  std::copy(buffer.begin(), buffer.end(), back_inserter(_buffer));
}

/**
 *  Return the container's size.
 *
 *  @return The container's size.
 */
int stack_array::size() const {
  return _buffer.size() - _offset;
}
