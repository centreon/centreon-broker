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

#ifndef CC_MISC_STRINGIFIER_HH
#define CC_MISC_STRINGIFIER_HH

#include <string>
#include <thread>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace misc {
/**
 *  @class stringifier stringifier.hh "com/centreon/misc/stringifier.hh"
 *  @brief Provide method to converting data to string.
 *
 *  Stringifier is a simple way to convert different data type to
 *  C-String (null pointer terminate).
 */
class stringifier {
 public:
  stringifier(char const* buffer = NULL) throw();
  stringifier(stringifier const& right);
  virtual ~stringifier() throw();
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
  stringifier& operator<<(unsigned int u) throw();
  stringifier& operator<<(unsigned long long ull) throw();
  stringifier& operator<<(unsigned long ul) throw();
  stringifier& operator<<(void const* p) throw();
  stringifier& operator<<(std::thread::id const& p) throw();
  stringifier& append(char const* str, unsigned int size) throw();
  char const* data() const throw();
  int precision() const throw();
  void precision(int val) throw();
  void reset() throw();
  unsigned int size() const throw();

 private:
  template <typename T>
  stringifier& _insert(char const* format, T t) throw();
  template <typename T>
  stringifier& _insert(char const* format, unsigned int limit, T t) throw();
  stringifier& _internal_copy(stringifier const& right);
  bool _realloc(unsigned int new_size) throw();

  char* _buffer;
  unsigned int _current;
  int _precision;
  unsigned int _size;
  static unsigned int const _static_buffer_size = 1024;
  char _static_buffer[_static_buffer_size];
};
}

CC_END()

#endif  // !CC_MISC_STRINGIFIER_HH
