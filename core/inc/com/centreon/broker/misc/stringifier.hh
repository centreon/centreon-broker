/*
** Copyright 2009-2011 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_MISC_STRINGIFIER_HH_
#  define CCB_MISC_STRINGIFIER_HH_

#  include <QString>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                     misc {
  /**
   *  @class stringifier stringifier.hh "com/centreon/misc/stringifier.hh"
   *  @brief Provide method to converting data to string.
   *
   *  Stringifier is a simple way to convert different data type to
   *  C-String (null pointer terminated).
   */
  class                       stringifier {
   public:
                              stringifier(char const* buffer = NULL) throw ();
                              stringifier(stringifier const& right);
    virtual                   ~stringifier();
    stringifier&              operator=(stringifier const& right);
    stringifier&              operator<<(bool b) throw ();
    stringifier&              operator<<(char const* str) throw ();
    stringifier&              operator<<(char c) throw ();
    stringifier&              operator<<(double d) throw ();
    stringifier&              operator<<(int i) throw ();
    stringifier&              operator<<(long long ll) throw ();
    stringifier&              operator<<(long l) throw ();
    stringifier&              operator<<(std::string const& str) throw ();
    stringifier&              operator<<(stringifier const& str) throw ();
    stringifier&              operator<<(unsigned int u) throw ();
    stringifier&              operator<<(unsigned long long ull) throw ();
    stringifier&              operator<<(unsigned long ul) throw ();
    stringifier&              operator<<(void const* p) throw ();
    stringifier&              operator<<(QString const& q) throw();
    stringifier&              append(char const* str,
                                     unsigned int size) throw ();
    char const*               data() const throw ();
    int                       precision() const throw ();
    void                      precision(int val) throw ();
    void                      reset() throw ();
    unsigned int              size() const throw ();

  protected:
    template     <typename T>
    stringifier&              _insert(char const* format, T t) throw ();
    template     <typename T>
    stringifier&              _insert(
          char const* format,
          unsigned int limit,
          T t) throw ();
    stringifier&              _internal_copy(stringifier const& right);
    bool                      _realloc(unsigned int new_size) throw ();

    char*                     _buffer;
    unsigned int              _current;
    int                       _precision;
    unsigned int              _size;
    static unsigned int const _static_buffer_size = 1024;
    char                      _static_buffer[_static_buffer_size];
  };
}

CCB_END()

#endif /* !CCB_MISC_STRINGIFIER_HH_ */
