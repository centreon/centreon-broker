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
  stringifier(char const* buffer = NULL) throw();
  stringifier(stringifier const& right);
  virtual ~stringifier();
  stringifier& operator=(stringifier const& right);
  stringifier& operator<<(bool b) throw();
  stringifier& operator<<(char const* str) throw();
  stringifier& operator<<(char c) throw();
  stringifier& operator<<(double d) throw();
  stringifier& operator<<(int i) throw();
  stringifier& operator<<(long long ll) throw();
  stringifier& operator<<(long l) throw();
  stringifier& operator<<(std::string const& str) throw();
  stringifier& operator<<(stringifier const& str) throw();
  stringifier& operator<<(uint32_t u) throw();
  stringifier& operator<<(unsigned long long ull) throw();
  stringifier& operator<<(unsigned long ul) throw();
  stringifier& operator<<(void const* p) throw();
  stringifier& append(char const* str, uint32_t size) throw();
  char const* data() const throw();
  int precision() const throw();
  void precision(int val) throw();
  void reset() throw();
  uint32_t size() const throw();

 protected:
  template <typename T>
  stringifier& _insert(char const* format, T t) throw();
  template <typename T>
  stringifier& _insert(char const* format, uint32_t limit, T t) throw();
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
