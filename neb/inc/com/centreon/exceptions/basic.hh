/*
** Copyright 2011-2014 Centreon
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

#ifndef CC_EXCEPTIONS_BASIC_HH
#define CC_EXCEPTIONS_BASIC_HH

#include <exception>
#include "com/centreon/misc/stringifier.hh"

CC_BEGIN()

namespace exceptions {
/**
 *  @class basic basic.hh "com/centreon/exceptions/basic.hh"
 *  @brief Base exception class.
 *
 *  Simple exception class containing an basic error message.
 */
class basic : public std::exception {
 public:
  basic();
  basic(char const* file, char const* function, int line);
  basic(basic const& other);
  virtual ~basic() throw();
  virtual basic& operator=(basic const& other);
  template <typename T>
  basic& operator<<(T t) {
    _buffer << t;
    return (*this);
  }
  virtual char const* what() const throw();

 private:
  void _internal_copy(basic const& other);

  misc::stringifier _buffer;
};
}

CC_END()

#if defined(__GNUC__)
#define FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define FUNCTION __FUNCSIG__
#else
#define FUNCTION __func__
#endif  // GCC, Visual or other.

#ifndef NDEBUG
#define basic_error() \
  com::centreon::exceptions::basic(__FILE__, FUNCTION, __LINE__)
#else
#define basic_error() com::centreon::exceptions::basic()
#endif  // !NDEBUG

#endif  // !CC_EXCEPTIONS_BASIC_HH
