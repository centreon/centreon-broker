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

#ifndef CCB_COMPRESSION_STACK_ARRAY_HH
#define CCB_COMPRESSION_STACK_ARRAY_HH

#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace compression {
/**
 *  @class stack_array stack_array.hh
 * "com/centreon/broker/compression/stack_array.hh"
 *  @brief Array that behave like a stack.
 *
 *  Array that behave like a stack (push, pop). Low internal buffer
 *  reallocation that is especially useful when decompressing data.
 */
class stack_array {
 public:
  stack_array();
  stack_array(stack_array const& other);
  ~stack_array();
  stack_array& operator=(stack_array const& other);
  char const* data() const;
  void pop(int bytes);
  void push(std::vector<char> const& buffer);
  int size() const;

 private:
  std::string _buffer;
  int _offset;
};
}  // namespace compression

CCB_END()

#endif  // !CCB_COMPRESSION_STACK_ARRAY_HH
