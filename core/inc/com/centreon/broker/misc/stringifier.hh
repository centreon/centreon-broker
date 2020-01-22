/*
** Copyright 2009-2011,2015 Centreon
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

#ifndef CCB_MISC_STRINGIFIER_HH_
#define CCB_MISC_STRINGIFIER_HH_

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
/**
 *  @class stringifier stringifier.hh "com/centreon/misc/stringifier.hh"
 *  @brief Provide method to converting data to string.
 *
 *  Stringifier is a simple way to convert different data type to
 *  C-String (null pointer terminated).
 */
class stringifier {
 public:
  stringifier(char const* buffer = nullptr) noexcept;
  stringifier(stringifier const& right);
  virtual ~stringifier();
  stringifier& operator=(stringifier const& right);
  stringifier& operator<<(bool b) noexcept;
  stringifier& operator<<(char const* str) noexcept;
  stringifier& operator<<(char c) noexcept;
  stringifier& operator<<(double d) noexcept;
  stringifier& operator<<(int i) noexcept;
  stringifier& operator<<(long long ll) noexcept;
  stringifier& operator<<(long l) noexcept;
  stringifier& operator<<(std::string const& str) noexcept;
  stringifier& operator<<(stringifier const& str) noexcept;
  stringifier& operator<<(uint32_t u) noexcept;
  stringifier& operator<<(unsigned long long ull) noexcept;
  stringifier& operator<<(unsigned long ul) noexcept;
  stringifier& operator<<(void const* p) noexcept;
  stringifier& append(char const* str, uint32_t size) noexcept;
  char const* data() const noexcept;
  int precision() const noexcept;
  void precision(int val) noexcept;
  void reset() noexcept;
  uint32_t size() const noexcept;

 protected:
  template <typename T>
  stringifier& _insert(char const* format, T t) noexcept;
  template <typename T>
  stringifier& _insert(char const* format, uint32_t limit, T t) noexcept;
  stringifier& _internal_copy(stringifier const& right);
  bool _realloc(uint32_t new_size);

  char* _buffer;
  uint32_t _current;
  int _precision;
  uint32_t _size;
  static uint32_t const _static_buffer_size = 1024;
  char _static_buffer[_static_buffer_size];
};
}  // namespace misc

CCB_END()

#endif /* !CCB_MISC_STRINGIFIER_HH_ */
