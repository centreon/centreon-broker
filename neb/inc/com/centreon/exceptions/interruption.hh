/*
** Copyright 2014 Centreon
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

#ifndef CC_EXCEPTIONS_INTERRUPTION_HH
#define CC_EXCEPTIONS_INTERRUPTION_HH

#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace exceptions {
/**
 *  @class interruption interruption.hh
 *"com/centreon/exceptions/interruption.hh"
 *  @brief Exception signaling an interruption in processing.
 *
 *  Some operation that was in progress was interrupted but did not
 *  fail. This is mostly used to warn users of an errno of EINTR
 *  during a syscall.
 */
class interruption : public basic {
 public:
  interruption();
  interruption(char const* file, char const* function, int line);
  interruption(interruption const& other);
  virtual ~interruption() throw();
  interruption& operator=(interruption const& other);
  template <typename T>
  interruption& operator<<(T t) {
    basic::operator<<(t);
    return (*this);
  }
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
#define interruption_error() \
  com::centreon::exceptions::basic(__FILE__, FUNCTION, __LINE__)
#else
#define interruption_error() com::centreon::exceptions::basic()
#endif  // !NDEBUG

#endif  // !CC_EXCEPTIONS_INTERRUPTION_HH
