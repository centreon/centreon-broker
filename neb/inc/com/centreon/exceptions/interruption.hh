/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_EXCEPTIONS_INTERRUPTION_HH
#  define CC_EXCEPTIONS_INTERRUPTION_HH

#  include "com/centreon/exceptions/basic.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace         exceptions {
  /**
   *  @class interruption interruption.hh "com/centreon/exceptions/interruption.hh"
   *  @brief Exception signaling an interruption in processing.
   *
   *  Some operation that was in progress was interrupted but did not
   *  fail. This is mostly used to warn users of an errno of EINTR
   *  during a syscall.
   */
  class           interruption : public basic {
  public:
                  interruption();
                  interruption(
                    char const* file,
                    char const* function,
                    int line);
                  interruption(interruption const& other);
    virtual       ~interruption() throw ();
    interruption& operator=(interruption const& other);
    template <typename T>
    interruption& operator<<(T t) {
      basic::operator<<(t);
      return (*this);
    }
  };
}

CC_END()

#  if defined(__GNUC__)
#    define FUNCTION __PRETTY_FUNCTION__
#  elif defined(_MSC_VER)
#    define FUNCTION __FUNCSIG__
#  else
#    define FUNCTION __func__
#  endif // GCC, Visual or other.

#  ifndef NDEBUG
#    define interruption_error() com::centreon::exceptions::basic( \
                                 __FILE__,                         \
                                 FUNCTION,                         \
                                 __LINE__)
#  else
#    define interruption_error() com::centreon::exceptions::basic()
#  endif // !NDEBUG

#endif // !CC_EXCEPTIONS_INTERRUPTION_HH
